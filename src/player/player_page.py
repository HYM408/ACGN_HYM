import httpx
import threading
from PySide6.QtCore import Qt, QSize
from PySide6.QtGui import QFont, QIcon, QTransform
from PySide6.QtWidgets import QPushButton, QVBoxLayout, QFrame, QHBoxLayout, QSpacerItem, QSizePolicy, QTabWidget, QScrollArea, QLabel, QDialog
from src.sqlite import get_by_subject_id
from src.thread_manager import thread_manager
from src.player.css import VideoCrawler, BTCrawler


class ChoiceEpisodeManager:
    """线路选择"""
    def __init__(self, main_window):
        self.main_window = main_window
        self.site_widgets = {}
        self.current_episode = None
        self.crawler = VideoCrawler()
        self.bt_crawler = BTCrawler()
        self.detail_tab_widget = None
        thread_manager.site_search_completed.connect(self._update_site_widget)

    def show_player_page(self, episode_data):
        """显示播放器页面"""
        # subject_id
        subject_id = episode_data.get('subject_id')
        if 'episode' in episode_data:
            subject_id = subject_id or episode_data['episode'].get('subject_id')
        # sort
        sort = episode_data.get('sort')
        if 'episode' in episode_data:
            sort = sort or episode_data['episode'].get('sort')
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
                if widget := layout.takeAt(0).widget():
                    widget.deleteLater()
        else:
            container.setLayout(QVBoxLayout())
        container.layout().setContentsMargins(10, 10, 10, 10)
        self._init_detail_tab()
        # 创建所有站点卡片
        all_site_ids = sorted(self.crawler.site_configs.keys()) + sorted(self.bt_crawler.bt_configs.keys())
        self.site_widgets = {}
        self.site_detail_tabs = {}
        for site_id in all_site_ids:
            widget = self._create_site_card(site_id, 'loading', None)
            self.site_widgets[site_id] = widget
            container.layout().addWidget(widget)
            self._create_site_detail_tab(site_id)
        container.layout().addItem(QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding))
        # 开始搜索
        thread_manager.search_sites(sorted(self.crawler.site_configs.keys()), keyword, self.crawler)
        thread_manager.search_bt_sites(sorted(self.bt_crawler.bt_configs.keys()), keyword)

    def _init_detail_tab(self):
        """初始化详细标签页"""
        player_widget = self.main_window.loaded_pages["player"]
        detail_tab = player_widget.ui.tabWidget.widget(1)
        if layout := detail_tab.layout():
            while layout.count():
                if widget := layout.takeAt(0).widget():
                    widget.deleteLater()
        else:
            detail_tab.setLayout(QVBoxLayout())
        self.detail_tab_widget = QTabWidget()
        self.detail_tab_widget.setTabPosition(QTabWidget.TabPosition.West)
        self.detail_tab_widget.setIconSize(QSize(30, 30))
        self.detail_tab_widget.setStyleSheet("QTabBar::tab {width: 50px; height: 50px}")
        detail_tab.layout().addWidget(self.detail_tab_widget)

    def _create_site_detail_tab(self, site_id):
        """详细标签页"""
        if not self.detail_tab_widget:
            return
        # 滚动区域
        scroll_area = QScrollArea()
        scroll_area.setWidgetResizable(True)
        scroll_area.setHorizontalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAsNeeded)
        scroll_area.setVerticalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAlwaysOn)
        # 滚动条样式
        scroll_area.setStyleSheet("QScrollBar:vertical {border: none; background: #f0f0f0; width: 11px}"
                                  "QScrollBar::handle:vertical {background: #c0c0c0; border-radius: 5px; min-height: 20px}"
                                  "QScrollBar::handle:vertical:hover {background: #a0a0a0}"
                                  "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {border: none; background: none; height: 0px}")
        # 内容框架
        content_frame = QFrame()
        content_layout = QVBoxLayout(content_frame)
        content_layout.setContentsMargins(5, 5, 5, 5)
        content_layout.setSpacing(3)
        content_layout.setAlignment(Qt.AlignmentFlag.AlignTop)
        loading_btn = QPushButton("搜索中...")
        loading_btn.setFlat(True)
        loading_btn.setStyleSheet("color: #666; padding: 5px; text-align: left; border: none")
        content_layout.addWidget(loading_btn)
        content_layout.addItem(QSpacerItem(20, 20, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding))
        scroll_area.setWidget(content_frame)
        tab_index = self.detail_tab_widget.addTab(scroll_area, "")
        site_config = self.crawler.site_configs.get(site_id) or self.bt_crawler.bt_configs.get(site_id)
        if icon_url := site_config.get('icon'):
            self.main_window.cache_manager.get_image_async(icon_url, lambda pixmap, idx=tab_index: self._set_tab_icon(idx, pixmap))
        else:
            self.detail_tab_widget.setTabText(tab_index, site_id[0].upper())
        self.detail_tab_widget.setTabToolTip(tab_index, site_id)
        self.site_detail_tabs[site_id] = {'content_frame': content_frame, 'content_layout': content_layout}

    def _set_tab_icon(self, tab_index, pixmap):
        """设置标签图标"""
        if not pixmap.isNull():
            pixmap = pixmap.transformed(QTransform().rotate(90))
            icon = QIcon(pixmap)
            self.detail_tab_widget.setTabIcon(tab_index, icon)

    def _create_site_card(self, site_id, status, result):
        """创建站点卡片"""
        card = QFrame()
        layout = QVBoxLayout(card)
        layout.setContentsMargins(15, 15, 15, 15)
        layout.setSpacing(10)
        top_layout = QHBoxLayout()
        top_layout.setSpacing(8)
        # 图标
        icon_btn = QPushButton()
        icon_btn.setFixedSize(20, 20)
        icon_btn.setFlat(True)
        site_config = self.crawler.site_configs.get(site_id) or self.bt_crawler.bt_configs.get(site_id)
        if icon_url := site_config.get('icon'):
            self.main_window.cache_manager.get_image_async(icon_url, lambda pixmap: self._set_button_icon(icon_btn, pixmap))
        top_layout.addWidget(icon_btn)
        # 站点名称
        site_btn = QPushButton(site_id)
        site_btn.setFont(QFont("", 14, QFont.Weight.Bold))
        site_btn.setStyleSheet("color: #333; text-align: left; border: none")
        site_btn.setFlat(True)
        top_layout.addWidget(site_btn)
        # 状态/标题信息
        if status == 'loading':
            status_text = "搜索中..."
        elif status == 'success':
            if site_id in self.bt_crawler.bt_configs:
                status_text = f"{len(result)}个结果" if result else "无结果"
            else:
                status_text = result.get('title', '') if result else ""
        else:
            status_text = "✗"
        status_btn = QPushButton(status_text)
        status_btn.setFlat(True)
        status_btn.setStyleSheet("color: #666; font-size: 13px; text-align: left; border: none")
        top_layout.addWidget(status_btn, 1)
        layout.addLayout(top_layout)
        # 线路
        if site_id not in self.bt_crawler.bt_configs and status == 'success' and result is not None and result.get('routes'):
            routes_layout = QHBoxLayout()
            routes_layout.setAlignment(Qt.AlignmentFlag.AlignLeft)
            routes_layout.setSpacing(5)
            for route in result['routes']:
                button = QPushButton(route['route'])
                button.setMinimumHeight(35)
                button.setStyleSheet("QPushButton {padding: 5px 10px;font-size: 13px;border: 1px solid #ddd;border-radius: 5px;background-color: white}"
                                     "QPushButton:hover {background-color: #f5f5f5;border-color: #aaa}")
                button.clicked.connect(lambda checked, s=site_id, r=route: self._on_route_selected(s, r))
                routes_layout.addWidget(button)
            layout.addLayout(routes_layout)
        return card

    @staticmethod
    def _set_button_icon(button, pixmap):
        """设置图标"""
        if not pixmap.isNull():
            button.setIcon(QIcon(pixmap))

    def _update_site_widget(self, site_data):
        """更新站点组件"""
        site_id = site_data['site_id']
        status = site_data['status']
        results = site_data['result']
        if site_id in self.site_widgets:
            if site_id in self.bt_crawler.bt_configs:
                card_result = results
            else:
                card_result = results[0] if results else None
            new_widget = self._create_site_card(site_id, status, card_result)
            if layout := self.site_widgets[site_id].parent().layout():
                index = layout.indexOf(self.site_widgets[site_id])
                if index >= 0:
                    self.site_widgets[site_id].deleteLater()
                    layout.insertWidget(index, new_widget)
                    self.site_widgets[site_id] = new_widget
        if site_id in self.site_detail_tabs:
            tab_info = self.site_detail_tabs[site_id]
            content_layout = tab_info['content_layout']
            while content_layout.count():
                if widget := content_layout.takeAt(0).widget():
                    widget.deleteLater()
            if status == 'success' and results:
                if site_id in self.bt_crawler.bt_configs:
                    for result in results:
                        bt_widget = self._create_bt_result_component(result)
                        content_layout.addWidget(bt_widget)
                else:
                    for result in results:
                        routes = result.get('routes', [])
                        title = result.get('title', '')
                        for route in routes:
                            route_widget = self._create_route_component(site_id, route, title)
                            content_layout.addWidget(route_widget)
            else:
                error_btn = QPushButton("搜索失败或无结果")
                error_btn.setFlat(True)
                error_btn.setStyleSheet("color: #ff4444; padding: 10px; border: none")
                content_layout.addWidget(error_btn)
            content_layout.addItem(QSpacerItem(20, 20, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding))

    def _create_route_component(self, site_id, route, title):
        """创建路线组件"""
        route_frame = QFrame()

        def on_route_clicked(_):
            self._on_route_selected(site_id, route)
        route_frame.mousePressEvent = on_route_clicked
        route_layout = QVBoxLayout(route_frame)
        route_layout.setContentsMargins(10, 10, 10, 10)
        route_layout.setSpacing(2)
        route_frame.setStyleSheet("QFrame {border: 1px solid #e0e0e0; border-radius: 4px; margin: 2px; background-color: white}"
                                  "QFrame:hover {border: 1px solid #4da6ff; background-color: #e8f4ff}")
        route_frame.setMaximumWidth(310)
        if title:
            title_label = QLabel(title)
            title_label.setFont(QFont("微软雅黑", 13, QFont.Weight.Bold))
            title_label.setStyleSheet("color: black; background-color: transparent; border: none; text-align: left")
            route_layout.addWidget(title_label)
        route_name = route.get('route')
        if route_name:
            route_label = QLabel(route_name)
            route_label.setFont(QFont("微软雅黑", 10, QFont.Weight.Bold))
            route_label.setStyleSheet("color: #333; background-color: transparent; border: none; text-align: left")
            route_layout.addWidget(route_label)
        return route_frame

    def _create_bt_result_component(self, result):
        """创建BT结果组件"""
        bt_frame = QFrame()
        def on_bt_clicked(_):
            self._on_bt_result_clicked(result)
        bt_frame.mousePressEvent = on_bt_clicked
        bt_layout = QVBoxLayout(bt_frame)
        bt_layout.setContentsMargins(10, 10, 10, 10)
        bt_layout.setSpacing(5)
        bt_frame.setStyleSheet("QFrame {border: 1px solid #e0e0e0; border-radius: 4px; margin: 2px; background-color: white}"
                               "QFrame:hover {border: 1px solid #4caf50; background-color: #e8f5e9}")
        bt_frame.setMaximumWidth(310)
        # 文件名
        name = result.get('name')
        if name:
            name_label = QLabel(name)
            name_label.setFont(QFont("微软雅黑", 13, QFont.Weight.Bold))
            name_label.setStyleSheet("color: black; border: none; background: transparent")
            name_label.setWordWrap(True)
            name_label.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Preferred)
            bt_layout.addWidget(name_label)
        # 文件大小
        size = result.get('size')
        if size:
            size_label = QLabel(f"大小: {size}")
            size_label.setFont(QFont("微软雅黑", 10, QFont.Weight.Bold))
            size_label.setStyleSheet("color: #666; border: none; background: transparent")
            bt_layout.addWidget(size_label)
        return bt_frame

    def _on_route_selected(self, site_id, route):
        """处理线路选择"""
        try:
            episode_index = int(self.current_episode['sort']) - 1
            episodes = route.get('episodes', [])
            if not episodes:
                print("线路中没有剧集")
                return
            if episode_index >= len(episodes):
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

    def _on_bt_result_clicked(self, result):
        """BT结果点击"""
        dialog = QDialog(self.main_window)
        dialog.setWindowTitle(result.get('name'))
        dialog.setFixedSize(600, 150)
        layout = QVBoxLayout(dialog)
        # BT
        layout.addWidget(QLabel(f"BT: {result.get('magnet_link')}"))
        # keepshare
        play_label = QLabel(f"<a href='{result.get('play_link')}'>keepshare: {result.get('play_link')}</a>")
        play_label.setTextFormat(Qt.TextFormat.RichText)
        play_label.setTextInteractionFlags(Qt.TextInteractionFlag.TextBrowserInteraction)
        play_label.setOpenExternalLinks(True)
        layout.addWidget(play_label)
        # 重定向
        redirect_label = QLabel("重定向中...")
        layout.addWidget(redirect_label)
        redirect_url = [None]
        def detect_redirect():
            """重定向检测"""
            redirect_label.setText("重定向中...")
            try:
                response = httpx.get(result.get('play_link'), follow_redirects=True, timeout=15)
                final_url = str(response.url)
                if '?act=play' in final_url:
                    final_url = final_url.split('?act=play')[0]
                redirect_url[0] = final_url
                redirect_label.setText(f"PikPak链接: {final_url}")
                pikpak_btn.setEnabled(True)
            except Exception as e:
                error_msg = f"重定向失败: {e}"
                redirect_label.setText(error_msg)
                pikpak_btn.setEnabled(False)
        thread = threading.Thread(target=detect_redirect, daemon=True)
        thread.start()
        button_layout = QHBoxLayout()
        # BT按钮
        download_btn = QPushButton("BT下载")
        download_btn.clicked.connect(lambda: print(f"BT链接: {result.get('magnet_link')}"))
        button_layout.addWidget(download_btn)
        # PikPak按钮
        pikpak_btn = QPushButton("保存到PikPak并下载(ip不能在中国大陆)")
        pikpak_btn.setFixedWidth(250)
        pikpak_btn.setEnabled(False)
        def save_to_pikpak():
            """保存到PikPak"""
            pikpak_btn.setText("保存中...")
            pikpak_btn.setEnabled(False)
            thread_manager.save_pikpak_share(redirect_url[0], on_save_completed)
        def on_save_completed(status):
            """保存完成回调"""
            if status == "success":
                pikpak_btn.setText("保存成功")
            else:
                pikpak_btn.setText("保存失败")
                pikpak_btn.setEnabled(True)
        pikpak_btn.clicked.connect(save_to_pikpak)
        button_layout.addWidget(pikpak_btn)
        # 重新重定向按钮
        redetect_btn = QPushButton("重定向")
        redetect_btn.clicked.connect(lambda: threading.Thread(target=detect_redirect, daemon=True).start())
        button_layout.addWidget(redetect_btn)
        layout.addLayout(button_layout)
        dialog.exec()