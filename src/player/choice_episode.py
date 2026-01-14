from typing import Dict, Any, Optional
from PySide6.QtCore import Qt
from PySide6.QtGui import QFont
from PySide6.QtWidgets import QPushButton, QVBoxLayout, QLabel, QFrame, QHBoxLayout, QSpacerItem, QSizePolicy, QWidget
from src.player.css import VideoCrawler
from src.sqlite import get_by_subject_id
from src.thread_manager import thread_manager
from src.player.player_ui_manager import play_video_in_player


class EpisodeData:
    """剧集数据封装"""
    def __init__(self, data: Dict[str, Any]):
        self.raw_data = data
        self.subject_id = self._extract_subject_id(data)
        self.sort = self._extract_sort(data)

    def _extract_subject_id(self, data: Dict[str, Any]) -> Optional[int]:
        """提取subject_id"""
        if 'episode' in data and isinstance(data['episode'], dict):
            return data['episode'].get('subject_id')
        return data.get('subject_id')

    def _extract_sort(self, data: Dict[str, Any]) -> Optional[int]:
        """提取sort值"""
        if 'sort' in data:
            return data['sort']
        elif 'episode' in data and isinstance(data['episode'], dict):
            return data['episode'].get('sort')
        return None

    def to_dict(self) -> Dict[str, Any]:
        """转换为字典"""
        return self.raw_data


class SiteWidgetFactory:
    """站点卡片"""
    @staticmethod
    def create_site_card(site_id: str, status: str, result: Optional[Dict] = None) -> QFrame:
        """创建站点卡片"""
        card = QFrame()
        card.setObjectName(f"site_card_{site_id}")
        layout = QVBoxLayout(card)
        layout.setContentsMargins(15, 15, 15, 15)
        layout.setSpacing(10)
        top_layout = QHBoxLayout()
        top_layout.setSpacing(10)
        # 站点名称
        site_label = QLabel(site_id, card)
        site_label.setFont(QFont("", 14, QFont.Bold))
        site_label.setStyleSheet("color: #333;")
        site_label.setSizePolicy(QSizePolicy.Minimum, QSizePolicy.Preferred)
        top_layout.addWidget(site_label)
        # 状态/标题信息
        status_label = QLabel(card)
        status_label.setStyleSheet("color: #666; font-size: 13px;")
        status_label.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Preferred)
        if status == 'loading':
            status_label.setText("搜索中...")
        elif status == 'success' and result:
            status_label.setText(result.get('title', ''))
        else:
            status_label.setText("✗")
        top_layout.addWidget(status_label, 1)
        layout.addLayout(top_layout)
        # 线路
        if status == 'success' and result and result.get('routes'):
            routes_layout = QHBoxLayout()
            routes_layout.setAlignment(Qt.AlignmentFlag.AlignLeft)
            routes_layout.setSpacing(5)
            for route in result['routes']:
                button = SiteWidgetFactory._create_route_button(site_id, route, card)
                routes_layout.addWidget(button)
            layout.addLayout(routes_layout)
        return card

    @staticmethod
    def _create_route_button(site_id: str, route: Dict[str, Any], parent: QWidget) -> QPushButton:
        """创建线路按钮"""
        button = QPushButton(route['route'], parent)
        button.setMinimumHeight(35)
        button.setToolTip(
            f"站点: {site_id}\n"
            f"线路: {route['route']}\n"
            f"剧集数: {len(route.get('episodes', []))}"
        )
        button.setStyleSheet("""
            QPushButton {
                padding: 5px 10px;
                font-size: 13px;
                border: 1px solid #ddd;
                border-radius: 5px;
                background-color: white;
            }
            QPushButton:hover {
                background-color: #f5f5f5;
                border-color: #aaa;
            }
        """)
        return button


class ChoiceEpisodeManager:
    """线路选择管理器"""
    def __init__(self, main_window):
        self.main_window = main_window
        self.site_widgets = {}
        self.current_episode: Optional[EpisodeData] = None
        self.crawler = VideoCrawler()

    def show_player_page(self, episode_data: Dict[str, Any]):
        """显示播放器页面"""
        print(f"显示播放器页面: {episode_data}")
        current_index = self.main_window.main_stackedWidget.currentIndex()
        self.main_window.page_history.append(current_index)

        player_widget = self.main_window.loaded_pages["player"]
        self.main_window.main_stackedWidget.setCurrentWidget(player_widget)
        self.current_episode = EpisodeData(episode_data)
        self._fetch_video_routes()

    def _fetch_video_routes(self):
        """获取视频线路"""
        collection_data = get_by_subject_id(self.current_episode.subject_id)
        keyword = collection_data.get('subject_name_cn') or collection_data.get('subject_name')
        player_widget = self.main_window.loaded_pages["player"]
        container = player_widget.ui.scrollAreaWidgetContents
        self._clear_container_layout(container)
        site_ids = sorted(self.crawler.site_configs.keys())
        self.site_widgets = {}
        for site_id in site_ids:
            widget = SiteWidgetFactory.create_site_card(site_id, 'loading')
            self.site_widgets[site_id] = widget
            container.layout().addWidget(widget)
        container.layout().addItem(QSpacerItem(20, 40, QSizePolicy.Minimum, QSizePolicy.Expanding))
        thread_manager.site_search_completed.connect(self._update_site_widget)
        thread_manager.search_sites(site_ids, keyword, self.crawler)

    def _clear_container_layout(self, container: QWidget):
        """清空容器布局"""
        if layout := container.layout():
            QWidget().setLayout(layout)
        container.setLayout(QVBoxLayout())
        container.layout().setContentsMargins(10, 10, 10, 10)

    def _update_site_widget(self, site_data: Dict[str, Any]):
        """更新站点组件"""
        site_id = site_data['site_id']
        status = site_data['status']
        result = site_data['result']
        if site_id not in self.site_widgets:
            return
        new_widget = SiteWidgetFactory.create_site_card(site_id, status, result)
        old_widget = self.site_widgets[site_id]
        if status == 'success' and result and result.get('routes'):
            for i, route in enumerate(result['routes']):
                button = new_widget.findChildren(QPushButton)[i]
                button.clicked.connect(lambda checked, s=site_id, r=route: self._on_route_selected(s, r))
        if layout := old_widget.parent().layout():
            index = layout.indexOf(old_widget)
            if index >= 0:
                layout.removeWidget(old_widget)
                layout.insertWidget(index, new_widget)
                old_widget.deleteLater()
                self.site_widgets[site_id] = new_widget

    def _on_route_selected(self, site_id: str, route: Dict[str, Any]):
        """处理线路选择"""
        try:
            episode_index = int(self.current_episode.sort) - 1
            episodes = route.get('episodes', [])
            if not episodes:
                print("线路中没有剧集")
                return
            if episode_index < 0 or episode_index >= len(episodes):
                print(f"sort值 {self.current_episode.sort} 超出范围 (1-{len(episodes)})")
                return
            episode = episodes[episode_index]
            episode_url = episode.get('link')
            if not episode_url:
                print("剧集链接为空")
                return
            print(f"开始获取视频:{episode_url}")
            thread_manager.fetch_video_url(episode_url, site_id, self.crawler, self._on_video_fetched)
        except (ValueError, IndexError, KeyError) as e:
            print(f"错误:{str(e)}")

    def _on_video_fetched(self, video_url: str):
        """视频链接获取完成的回调"""
        if not video_url:
            print("获取视频链接失败")
            return
        print(f"获取到视频链接: {video_url}")
        player_widget = self.main_window.loaded_pages["player"]
        if hasattr(self.main_window, 'video_player') and self.main_window.video_player:
            success = self.main_window.video_player.play_video(video_url)
        else:
            success = play_video_in_player(player_widget, video_url)
        if success:
            print("视频开始播放")
        else:
            print("视频播放失败")