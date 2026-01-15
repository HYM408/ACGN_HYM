from PySide6.QtGui import QIcon, QFont, QCursor
from PySide6.QtCore import Qt, QSize, QTimer, QObject, Signal
from PySide6.QtWidgets import QLabel, QFrame, QVBoxLayout, QHBoxLayout, QSpacerItem, QSizePolicy, QPushButton
from src.cache_manager import CacheManager
from src.thread_manager import thread_manager
from src.menu_utils import show_status_selector
from src.image_utils import load_image_with_cache
from src.sqlite import get_by_subject_type_and_type


class MainPageManager(QObject):
    refresh_requested = Signal()

    def __init__(self, main_window):
        super().__init__()
        self.main_window = main_window
        self.current_subject_type = None
        self.current_status_type = None
        self.current_page = 1
        self.items_per_page = 12
        self.all_collections = []
        self.filtered_collections = []
        # 字体预热
        QTimer.singleShot(50, self._warmup_music_symbols)
        # 缓存管理器
        self._cache_manager = CacheManager(self.main_window)
        # 状态映射
        self.status_reverse_map = {"取消追番": 0, "想看": 1, "看过": 2, "在看": 3, "搁置": 4, "抛弃": 5}
        # 刷新信号
        thread_manager.refresh_main_page.connect(self._refresh_in_main_thread)
        # 链接
        self.setup_connections()

    def _refresh_in_main_thread(self):
        """在主线程中刷新页面"""
        self.load_collections(self.current_subject_type, self.current_status_type, force_refresh=True)

    def _warmup_music_symbols(self):
        """字体预热"""
        label = QLabel()
        label.hide()
        label.setText("♭")
        label.grab()
        label.deleteLater()

    def setup_connections(self):
        """设置状态按钮的连接"""
        # 刷新
        self.main_window.refresh_Button.clicked.connect(lambda: self.load_collections(self.current_subject_type, self.current_status_type, force_refresh=True))
        # 状态
        self.main_window.pushButton_3.clicked.connect(lambda: self.load_collections(self.current_subject_type, 1))
        self.main_window.pushButton_4.clicked.connect(lambda: self.load_collections(self.current_subject_type, 3))
        self.main_window.pushButton_5.clicked.connect(lambda: self.load_collections(self.current_subject_type, 2))
        self.main_window.pushButton_6.clicked.connect(lambda: self.load_collections(self.current_subject_type, 4))
        self.main_window.pushButton_7.clicked.connect(lambda: self.load_collections(self.current_subject_type, 5))
        # 类别
        self.main_window.animation_Button.clicked.connect(lambda: self.load_collections_and_switch(2, self.current_status_type))
        self.main_window.novel_Button.clicked.connect(lambda: self.load_collections_and_switch(7, self.current_status_type))
        self.main_window.game_Button.clicked.connect(lambda: self.load_collections_and_switch(4, self.current_status_type))
        self.main_window.comic_Button.clicked.connect(lambda: self.load_collections_and_switch(8, self.current_status_type))
        # 下载
        self.main_window.pushButton_9.clicked.connect(self.show_download_page)
        # 分页
        self.main_window.previous_Button.clicked.connect(self.previous_page)
        self.main_window.next_Button.clicked.connect(self.next_page)
        # 页面标题更新
        self.main_window.animation_Button.toggled.connect(lambda checked: self.update_title("动画"))
        self.main_window.novel_Button.toggled.connect(lambda checked: self.update_title("小说"))
        self.main_window.game_Button.toggled.connect(lambda checked: self.update_title("游戏"))
        self.main_window.comic_Button.toggled.connect(lambda checked: self.update_title("漫画"))
        # 搜索
        self.main_window.searchlist_lineEdit.textChanged.connect(self.on_search_text_changed)
        # 状态名称映射
        self.status_names_map = {2: {1: "想看", 2: "看过", 3: "在看"},4: {1: "想玩", 2: "玩过", 3: "在玩"},7: {1: "想读", 2: "读过", 3: "在读"},8: {1: "想读", 2: "读过", 3: "在读"}}
        # 默认动画在看
        self.main_window.animation_Button.setChecked(True)
        self.main_window.pushButton_4.setChecked(True)
        self.load_collections(subject_type=2, status_type=3)

    def on_search_text_changed(self, text):
        """搜索文本变化处理"""
        self.current_page = 1
        self.filter_collections(text)
        self.display_current_page()

    def filter_collections(self, search_text):
        """根据搜索文本过滤收藏"""
        if not search_text.strip():
            self.filtered_collections = self.all_collections.copy()
            return
        search_text_lower = search_text.lower()
        self.filtered_collections = [collection for collection in self.all_collections if search_text_lower in (collection.get('subject_name_cn', '').lower() or '') or search_text_lower in (collection.get('subject_name', '').lower() or '')]

    def update_status_buttons_text(self, subject_type):
        """根据subject_type更新状态按钮的文本"""
        if status_names := self.status_names_map.get(subject_type):
            self.main_window.pushButton_3.setText(status_names[1])
            self.main_window.pushButton_4.setText(status_names[3])
            self.main_window.pushButton_5.setText(status_names[2])

    def load_collections_and_switch(self, subject_type, status_type):
        """回到主页面并加载收藏"""
        self.main_window.switch_to_main_page()
        self.load_collections(subject_type, status_type)

    def load_collections(self, subject_type=None, status_type=None, force_refresh=False):
        """加载或刷新对应类别和状态的数据"""
        needs_update = False
        if subject_type != self.current_subject_type:
            self.update_status_buttons_text(subject_type)
            self.current_subject_type = subject_type
            needs_update = True
        if status_type != self.current_status_type:
            self.current_status_type = status_type
            needs_update = True
        if needs_update or force_refresh or not self.all_collections:
            self._cache_manager.clear_pending_downloads()
            self.all_collections = get_by_subject_type_and_type(subject_type, status_type)
            self.filtered_collections = self.all_collections.copy()
            self.current_page = 1
            self.main_window.searchlist_lineEdit.clear()
        if not self.all_collections:
            self.clear_display_area()
        else:
            self.display_current_page()
        self.update_page_info()

    def update_title(self, title):
        """更新页面标题"""
        self.main_window.project_Button.setText(title)

    def display_current_page(self):
        """显示当前页数据"""
        self.clear_display_area()
        self.setup_fixed_layout_constraints()
        self._layout_constraints_set = True
        display_collections = self.filtered_collections
        start_idx = (self.current_page - 1) * self.items_per_page
        end_idx = min(start_idx + self.items_per_page, len(display_collections))
        current_page_data = display_collections[start_idx:end_idx]
        row, col = 0, 0
        for collection in current_page_data:
            card = self.create_collection_card(collection)
            self.main_window.gridLayout_2.addWidget(card, row, col, Qt.AlignCenter)
            col += 1
            if col >= 3:
                col = 0
                row += 1
        total_cards = len(current_page_data)
        rows_needed = (total_cards + 2) // 3
        for i in range(rows_needed * 3 - total_cards):
            placeholder = QFrame()
            placeholder.setFixedSize(QSize(420, 170))
            placeholder.setStyleSheet("background: transparent;")
            self.main_window.gridLayout_2.addWidget(placeholder, row, col)
            col += 1
            if col >= 3:
                col = 0
                row += 1
        self.update_page_info()

    def show_download_page(self):
        """显示下载页面"""
        self.main_window.showmain_stackedWidget.setCurrentWidget(self.main_window.loaded_pages["download"])

    def create_collection_card(self, collection):
        """创建收藏卡片"""
        # 主框架
        card = QFrame()
        card.setObjectName(f"animationdata_frame_{id(card)}")
        card.setFixedSize(QSize(420, 170))
        card.setStyleSheet("""
            QFrame{
                background-color: rgb(242, 236, 244);
                border-radius: 15px;
            }
        """)
        card.setCursor(QCursor(Qt.PointingHandCursor))
        # 水平布局
        horizontal_layout = QHBoxLayout(card)
        horizontal_layout.setSpacing(0)
        horizontal_layout.setContentsMargins(0, 0, 0, 0)
        # 封面
        cover_label = QLabel(card)
        cover_label.setFixedSize(QSize(125, 170))
        cover_label.setStyleSheet("""
            background-color: rgb(242, 236, 244);
            border-top-left-radius: 15px;
            border-bottom-left-radius: 15px;
        """)
        cover_label.setAlignment(Qt.AlignCenter)
        # 图片
        image_url = collection.get('subject_images_common')
        if image_url:
            load_image_with_cache(self._cache_manager, image_url, cover_label, 40, False)
        else:
            cover_label.setText("暂无封面")
        horizontal_layout.addWidget(cover_label)
        # 垂直布局
        info_layout = QVBoxLayout()
        info_layout.setSpacing(8)
        info_layout.setContentsMargins(5, 5, -1, -1)
        # 标题
        title_label = QLabel(card)
        title_label.setMinimumSize(QSize(0, 30))
        title_label.setMaximumSize(QSize(16777215, 30))
        font = QFont()
        font.setFamilies(["微软雅黑"])
        font.setPointSize(13)
        title_label.setFont(font)
        # 设置标题
        title = collection.get('subject_name_cn') or collection.get('subject_name')
        title_label.setText(title)
        info_layout.addWidget(title_label)
        # 进度
        progress_label = QLabel(card)
        font = QFont()
        font.setFamilies(["微软雅黑"])
        font.setPointSize(10)
        progress_label.setFont(font)
        # 进度
        subject_eps = collection.get('subject_eps', 0)
        progress_text = f"全{subject_eps if subject_eps > 0 else '--'}话"
        progress_label.setText(progress_text)
        info_layout.addWidget(progress_label)
        # 弹簧
        spacer = QSpacerItem(20, 13, QSizePolicy.Minimum, QSizePolicy.Expanding)
        info_layout.addItem(spacer)
        # 按钮水平布局
        button_layout = QHBoxLayout()
        button_layout.setSpacing(0)
        button_layout.setContentsMargins(-1, 0, 15, 15)
        # 左侧弹簧
        left_spacer = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)
        button_layout.addItem(left_spacer)
        # 状态按钮
        more_button = QPushButton(card)
        more_button.setFixedSize(QSize(40, 40))
        more_button.setStyleSheet("""
            QPushButton {
                background-color: rgb(242, 236, 244);
                border: none;
                border-radius: 20px;
            }
            QPushButton:hover {
                background-color: rgb(216, 207, 232);
                border: none;
                border-radius: 20px;
            }
        """)
        more_button.setIcon(QIcon("icons/more.png"))
        button_layout.addWidget(more_button)
        # 选集
        episode_button = QPushButton(card)
        episode_button.setFixedSize(QSize(60, 40))
        font = QFont()
        font.setFamilies(["微软雅黑"])
        font.setPointSize(10)
        episode_button.setFont(font)
        episode_button.setStyleSheet("""
            QPushButton {
                background-color: rgb(242, 236, 244);
                border: none;
                border-radius: 20px;
            }
            QPushButton:hover {
                background-color: rgb(216, 207, 232);
                border: none;
                border-radius: 20px;
            }
        """)
        episode_button.setText("选集")
        button_layout.addWidget(episode_button, 0, Qt.AlignRight)
        info_layout.addLayout(button_layout)
        horizontal_layout.addLayout(info_layout)
        card.cover_label = cover_label
        card.collection_data = collection
        # 连接选集页
        episode_button.clicked.connect(lambda: self.show_episode_page(card.collection_data))
        # 连接详情页
        def on_card_clicked(event):
            if event.button() == Qt.LeftButton:
                self.show_detail_page(card.collection_data)
        card.mousePressEvent = on_card_clicked
        cover_label.setCursor(Qt.PointingHandCursor)
        cover_label.mousePressEvent = on_card_clicked
        more_button.clicked.connect(lambda: self.on_more_button_clicked(more_button, collection))
        return card

    def on_more_button_clicked(self, button, collection):
        selector = show_status_selector(parent_button=button,on_status_selected=lambda status_text: self.on_status_selected(status_text, collection))
        button.status_selector = selector

    def setup_fixed_layout_constraints(self):
        """布局约束"""
        layout = self.main_window.gridLayout_2
        for col in range(3):
            layout.setColumnMinimumWidth(col, 420)
            layout.setColumnStretch(col, 0)
        for row in range(4):
            layout.setRowMinimumHeight(row, 170)
            layout.setRowStretch(row, 0)
        layout.setVerticalSpacing(20)
        layout.setHorizontalSpacing(40)
        layout.setAlignment(Qt.AlignTop | Qt.AlignHCenter)

    def show_episode_page(self, collection_data):
        """显示选集页面"""
        current_index = self.main_window.main_stackedWidget.currentIndex()
        self.main_window.page_history.append(current_index)
        self.main_window.show_episode_page(collection_data)

    def show_detail_page(self, collection_data):
        """显示详情页面"""
        current_index = self.main_window.main_stackedWidget.currentIndex()
        self.main_window.page_history.append(current_index)
        self.main_window.show_detail_page(collection_data)

    def clear_display_area(self):
        """清空显示区域"""
        while self.main_window.gridLayout_2.count():
            item = self.main_window.gridLayout_2.takeAt(0)
            if item and item.widget():
                item.widget().deleteLater()

    def update_page_info(self):
        """更新分页信息"""
        total_items = len(self.filtered_collections)
        total_pages = (total_items + self.items_per_page - 1) // self.items_per_page or 1
        self.main_window.pages_Button.setText(f"{self.current_page}/{total_pages}")
        self.main_window.previous_Button.setEnabled(self.current_page > 1)
        self.main_window.next_Button.setEnabled(self.current_page < total_pages)

    def previous_page(self):
        """上一页"""
        if self.current_page > 1:
            self._cache_manager.clear_pending_downloads()
            self.current_page -= 1
            self.display_current_page()

    def next_page(self):
        """下一页"""
        total_items = len(self.filtered_collections)
        total_pages = (total_items + self.items_per_page - 1) // self.items_per_page
        if self.current_page < total_pages:
            self._cache_manager.clear_pending_downloads()
            self.current_page += 1
            self.display_current_page()

    def on_status_selected(self, status_text, collection_data):
        """选择状态回调"""
        new_type = self.status_reverse_map.get(status_text)
        if new_type is None:
            return
        old_type = collection_data.get('type')
        thread_manager.update_status(collection_data.get('subject_id'),old_type,new_type,self.on_status_update_complete)

    def on_status_update_complete(self, success, error_msg):
        """状态更新完成回调"""
        if not success:
            print(f"状态更新失败: {error_msg}")