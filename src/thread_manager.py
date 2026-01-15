import httpx
import threading
import webbrowser
import xml.etree.ElementTree as ET
from typing import Optional, Callable
from PySide6.QtCore import QThreadPool, QRunnable, QObject, Signal, QTimer
from src.player.css import VideoCrawler
from src.api import BangumiAPI, BangumiOAuth
from src.config import get_config_item, set_config_items
from src.sqlite import insert_many_data, insert_many_episodes, update_all_episodes_status, update_field


class TaskResult(QObject):
    """任务结果持有器"""
    episodes_fetched = Signal(list)
    mark_finished = Signal(bool)
    search_finished = Signal(list)
    search_error = Signal(str)
    auth_success = Signal()
    auth_error = Signal(str)
    collection_ready = Signal(list)
    collection_error = Signal()
    status_updated = Signal(bool, str)
    subject_data_fetched = Signal(dict)
    rss_update_finished = Signal(bool, str)
    site_search_completed = Signal(dict)
    video_fetched = Signal(str)


class BaseTask(QRunnable):
    """基础任务类"""

    def __init__(self, result_holder: TaskResult = None):
        super().__init__()
        self.setAutoDelete(True)
        self.result_holder = result_holder or TaskResult()
        self._is_cancelled = False

    def run(self):
        raise NotImplementedError("子类必须实现run方法")

    def cancel(self):
        """取消任务"""
        self._is_cancelled = True


# ====================获取视频链接====================
class VideoFetchTask(BaseTask):
    """获取视频链接"""
    def __init__(self, episode_url: str, site_id: str, crawler: VideoCrawler):
        super().__init__()
        self.episode_url = episode_url
        self.site_id = site_id
        self.crawler = crawler

    def run(self):
        try:
            if self._is_cancelled:
                return
            video_url = self.crawler.find_video_stream(self.episode_url, self.site_id)
            if video_url and not self._is_cancelled:
                if 'url=' in video_url:
                    start = video_url.find('url=') + 4
                    end = video_url.find('&', start)
                    if end == -1:
                        end = len(video_url)
                    video_url = video_url[start:end]
                self.result_holder.video_fetched.emit(video_url)
            elif not self._is_cancelled:
                self.result_holder.video_fetched.emit("")
        except Exception as e:
            print(f"获取视频链接失败: {e}")
            if not self._is_cancelled:
                self.result_holder.video_fetched.emit("")


# ====================站点搜索====================
class SiteSearchTask(BaseTask):
    """站点搜索任务"""
    def __init__(self, site_id: str, keyword: str, crawler: VideoCrawler):
        super().__init__()
        self.site_id = site_id
        self.keyword = keyword
        self.crawler = crawler

    def run(self):
        try:
            result = self.crawler.search_site(self.keyword, self.site_id)
            status = 'success' if result and result.get('routes') else 'failed'
            self.result_holder.site_search_completed.emit({'site_id': self.site_id, 'status': status, 'result': result})
        except Exception as e:
            print(f"站点 {self.site_id} 搜索失败: {e}")
            self.result_holder.site_search_completed.emit({'site_id': self.site_id, 'status': 'failed', 'result': None})


# ====================RSS====================
class RSSWorker(QObject):
    """RSS工作器，管理RSS更新逻辑"""
    update_finished = Signal(bool, str)
    refresh_needed = Signal()

    def __init__(self):
        super().__init__()
        self.is_running = False
        self.last_update_time = 0
        self._lock = threading.Lock()
        self.api = BangumiAPI()

    def execute_update(self):
        """执行RSS更新"""
        print("RSS: 启动")
        with self._lock:
            if self.is_running:
                return
            self.is_running = True
        task = RSSUpdateTask()
        task.result_holder.rss_update_finished.connect(self._on_rss_update_complete)
        QThreadPool.globalInstance().start(task)

    def _on_rss_update_complete(self, success: bool, message: str):
        """RSS更新完成回调"""
        with self._lock:
            self.is_running = False
        self.update_finished.emit(success, message)
        if success and "RSS更新完成" in message:
            self.refresh_needed.emit()


class RSSUpdateTask(BaseTask):
    """RSS更新任务"""
    def __init__(self):
        super().__init__()
        self.api = BangumiAPI()

    def run(self):
        try:
            success = self._update_rss_guid()
            if success:
                self.result_holder.rss_update_finished.emit(True, "RSS更新完成")
            else:
                self.result_holder.rss_update_finished.emit(False, "无新内容")
        except httpx.TimeoutException as e:
            self.result_holder.rss_update_finished.emit(False, f"RSS更新超时: {str(e)}")
        except Exception as e:
            self.result_holder.rss_update_finished.emit(False, f"RSS更新失败: {str(e)}")

    def _update_rss_guid(self) -> bool:
        """更新RSS GUID"""
        old_guid = get_config_item("rss_guid")
        rss_content = self._get_user_rss()
        if not rss_content:
            return False
        new_guid = self._extract_first_guid_number(rss_content)
        if old_guid is None:
            set_config_items(rss_guid=str(new_guid))
            return True
        if old_guid == new_guid:
            return False
        set_config_items(rss_guid=str(new_guid))
        self._sync_collections()
        return True

    def _get_user_rss(self) -> Optional[str]:
        """获取用户RSS"""
        user_id = get_config_item("user_id")
        base_url = get_config_item("bangumi_base_url")
        url = f"{base_url}feed/user/{user_id}/timeline?type=subject"
        try:
            response = httpx.get(url, headers={"User-Agent": "ACGN_HYM/1.0"}, timeout=20.0)
            response.raise_for_status()
            return response.text
        except Exception as e:
            print(f"获取RSS失败: {e}")
            return None

    def _extract_first_guid_number(self, rss_content: str) -> Optional[str]:
        """提取GUID"""
        if not rss_content:
            return None
        try:
            root = ET.fromstring(rss_content)
            first_item = root.find('.//item')
            if first_item is None:
                return None
            guid_elem = first_item.find('guid')
            if guid_elem is None or guid_elem.text is None:
                return None
            last_part = guid_elem.text.strip().split('/')[-1]
            return last_part if last_part.isdigit() else None
        except Exception as e:
            print(f"解析RSS失败: {e}")
            return None

    def _sync_collections(self) -> bool:
        """同步收藏"""
        try:
            collections = self.api.get_user_collections(get_all=False)
            if collections is None:
                return False
            insert_many_data(collections)
            return True
        except Exception as e:
            print(f"同步收藏失败: {e}")
            return False


# ====================剧集相关====================
class EpisodeFetcher(BaseTask):
    """剧集获取任务"""
    def __init__(self, subject_id: int):
        super().__init__()
        self.subject_id = subject_id
        self.api = BangumiAPI()

    def run(self):
        try:
            episodes = self.api.get_subject_episodes(self.subject_id)
            if episodes is not None:
                insert_many_episodes(self.subject_id, episodes)
                self.result_holder.episodes_fetched.emit(episodes)
            else:
                self.result_holder.episodes_fetched.emit([])
        except Exception as e:
            print(f"获取剧集失败: {e}")
            self.result_holder.episodes_fetched.emit([])


class EpisodeMarker(BaseTask):
    """剧集标记任务"""
    def __init__(self, subject_id: int, episodes: list):
        super().__init__()
        self.subject_id = subject_id
        self.episodes = episodes
        self.api = BangumiAPI()

    def run(self):
        try:
            episode_ids = []
            for ep in self.episodes:
                if 'episode' in ep:
                    ep_id = ep.get('episode', {}).get('id')
                else:
                    ep_id = ep.get('id')
                if ep_id:
                    episode_ids.append(ep_id)
            if episode_ids:
                self.api.update_subject_episodes(self.subject_id, {"episode_id": episode_ids, "type": 2})
            update_all_episodes_status(self.subject_id, 2)
            for ep in self.episodes:
                if 'episode' in ep:
                    ep['type'] = 2
                else:
                    ep['collection_type'] = 2
            self.result_holder.mark_finished.emit(True)
        except Exception as e:
            print(f"标记剧集失败: {e}")
            self.result_holder.mark_finished.emit(False)


# ====================搜索相关====================
class SubjectSearcher(BaseTask):
    """条目搜索任务"""
    def __init__(self, keyword: str = None, tag: str = None, search_type: int = 2):
        super().__init__()
        self.keyword = keyword
        self.tag = tag
        self.search_type = search_type
        self.api = BangumiAPI()

    def run(self):
        try:
            results = self.api.search_subjects(keyword=self.keyword, tag=self.tag, subject_type=self.search_type)
            self.result_holder.search_finished.emit(results or [])
        except Exception as e:
            self.result_holder.search_error.emit(str(e))


# ====================认证相关====================
class AuthWorker(BaseTask):
    """认证任务"""
    def __init__(self):
        super().__init__()

    def run(self):
        try:
            oauth = BangumiOAuth()
            auth_url = oauth.generate_auth_url()
            webbrowser.open(auth_url)
            code = oauth.listen_for_code(timeout=60)
            if not code:
                self.result_holder.auth_error.emit("授权超时")
                return
            token_data = oauth.exchange_code_for_token(code=code)
            if token_data:
                self.result_holder.auth_success.emit()
            else:
                self.result_holder.auth_error.emit("授权失败")
        except Exception as e:
            self.result_holder.auth_error.emit(f"授权失败: {str(e)}")


# ====================收藏相关====================
class CollectionFetcher(BaseTask):
    """收藏获取任务"""
    def __init__(self):
        super().__init__()
        self.api = BangumiAPI()

    def run(self):
        try:
            collections = self.api.get_user_collections()
            if collections is None:
                self.result_holder.collection_error.emit()
            else:
                self.result_holder.collection_ready.emit(collections)
        except Exception as e:
            print(f"获取收藏失败: {e}")
            self.result_holder.collection_error.emit()


# ====================状态更新相关====================
class StatusUpdater(BaseTask):
    """状态更新任务"""
    def __init__(self, subject_id: int, old_type: int, new_type: int):
        super().__init__()
        self.subject_id = subject_id
        self.old_type = old_type
        self.new_type = new_type
        self.api = BangumiAPI()

    def run(self):
        try:
            if self.old_type in [1, 2, 3, 4, 5]:
                update_field(self.subject_id, 'type', self.new_type)
                self.api.update_collection(self.subject_id, {"type": self.new_type})
            else:
                self.api.create_or_update_collection(self.subject_id, {"type": self.new_type})
                collection_data = self.api.get_user_collection(self.subject_id)
                if collection_data:
                    insert_many_data([collection_data])
            self.result_holder.status_updated.emit(True, None)
        except Exception as e:
            self.result_holder.status_updated.emit(False, str(e))


# ====================条目数据获取====================
class SubjectDataFetcher(BaseTask):
    """条目数据获取任务"""
    def __init__(self, subject_id: int):
        super().__init__()
        self.subject_id = subject_id

    def run(self):
        try:
            api = BangumiAPI()
            subject_data = api.get_subject_info(self.subject_id)
            self.result_holder.subject_data_fetched.emit(subject_data)
        except Exception as e:
            print(f"获取条目数据失败: {e}")
            self.result_holder.subject_data_fetched.emit({})


# ====================线程管理器====================
class ThreadManager(QObject):
    """统一的线程管理器，使用线程池处理短任务"""
    rss_update_finished = Signal(bool, str)
    episodes_fetched = Signal(list)
    episodes_marked = Signal(bool)
    search_finished = Signal(list)
    search_error = Signal(str)
    auth_success = Signal()
    auth_error = Signal(str)
    collection_ready = Signal(list)
    collection_error = Signal()
    status_updated = Signal(bool, str)
    subject_data_fetched = Signal(dict)
    refresh_main_page = Signal()
    site_search_completed = Signal(dict)
    video_fetched = Signal(str)

    def __init__(self, parent=None):
        super().__init__(parent)
        self.thread_pool = QThreadPool.globalInstance()
        self.thread_pool.setMaxThreadCount(5)
        self.thread_pool.setExpiryTimeout(30000)
        self.rss_timer = None
        self.rss_worker = None
        self.current_video_fetch_task = None
        self.video_fetch_connections = set()

    def setup_rss_timer(self):
        """设置RSS定时器"""
        self.rss_worker = RSSWorker()
        self.rss_worker.update_finished.connect(self.rss_update_finished)
        self.rss_worker.refresh_needed.connect(self.refresh_main_page)
        self.rss_timer = QTimer()
        self.rss_timer.setInterval(5 * 60 * 1000)
        self.rss_timer.timeout.connect(self.rss_worker.execute_update)
        QTimer.singleShot(5000, self.rss_worker.execute_update)
        self.rss_timer.start()

    def stop_rss_service(self):
        """停止RSS服务"""
        if self.rss_timer:
            self.rss_timer.stop()
            self.rss_timer.deleteLater()
            self.rss_timer = None
        if self.rss_worker:
            self.rss_worker.deleteLater()
            self.rss_worker = None

    # =================== 短任务接口 ===================

    def fetch_episodes(self, subject_id: int):
        """获取剧集数据"""
        task = EpisodeFetcher(subject_id)
        task.result_holder.episodes_fetched.connect(self.episodes_fetched)
        self.thread_pool.start(task)

    def mark_episodes_watched(self, subject_id: int, episodes: list):
        """标记剧集为已看"""
        task = EpisodeMarker(subject_id, episodes)
        task.result_holder.mark_finished.connect(self.episodes_marked)
        self.thread_pool.start(task)

    def search_subjects(self, keyword: str = None, tag: str = None, search_type: int = 2):
        """搜索条目"""
        task = SubjectSearcher(keyword, tag, search_type)
        task.result_holder.search_finished.connect(self.search_finished)
        task.result_holder.search_error.connect(self.search_error)
        self.thread_pool.start(task)

    def start_auth(self):
        """启动Bangumi认证"""
        task = AuthWorker()
        task.result_holder.auth_success.connect(self.auth_success)
        task.result_holder.auth_error.connect(self.auth_error)
        self.thread_pool.start(task)

    def fetch_collections(self):
        """获取收藏"""
        task = CollectionFetcher()
        task.result_holder.collection_ready.connect(self.collection_ready)
        task.result_holder.collection_error.connect(self.collection_error)
        self.thread_pool.start(task)

    def update_status(self, subject_id: int, old_type: int, new_type: int, callback: Callable):
        """更新收藏状态"""
        task = StatusUpdater(subject_id, old_type, new_type)
        task.result_holder.status_updated.connect(callback)
        task.result_holder.status_updated.connect(lambda success, _: success and self.refresh_main_page.emit())
        self.thread_pool.start(task)

    def fetch_subject_data(self, subject_id: int, callback: Callable):
        """获取条目数据"""
        task = SubjectDataFetcher(subject_id)
        task.result_holder.subject_data_fetched.connect(callback)
        self.thread_pool.start(task)

    def search_sites(self, site_ids: list, keyword: str, crawler: VideoCrawler):
        """搜索站点"""
        for site_id in site_ids:
            task = SiteSearchTask(site_id, keyword, crawler)
            task.result_holder.site_search_completed.connect(self.site_search_completed)
            self.thread_pool.start(task)

    def fetch_video_url(self, episode_url: str, site_id: str, crawler: VideoCrawler, callback: Callable):
        """获取视频链接"""
        self._cleanup_video_fetch_connections()
        if self.current_video_fetch_task:
            self.current_video_fetch_task.cancel()
            self.current_video_fetch_task = None
        task = VideoFetchTask(episode_url, site_id, crawler)

        def on_video_fetched(url):
            callback(url)
            self._cleanup_video_fetch_connections()
        connection = task.result_holder.video_fetched.connect(on_video_fetched)
        self.video_fetch_connections.add((task, connection))
        self.current_video_fetch_task = task
        self.thread_pool.start(task)

    def cancel_video_fetch(self):
        """取消当前的视频获取任务"""
        if self.current_video_fetch_task:
            self.current_video_fetch_task.cancel()
            self.current_video_fetch_task = None
        self._cleanup_video_fetch_connections()

    def _cleanup_video_fetch_connections(self):
        """清理视频获取信号连接"""
        for task, connection in self.video_fetch_connections:
            try:
                task.result_holder.video_fetched.disconnect(connection)
            except:
                pass
        self.video_fetch_connections.clear()

    def cleanup(self):
        """清理资源"""
        self.stop_rss_service()
        self.cancel_video_fetch()
        self.thread_pool.clear()
        self.thread_pool.waitForDone(3000)


thread_manager = ThreadManager()