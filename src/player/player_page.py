from PySide6.QtCore import Qt
from PySide6.QtGui import QFont
from PySide6.QtWidgets import QPushButton, QVBoxLayout, QLabel, QFrame, QHBoxLayout, QSpacerItem, QSizePolicy
from src.player.css import VideoCrawler
from src.sqlite import get_by_subject_id
from src.thread_manager import thread_manager


class ChoiceEpisodeManager:
    """线路选择"""
    def __init__(self, main_window):
        self.main_window = main_window
        self.site_widgets = {}
        self.current_episode = None
        self.crawler = VideoCrawler()
        thread_manager.site_search_completed.connect(self._update_site_widget)

    def show_player_page(self, episode_data):
        """显示播放器页面"""
        # subject_id
        subject_id = episode_data.get('subject_id')
        if not subject_id and 'episode' in episode_data:
            subject_id = episode_data['episode'].get('subject_id')
        # sort
        sort = episode_data.get('sort')
        if not sort and 'episode' in episode_data:
            sort = episode_data['episode'].get('sort')
        # 保存当前剧集信息
        self.current_episode = {'subject_id': subject_id, 'sort': sort}
        # 切换页面
        current_index = self.main_window.main_stackedWidget.currentIndex()
        self.main_window.page_history.append(current_index)
        player_widget = self.main_window.loaded_pages["player"]
        self.main_window.main_stackedWidget.setCurrentWidget(player_widget)
        # 获取视频线路
        self._fetch_video_routes()

    def _fetch_video_routes(self):
        """获取视频线路"""
        if not self.current_episode:
            return
        collection_data = get_by_subject_id(self.current_episode['subject_id'])
        keyword = collection_data.get('subject_name_cn') or collection_data.get('subject_name')
        player_widget = self.main_window.loaded_pages["player"]
        container = player_widget.ui.scrollAreaWidgetContents
        # 清空容器
        if layout := container.layout():
            while layout.count():
                item = layout.takeAt(0)
                if widget := item.widget():
                    widget.deleteLater()
        else:
            container.setLayout(QVBoxLayout())
        container.layout().setContentsMargins(10, 10, 10, 10)
        # 创建站点卡片
        site_ids = sorted(self.crawler.site_configs.keys())
        self.site_widgets = {}
        for site_id in site_ids:
            widget = self._create_site_card(site_id, 'loading')
            self.site_widgets[site_id] = widget
            container.layout().addWidget(widget)
        container.layout().addItem(QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding))
        # 开始搜索
        thread_manager.search_sites(site_ids, keyword, self.crawler)

    def _create_site_card(self, site_id, status, result=None):
        """创建站点卡片"""
        card = QFrame()
        card.setObjectName(f"site_card_{site_id}")
        layout = QVBoxLayout(card)
        layout.setContentsMargins(15, 15, 15, 15)
        layout.setSpacing(10)
        top_layout = QHBoxLayout()
        top_layout.setSpacing(8)  # 设置图标和站点名称之间的间距
        # 图标
        icon_label = QLabel(card)
        icon_label.setFixedSize(20, 20)
        icon_label.setScaledContents(True)
        site_config = self.crawler.site_configs.get(site_id, {})
        if icon_url := site_config.get('icon'):
            self.main_window.cache_manager.get_image_async(icon_url, lambda pixmap: self._set_icon(icon_label, pixmap))
        top_layout.addWidget(icon_label)

        # 站点名称
        site_label = QLabel(site_id, card)
        site_label.setFont(QFont("", 14, QFont.Weight.Bold))
        site_label.setStyleSheet("color: #333;")
        top_layout.addWidget(site_label)
        # 状态/标题信息
        status_label = QLabel(card)
        status_label.setStyleSheet("color: #666; font-size: 13px;")
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
                button = QPushButton(route['route'], card)
                button.setMinimumHeight(35)
                button.setStyleSheet("QPushButton {padding: 5px 10px;font-size: 13px;border: 1px solid #ddd;border-radius: 5px;background-color: white}"
                                     "QPushButton:hover {background-color: #f5f5f5;border-color: #aaa}")
                button.clicked.connect(lambda checked, s=site_id, r=route: self._on_route_selected(s, r))
                routes_layout.addWidget(button)
            layout.addLayout(routes_layout)
        return card

    def _set_icon(self, label, pixmap):
        """设置图标到标签"""
        if not pixmap.isNull():
            label.setPixmap(pixmap)

    def _update_site_widget(self, site_data):
        """更新站点组件"""
        site_id = site_data['site_id']
        status = site_data['status']
        result = site_data['result']
        if site_id not in self.site_widgets:
            return
        new_widget = self._create_site_card(site_id, status, result)
        if layout := self.site_widgets[site_id].parent().layout():
            index = layout.indexOf(self.site_widgets[site_id])
            if index >= 0:
                self.site_widgets[site_id].deleteLater()
                layout.insertWidget(index, new_widget)
                self.site_widgets[site_id] = new_widget

    def _on_route_selected(self, site_id, route):
        """处理线路选择"""
        try:
            episode_index = int(self.current_episode['sort']) - 1
            episodes = route.get('episodes', [])
            if not episodes:
                print("线路中没有剧集")
                return
            if episode_index < 0 or episode_index >= len(episodes):
                print(f"集数 {episode_index + 1} 超出范围 (1-{len(episodes)})")
                return
            episode = episodes[episode_index]
            episode_url = episode.get('link')
            if not episode_url:
                print("剧集链接为空")
                return
            print(f"开始获取视频:{episode_url}")
            video_url = self.crawler.find_video_stream(episode_url, site_id)
            if video_url:
                success = self.main_window.video_player.play_video(video_url)
                if success:
                    print("视频开始播放")
                else:
                    print("视频播放失败")
            else:
                print("获取视频链接失败")
        except Exception as e:
            print(f"错误: {e}")