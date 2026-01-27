from PySide6.QtCore import Qt, QPoint, QTimer
from PySide6.QtWidgets import QMainWindow, QWidget
from ui.main_ui import Ui_MainWindow
from src.main_page import MainPageManager
from src.cache_manager import CacheManager


class MainWindow(QMainWindow, Ui_MainWindow):
    def __init__(self):
        super().__init__()
        self.setupUi(self)
        self.page_history = []
        self.loaded_pages = {}
        self.cache_manager = CacheManager(self)
        self.MainPageManager = MainPageManager(self)
        self.setup_connections()
        self.setup_titlebar()
        QTimer.singleShot(0, self.initialize_all_pages)

    def initialize_all_pages(self):
        """初始化所有页面"""
        # 搜索页面
        from ui.search_ui import Ui_SearchPage
        from src.search_page import SearchPageManager
        search_widget = QWidget()
        search_ui = Ui_SearchPage()
        search_ui.setupUi(search_widget)
        search_ui.back_Button.clicked.connect(self.on_search_back)
        self.SearchPageManager = SearchPageManager(search_widget, search_ui, self)
        self.loaded_pages["search"] = search_widget
        self.main_stackedWidget.addWidget(search_widget)
        # 设置页面
        from ui.settings_ui import Ui_SettingsPage
        from src.settings_page import SettingsPageManager
        settings_widget = QWidget()
        settings_ui = Ui_SettingsPage()
        settings_ui.setupUi(settings_widget)
        settings_ui.back_Button_2.clicked.connect(self.go_back_history)
        settings_widget.ui = settings_ui
        self.SettingsPageManager = SettingsPageManager(settings_widget)
        self.loaded_pages["settings"] = settings_widget
        self.main_stackedWidget.addWidget(settings_widget)
        # 详情页面
        from ui.detail_ui import Ui_DetailPage
        from src.detail_page import DetailManager
        detail_widget = QWidget()
        detail_ui = Ui_DetailPage()
        detail_ui.setupUi(detail_widget)
        detail_ui.pushButton_19.clicked.connect(self.go_back_history)
        self.DetailPageManager = DetailManager(detail_ui)
        self.DetailPageManager.request_show_episode.connect(self.show_episode_page)
        self.DetailPageManager.tag_clicked.connect(self.on_tag_clicked)
        self.loaded_pages["detail"] = detail_widget
        self.main_stackedWidget.addWidget(detail_widget)
        # 播放器页面
        from ui.player_ui import Ui_choicePage
        from src.player.player_page import ChoiceEpisodeManager
        from src.player.player_ui_manager import setup_video_player
        player_widget = QWidget()
        player_ui = Ui_choicePage()
        player_ui.setupUi(player_widget)
        player_widget.ui = player_ui
        self.loaded_pages["player"] = player_widget
        self.main_stackedWidget.addWidget(player_widget)
        self.video_player = setup_video_player(player_widget)
        self.PlayerManager = ChoiceEpisodeManager(self)
        self.video_player.back_requested.connect(self.go_back_history)
        # 下载页面
        from ui.download_ui import Ui_DownloaderPage
        from src.download.download_page import DownloadPageManager
        download_widget = QWidget()
        download_ui = Ui_DownloaderPage()
        download_ui.setupUi(download_widget)
        self.DownloadPageManager = DownloadPageManager(download_ui)
        download_widget.ui = download_ui
        self.loaded_pages["download"] = download_widget
        self.showmain_stackedWidget.addWidget(download_widget)

    def setup_titlebar(self):
        """设置标题栏功能"""
        self.is_maximized = False
        self.drag_position = QPoint()
        self.pushButton.clicked.connect(self.minimize_window)
        self.pushButton_8.clicked.connect(self.toggle_maximize_window)
        self.pushButton_2.clicked.connect(self.close_window)
        self.titlebar_frame.mouseDoubleClickEvent = self.titlebar_double_click
        self.titlebar_frame.mousePressEvent = self.titlebar_mouse_press
        self.titlebar_frame.mouseMoveEvent = self.titlebar_mouse_move
        self.titlebar_frame.mouseReleaseEvent = self.titlebar_mouse_release

    def minimize_window(self):
        """最小化窗口"""
        self.showMinimized()

    def toggle_maximize_window(self):
        """切换最大化/还原窗口"""
        if self.is_maximized:
            self.showNormal()
            self.is_maximized = False
        else:
            self.showMaximized()
            self.is_maximized = True

    def close_window(self):
        """关闭窗口"""
        self.close()

    def titlebar_double_click(self, event):
        """标题栏双击事件 - 切换最大化/最小化"""
        if event.button() == Qt.MouseButton.LeftButton:
            self.toggle_maximize_window()

    def titlebar_mouse_press(self, event):
        """标题栏鼠标按下事件 - 开始拖动"""
        if event.button() == Qt.MouseButton.LeftButton:
            self.drag_position = event.globalPos() - self.frameGeometry().topLeft()
            event.accept()

    def titlebar_mouse_move(self, event):
        """标题栏鼠标移动事件 - 拖动窗口"""
        if event.buttons() == Qt.MouseButton.LeftButton and self.drag_position:
            if self.is_maximized:
                self.showNormal()
                self.is_maximized = False
                cursor_pos = event.globalPos()
                self.screen().geometry().center()
                self.move(cursor_pos - QPoint(self.width() // 2, 50))
                self.drag_position = QPoint(self.width() // 2, 50)
            else:
                self.move(event.globalPos() - self.drag_position)
            event.accept()

    def titlebar_mouse_release(self, event):
        """标题栏鼠标释放事件 - 停止拖动"""
        if event.button() == Qt.MouseButton.LeftButton:
            self.drag_position = None
            event.accept()

    def setup_connections(self):
        """设置连接槽"""
        self.search_Button.clicked.connect(self.show_search_page)
        self.settings_Button.clicked.connect(self.show_settings_page)
        self.pushButton_9.clicked.connect(self.switch_to_download_page)

    def switch_to_main_page(self):
        """切换到主页面"""
        self.showmain_stackedWidget.setCurrentIndex(0)

    def go_back_history(self):
        """返回上一个页面"""
        if not self.page_history:
            self.main_stackedWidget.setCurrentIndex(0)
            return
        current_widget = self.main_stackedWidget.currentWidget()
        if current_widget == self.loaded_pages["detail"]:
            self.DetailPageManager.clear_detail_page()
        previous_index = self.page_history.pop()
        self.main_stackedWidget.setCurrentIndex(previous_index)

    def on_search_back(self):
        """搜索页面返回"""
        self.SearchPageManager.clear_search_results()
        self.main_stackedWidget.setCurrentIndex(0)
        self.page_history.clear()

    def show_settings_page(self):
        """跳转设置页面"""
        self.main_stackedWidget.setCurrentWidget(self.loaded_pages["settings"])

    def show_episode_page(self, collection_data):
        """显示选集遮罩层"""
        from ui.episode_ui import Ui_EpisodePage
        from src.episode_page import EpisodeManager
        self.episode_overlay = QWidget(self)
        self.episode_overlay.setGeometry(0, 0, self.width(), self.height())
        self.episode_overlay.setObjectName("episode_overlay")
        self.episode_overlay.setStyleSheet("#episode_overlay{background-color: rgba(0, 0, 0, 50)}")
        self.episode_overlay.mousePressEvent = self.close_episode_overlay
        self.episode_container = QWidget(self.episode_overlay)
        self.episode_container.setObjectName("episode_container")
        self.episode_container.setStyleSheet("#episode_container{background-color: white}")
        default_height = 180
        self.episode_container.setFixedSize(600, default_height)
        self.episode_container.move((self.width() - 600) // 2, (self.height() - default_height) // 2)
        episode_ui = Ui_EpisodePage()
        episode_ui.setupUi(self.episode_container)
        episode_ui.pushButton_14.clicked.connect(lambda: self.close_episode_overlay())
        self.episode_manager = EpisodeManager(episode_ui, self)
        self.episode_manager.episodes_layout_updated.connect(self.update_episode_container_position)
        self.episode_manager.episode_page_data(collection_data)
        self.episode_overlay.show()

    def update_episode_container_position(self):
        """更新选集容器位置"""
        total_rows = self.episode_manager.get_total_rows()
        button_height = 40
        row_spacing = 5
        title_height = 60
        top_margin = 40
        bottom_margin = 40
        container_width = 600
        container_height = title_height + top_margin + bottom_margin
        if total_rows > 0:
            container_height += total_rows * button_height + (total_rows - 1) * row_spacing
        self.episode_container.setFixedSize(container_width, container_height)
        self.episode_container.move((self.width() - container_width) // 2, (self.height() - container_height) // 2)

    def close_episode_overlay(self, event=None):
        """关闭选集遮罩层"""
        if event is None or not self.episode_container.geometry().contains(event.pos()):
            if hasattr(self, 'episode_overlay'):
                self.episode_overlay.deleteLater()
                attrs = ['episode_overlay', 'episode_container', 'episode_manager']
                for attr in attrs:
                    if hasattr(self, attr):
                        delattr(self, attr)

    def show_search_page(self):
        """跳转搜索页面"""
        self.main_stackedWidget.setCurrentWidget(self.loaded_pages["search"])
        current_type = self.MainPageManager.current_subject_type
        self.SearchPageManager.update_combo_box_by_type(current_type)

    def switch_to_download_page(self):
        """切换到下载页面"""
        download_widget = self.loaded_pages["download"]
        self.showmain_stackedWidget.setCurrentWidget(download_widget)
        self.DownloadPageManager.load_recent_files()

    def on_tag_clicked(self, tag_name):
        """tag搜索"""
        self.main_stackedWidget.setCurrentWidget(self.loaded_pages["search"])
        self.SearchPageManager.clear_search_results()
        self.SearchPageManager.search_by_tag(tag_name)
        self.SearchPageManager.ui.search_lineEdit.setText(f"tag:{tag_name}")

    def show_detail_page(self, collection_data):
        """跳转详情页面"""
        current_index = self.main_stackedWidget.currentIndex()
        self.page_history.append(current_index)
        self.main_stackedWidget.setCurrentWidget(self.loaded_pages["detail"])
        self.DetailPageManager.detail_page_data(collection_data)