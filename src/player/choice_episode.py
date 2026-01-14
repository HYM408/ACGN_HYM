from PySide6.QtCore import Qt
from PySide6.QtGui import QFont
from PySide6.QtWidgets import QPushButton, QVBoxLayout, QLabel, QFrame, QHBoxLayout, QSpacerItem, QSizePolicy
from src.player.css import VideoCrawler
from src.sqlite import get_by_subject_id
from src.thread_manager import thread_manager
from src.player.player_ui import play_video_in_player


class ChoiceEpisodeManager:
    def __init__(self, main_window):
        self.main_window = main_window
        self.site_widgets = {}
        self.current_episode_data = None
        self.crawler = VideoCrawler()

    def show_player_page(self, episode_data):
        print(episode_data)
        player_widget = self.main_window.loaded_pages["player"]
        self.main_window.main_stackedWidget.setCurrentWidget(player_widget)
        self.current_episode_data = episode_data
        self.get_name_and_fetch_routes(episode_data)

    def get_name_and_fetch_routes(self, episode_data):
        """获取subject_id"""
        if 'episode' in episode_data and isinstance(episode_data['episode'], dict):
            subject_id = episode_data['episode'].get('subject_id')
            if 'sort' not in episode_data and 'episode' in episode_data:
                episode_data['sort'] = episode_data['episode'].get('sort')
        else:
            subject_id = episode_data.get('subject_id')
        collection_data = get_by_subject_id(subject_id)
        keyword = collection_data.get('subject_name_cn') or collection_data.get('subject_name')
        if keyword:
            self.fetch_route_names(keyword)

    def fetch_route_names(self, keyword):
        """获取视频线路"""
        player_widget = self.main_window.loaded_pages["player"]
        container = player_widget.ui.scrollAreaWidgetContents
        # 清空容器
        layout = container.layout()
        if layout:
            while layout.count():
                item = layout.takeAt(0)
                if item.widget():
                    item.widget().deleteLater()
        # 创建主布局
        main_layout = QVBoxLayout(container)
        main_layout.setContentsMargins(10, 10, 10, 10)
        main_layout.setSpacing(5)
        # 站点
        self.sites_container = QFrame(container)
        self.sites_layout = QVBoxLayout(self.sites_container)
        self.sites_layout.setSpacing(5)
        main_layout.addWidget(self.sites_container)
        # 底部弹簧
        spacer = QSpacerItem(20, 40, QSizePolicy.Minimum, QSizePolicy.Expanding)
        main_layout.addItem(spacer)
        # 获取所有站点
        site_ids = sorted(list(self.crawler.site_configs.keys()))
        # 创建初始卡片
        self.site_widgets.clear()
        for site_id in site_ids:
            site_widget = self.create_site_widget(site_id, 'loading', None)
            self.site_widgets[site_id] = site_widget
            self.sites_layout.addWidget(site_widget)
        # 搜索线程
        thread_manager.site_search_completed.connect(self.update_site_widget)
        thread_manager.search_sites(site_ids, keyword, self.crawler)

    def create_site_widget(self, site_id, status, result):
        """创建站点卡片"""
        site_frame = QFrame()
        layout = QVBoxLayout(site_frame)
        layout.setContentsMargins(15, 15, 15, 15)
        layout.setSpacing(10)
        # 站点
        top_layout = QHBoxLayout()
        top_layout.setSpacing(10)
        # 站点名称
        site_label = QLabel(site_id, site_frame)
        site_label.setFont(QFont("", 14, QFont.Bold))
        site_label.setStyleSheet("color: #333;")
        site_label.setSizePolicy(QSizePolicy.Minimum, QSizePolicy.Preferred)
        top_layout.addWidget(site_label)
        # 来源信息
        source_label = QLabel(site_frame)
        source_label.setStyleSheet("color: #666; font-size: 13px;")
        source_label.setWordWrap(True)
        source_label.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Preferred)
        if status == 'loading':
            source_label.setText("搜索中...")
        elif status == 'success':
            source_label.setText(result['title'])
        else:
            source_label.setText("✗")
        top_layout.addWidget(source_label, 1)
        layout.addLayout(top_layout)
        # 线路
        if status == 'success' and result.get('routes'):
            routes_layout = QHBoxLayout()
            routes_layout.setAlignment(Qt.AlignmentFlag.AlignLeft)
            routes_layout.setSpacing(5)
            for route in result['routes']:
                button = QPushButton(route['route'], site_frame)
                button.setMinimumHeight(35)
                button.setToolTip(f"站点: {site_id}\n线路: {route['route']}\n剧集数: {len(route['episodes'])}")
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
                button.clicked.connect(lambda checked, s=site_id, r=route: self.handle_route_click(s, r))
                routes_layout.addWidget(button)
            layout.addLayout(routes_layout)
        return site_frame

    def handle_route_click(self, site_id, route):
        """点击线路"""
        sort = None
        if 'sort' in self.current_episode_data:
            sort = self.current_episode_data.get('sort')
        elif 'episode' in self.current_episode_data and isinstance(self.current_episode_data['episode'], dict):
            sort = self.current_episode_data['episode'].get('sort')
        try:
            episode_index = int(sort) - 1
            if episode_index < 0 or episode_index >= len(route['episodes']):
                print(f"错误：sort值 {sort} 超出范围（1-{len(route['episodes'])}）")
                return
            episode = route['episodes'][episode_index]
            episode_url = episode['link']
            print(f"开始获取视频链接: {episode_url}")
            thread_manager.fetch_video_url(episode_url, site_id, self.crawler, self.on_video_fetched)
        except (ValueError, IndexError) as e:
            print(f"错误：{str(e)}")

    def on_video_fetched(self, video_url):
        """视频链接获取完成的回调"""
        if video_url:
            print(f"链接:{video_url}")
            player_widget = self.main_window.loaded_pages["player"]
            self.main_window.main_stackedWidget.setCurrentWidget(player_widget)
            if play_video_in_player(player_widget, video_url):
                print("视频开始播放")
            else:
                print("视频播放失败")
        else:
            print("获取视频链接失败")

    def update_site_widget(self, site_data):
        """更新站点组件"""
        site_id = site_data['site_id']
        status = site_data['status']
        result = site_data['result']
        new_widget = self.create_site_widget(site_id, status, result)
        if site_id in self.site_widgets:
            old_widget = self.site_widgets[site_id]
            index = self.sites_layout.indexOf(old_widget)
            if index >= 0:
                self.sites_layout.removeWidget(old_widget)
                old_widget.deleteLater()
                self.sites_layout.insertWidget(index, new_widget)
                self.site_widgets[site_id] = new_widget