import re
import json
import httpx
from lxml import html
from pathlib import Path
from urllib.parse import urljoin
from difflib import SequenceMatcher
from PySide6.QtCore import QUrl, QTimer, QEventLoop
from PySide6.QtWebEngineWidgets import QWebEngineView
from PySide6.QtWebEngineCore import QWebEngineUrlRequestInterceptor


class VideoStreamDetector:
    """视频流检测器"""
    def __init__(self, video_patterns):
        self.video_patterns = video_patterns
        self.video_url = None
        self.event_loop = None
        self.view = None

    def detect(self, episode_url):
        """检测视频流"""
        self.video_url = None
        self.event_loop = QEventLoop()
        self.view = QWebEngineView()
        class StreamInterceptor(QWebEngineUrlRequestInterceptor):
            def __init__(self, detector):
                super().__init__()
                self.detector = detector
            def interceptRequest(self, info):
                url = info.requestUrl().toString()
                for pattern in self.detector.video_patterns:
                    if pattern in url.lower():
                        print(f"获取到视频链接: {url}")
                        self.detector.video_url = url
                        if self.detector.event_loop:
                            self.detector.event_loop.quit()
        interceptor = StreamInterceptor(self)
        self.view.page().profile().setUrlRequestInterceptor(interceptor)
        QTimer.singleShot(20000, self._on_timeout)
        self.view.load(QUrl(episode_url))
        self.event_loop.exec()
        self._cleanup()
        return self.video_url

    def _on_timeout(self):
        """超时处理"""
        if self.event_loop and self.event_loop.isRunning():
            self.event_loop.quit()

    def _cleanup(self):
        """清理资源"""
        if self.view:
            self.view.close()
            self.view.deleteLater()
            self.view = None
        self.event_loop = None


class VideoCrawler:
    """网站源"""
    def __init__(self, config_file="css.json"):
        """初始化爬虫"""
        config_path = Path(__file__).parent / config_file
        self.config = json.loads(config_path.read_text(encoding='utf-8'))
        self.site_configs = self.config.get("site_configs", {})
        self.headers = {
            'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36',
            'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8',
            'Accept-Language': 'zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2',
            'Accept-Encoding': 'gzip, deflate, br',
            'Connection': 'keep-alive',
            'Upgrade-Insecure-Requests': '1'
        }

    def _make_request(self, url):
        """统一请求处理"""
        try:
            response = httpx.get(url, headers=self.headers, timeout=10, follow_redirects=True)
            response.encoding = 'utf-8'
            return html.fromstring(response.text)
        except Exception as e:
            print(f"失败: {url} - {e}")
            return None

    def search_site(self, keyword, site_id):
        """搜索指定站点的视频"""
        site_config = self.site_configs.get(site_id)
        url = site_config["base_url"] + site_config["search_path"].replace("{keyword}", keyword)
        tree = self._make_request(url)
        if tree is None:
            return []
        titles = tree.cssselect(site_config["title"])
        links = tree.cssselect(site_config["link"])
        if not titles or not links:
            return []
        results = []
        for title_elem, link_elem in zip(titles, links):
            title = title_elem.text_content().strip()
            link = urljoin(site_config["base_url"], link_elem.get('href'))
            if link.startswith("http://"):
                link = link.replace("http://", "https://", 1)
            similarity = SequenceMatcher(None, keyword, title).ratio()
            results.append({'title': title, 'link': link, 'similarity': similarity})
        sorted_results = sorted(results, key=lambda x: x['similarity'], reverse=True)
        final_results = []
        for result in sorted_results:
            try:
                routes = self.get_routes(result['link'], site_id)
                if routes:
                    final_results.append({'title': result['title'], 'link': result['link'], 'similarity': result['similarity'], 'site': site_id, 'routes': routes})
            except Exception as e:
                print(f"获取线路失败: {e}")
                continue
        return final_results

    def get_routes(self, page_url, site_id):
        """获取视频线路和集数"""
        site_config = self.site_configs.get(site_id)
        tree = self._make_request(page_url)
        if tree is None:
            return []
        routes = []
        route_tabs = tree.cssselect(site_config["route_tabs"])
        episode_containers = tree.cssselect(site_config["episode_containers"])
        for route_tab, container in zip(route_tabs, episode_containers):
            episodes = []
            for ep in container.cssselect(site_config["episode_items"]):
                episode_link = urljoin(page_url, ep.get('href'))
                episodes.append({'name': ep.text_content().strip(), 'link': episode_link})
            if episodes:
                routes.append({'route': route_tab.text_content().strip(), 'episodes': episodes})
        return routes

    @staticmethod
    def _video_url_processing(video_url):
        """加工URL"""
        if not video_url:
            return video_url
        matches = list(re.finditer(r'https?://', video_url))
        if len(matches) >= 2:
            start_pos = matches[1].start()
            video_url = video_url[start_pos:]
            print(f"处理后视频链接: {video_url}")
        return video_url

    def find_video_stream(self, episode_url, site_id):
        """捕获视频播放链接"""
        site_config = self.site_configs.get(site_id)
        if not site_config:
            return None
        detector = VideoStreamDetector(site_config["video_patterns"])
        video_url = self._video_url_processing(detector.detect(episode_url))
        return video_url