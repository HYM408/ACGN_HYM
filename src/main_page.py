from PySide6.QtGui import QIcon, QFont, QCursor
from PySide6.QtCore import Qt, QSize, QTimer, QObject, Signal
from PySide6.QtWidgets import QLabel, QFrame, QVBoxLayout, QHBoxLayout, QSpacerItem, QSizePolicy, QPushButton
from src.thread_manager import thread_manager
from src.utils.menu_utils import show_status_selector
from src.utils.image_utils import load_image_with_cache
from src.sqlite import get_by_subject_type_and_type, get_status_counts_by_subject_type


class MainPageManager(QObject):
    refresh_requested = Signal()

    def __init__(self, main_window):
        super().__init__()
        self.main_window = main_window
        self.current_subject_type = 2
        self.current_status_type = 3
        self.current_page = 1
        self.items_per_page = 12
        self.all_collections = []
        self.filtered_collections = []
        # 字体预热
        QTimer.singleShot(0, self._warmup_music_symbols)
        # 状态映射
        self.status_reverse_map = {"取消追番": 0, "想看": 1, "看过": 2, "在看": 3, "搁置": 4, "抛弃": 5}
        self.status_names_map = {2: {1: "想看", 2: "看过", 3: "在看"}, 4: {1: "想玩", 2: "玩过", 3: "在玩"}, 7: {1: "想读", 2: "读过", 3: "在读"}, 8: {1: "想读", 2: "读过", 3: "在读"}}
        # 状态和按钮映射
        self.status_frames = {1: (self.main_window.frame_3, self.main_window.pushButton_3, self.main_window.pushButton_10), 2: (self.main_window.frame_7, self.main_window.pushButton_5, self.main_window.pushButton_12), 3: (self.main_window.frame_5, self.main_window.pushButton_4, self.main_window.pushButton_11), 4: (self.main_window.frame_9, self.main_window.pushButton_6, self.main_window.pushButton_13), 5: (self.main_window.frame_11, self.main_window.pushButton_7, self.main_window.pushButton_14)}
        # 类别按钮映射
        self.category_buttons = {self.main_window.animation_Button: (2, "动画"),self.main_window.novel_Button: (7, "小说"), self.main_window.game_Button: (4, "游戏"), self.main_window.comic_Button: (8, "漫画")}
        # 刷新信号
        thread_manager.refresh_main_page.connect(self._refresh_in_main_thread)
        # 连接
        QTimer.singleShot(0, self.setup_connections)

    def _refresh_in_main_thread(self):
        """在主线程中刷新页面"""
        self.load_collections(self.current_subject_type, self.current_status_type)

    @staticmethod
    def _warmup_music_symbols():
        """字体预热"""
        label = QLabel()
        label.hide()
        label.setText("♭")
        label.grab()
        label.deleteLater()

    def setup_connections(self):
        """设置连接"""
        # 刷新
        self.main_window.refresh_Button.clicked.connect(lambda: self.load_collections(self.current_subject_type, self.current_status_type))
        # 状态
        for status_type, (frame, btn, count_btn) in self.status_frames.items():
            frame.installEventFilter(self)
            frame.setProperty("status_type", status_type)
            frame.enterEvent = lambda event, f=frame, st=status_type: self.on_status_frame_enter(f, st)
            frame.leaveEvent = lambda event, f=frame, st=status_type: self.on_status_frame_leave(f, st)
            btn.setStyleSheet("QPushButton { background-color: transparent; border: none}")
            btn.setAttribute(Qt.WidgetAttribute.WA_TransparentForMouseEvents, True)
            count_btn.setStyleSheet("QPushButton { background-color: transparent; border: none}")
            count_btn.setAttribute(Qt.WidgetAttribute.WA_TransparentForMouseEvents, True)
        # 类别
        for button, (subject_type, title) in self.category_buttons.items():
            button.clicked.connect(lambda _, st=subject_type, t=title: self.switch_category(st, t))
        # 分页
        self.main_window.previous_Button.clicked.connect(self.previous_page)
        self.main_window.next_Button.clicked.connect(self.next_page)
        # 搜索
        self.main_window.searchlist_lineEdit.textChanged.connect(self.on_search_text_changed)
        # 默认选中
        self.main_window.animation_Button.setChecked(True)
        self.load_collections(2, 3)

    def on_status_frame_enter(self, frame, status_type):
        """鼠标进入状态框架事件"""
        if status_type == self.current_status_type:
            frame.setStyleSheet("QFrame{background-color: rgba(103, 79, 165, 30); border-bottom: 3px solid rgb(103, 79, 165)}")
        else:
            frame.setStyleSheet("QFrame{background-color: rgba(103, 79, 165, 30)}")

    def on_status_frame_leave(self, frame, status_type):
        """鼠标离开状态框架事件"""
        if status_type == self.current_status_type:
            frame.setStyleSheet("QFrame{background-color: transparent; border-bottom: 3px solid rgb(103, 79, 165)}")
        else:
            frame.setStyleSheet("QFrame{background-color: transparent}")

    def eventFilter(self, obj, event):
        """事件过滤器，捕获鼠标点击事件"""
        if event.type() == event.Type.MouseButtonPress and event.button() == Qt.MouseButton.LeftButton:
            for status_type, (frame, _, _) in self.status_frames.items():
                if obj == frame:
                    self.load_collections(self.current_subject_type, status_type)
                    return True
        return super().eventFilter(obj, event)

    def switch_category(self, subject_type, title):
        """切换类别"""
        self.main_window.switch_to_main_page()
        self.main_window.project_Button.setText(title)
        self.load_collections(subject_type, self.current_status_type)

    def on_search_text_changed(self, text):
        """搜索文本变化处理"""
        self.current_page = 1
        self.filter_collections(text)
        self.display_current_page()

    def filter_collections(self, search_text):
        """根据搜索文本过滤收藏"""
        if not search_text.strip():
            self.filtered_collections = self.all_collections.copy()
        else:
            search_text_lower = search_text.lower()
            self.filtered_collections = [
                collection for collection in self.all_collections
                if search_text_lower in (collection.get('subject_name_cn', '').lower() or '')
                or search_text_lower in (collection.get('subject_name', '').lower() or '')
            ]

    def load_collections(self, subject_type, status_type):
        """加载数据"""
        self.current_subject_type = subject_type
        self.current_status_type = status_type
        self.update_status_ui(subject_type, status_type)
        self.main_window.cache_manager.clear_pending_downloads()
        self.all_collections = get_by_subject_type_and_type(subject_type, status_type)
        self.filtered_collections = self.all_collections.copy()
        self.current_page = 1
        self.main_window.searchlist_lineEdit.clear()
        if self.all_collections:
            self.display_current_page()
        else:
            self.clear_display_area()
        self.update_page_info()

    def update_status_ui(self, subject_type, status_type):
        """更新所有状态相关的UI元素"""
        if status_names := self.status_names_map.get(subject_type):
            self.main_window.pushButton_3.setText(status_names[1])
            self.main_window.pushButton_4.setText(status_names[3])
            self.main_window.pushButton_5.setText(status_names[2])
        self.update_status_counts(subject_type)
        self.update_selection(status_type)

    def update_status_counts(self, subject_type):
        """更新状态数量显示"""
        try:
            status_counts = get_status_counts_by_subject_type(subject_type)
            for status_type, (_, _, count_button) in self.status_frames.items():
                count_button.setText(f"{status_counts.get(status_type, 0)}")
        except Exception as e:
            print(f"更新状态数量失败: {e}")

    def update_selection(self, status_type):
        """更新所有选中状态"""
        for frame, btn, _ in self.status_frames.values():
            frame.setStyleSheet("QFrame{background-color: transparent}")
            btn.setChecked(False)
        if status_type in self.status_frames:
            frame, btn, _ = self.status_frames[status_type]
            frame.setStyleSheet("QFrame{background-color: transparent; border-bottom: 3px solid rgb(103, 79, 165)}")
            btn.setChecked(True)

    def display_current_page(self):
        """显示当前页数据"""
        self.clear_display_area()
        self.setup_fixed_layout_constraints()
        start_idx = (self.current_page - 1) * self.items_per_page
        end_idx = min(start_idx + self.items_per_page, len(self.filtered_collections))
        current_page_data = self.filtered_collections[start_idx:end_idx]
        for i, collection in enumerate(current_page_data):
            row, col = divmod(i, 3)
            card = self.create_collection_card(collection)
            self.main_window.gridLayout_2.addWidget(card, row, col, Qt.AlignmentFlag.AlignCenter)
        total_cards = len(current_page_data)
        rows_needed = (total_cards + 2) // 3
        for i in range(total_cards, rows_needed * 3):
            row, col = divmod(i, 3)
            placeholder = QFrame()
            placeholder.setFixedSize(QSize(420, 170))
            placeholder.setStyleSheet("background: transparent")
            self.main_window.gridLayout_2.addWidget(placeholder, row, col)
        self.update_page_info()

    def create_collection_card(self, collection):
        """创建收藏卡片"""
        # 主框架
        card = QFrame()
        card.setFixedSize(QSize(420, 170))
        card.setStyleSheet("QFrame{background-color: rgb(242, 236, 244);border-radius: 15px}")
        card.setCursor(QCursor(Qt.CursorShape.PointingHandCursor))
        # 水平布局
        layout = QHBoxLayout(card)
        layout.setSpacing(0)
        layout.setContentsMargins(0, 0, 0, 0)
        # 封面
        cover_label = QLabel(card)
        cover_label.setFixedSize(QSize(125, 170))
        cover_label.setStyleSheet("background-color: rgb(242, 236, 244);border-top-left-radius: 15px;border-bottom-left-radius: 15px")
        cover_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        # 图片
        image_url = collection.get('subject_images_common')
        if image_url:
            load_image_with_cache(self.main_window.cache_manager, image_url, cover_label, 40, False)
        else:
            cover_label.setText("暂无封面")
        layout.addWidget(cover_label)
        # 信息框架
        info_frame = self._create_info_frame(collection, card)
        layout.addWidget(info_frame)
        # 保存数据和点击事件
        card.collection_data = collection
        card.mousePressEvent = lambda event: self.on_card_clicked(event, card)
        cover_label.mousePressEvent = lambda event: self.on_card_clicked(event, card)
        cover_label.setCursor(QCursor(Qt.CursorShape.PointingHandCursor))
        return card

    def on_card_clicked(self, event, card):
        """卡片点击事件"""
        if event.button() == Qt.MouseButton.LeftButton:
            self.show_detail_page(card.collection_data)

    def _create_info_frame(self, collection, card):
        """创建信息框架"""
        info_frame = QFrame()
        info_frame.setStyleSheet("background-color: transparent;")
        # 垂直布局
        layout = QVBoxLayout(info_frame)
        layout.setSpacing(8)
        layout.setContentsMargins(5, 5, -1, -1)
        # 标题
        title_label = QLabel(info_frame)
        title_label.setMaximumSize(QSize(16777215, 30))
        title_label.setFont(QFont("微软雅黑", 13))
        title = collection.get('subject_name_cn') or collection.get('subject_name')
        title_label.setText(title)
        layout.addWidget(title_label)
        # 进度
        progress_label = QLabel(info_frame)
        progress_label.setFont(QFont("微软雅黑", 10))
        subject_eps = collection.get('subject_eps', 0)
        progress_label.setText(f"全{subject_eps if subject_eps > 0 else '--'}话")
        layout.addWidget(progress_label)
        # 弹簧
        layout.addItem(QSpacerItem(20, 13, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding))
        # 按钮布局
        button_layout = QHBoxLayout()
        button_layout.setContentsMargins(-1, 0, 15, 15)
        # 左侧弹簧
        button_layout.addItem(QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum))
        # 状态按钮
        more_button = QPushButton(info_frame)
        more_button.setFixedSize(QSize(40, 40))
        more_button.setStyleSheet("QPushButton {background-color: rgb(242, 236, 244);border: none;border-radius: 20px}"
                                  "QPushButton:hover {background-color: rgb(216, 207, 232);border: none;border-radius: 20px}")
        more_button.setIcon(QIcon("icons/more.png"))
        more_button.clicked.connect(lambda: self.on_more_button_clicked(more_button, collection))
        button_layout.addWidget(more_button)
        # 选集
        episode_button = QPushButton(info_frame)
        episode_button.setFixedSize(QSize(60, 40))
        episode_button.setFont(QFont("微软雅黑", 10))
        episode_button.setStyleSheet("QPushButton {background-color: rgb(242, 236, 244); border: none; border-radius: 20px}"
                                     "QPushButton:hover {background-color: rgb(216, 207, 232)}")
        episode_button.setText("选集")
        episode_button.clicked.connect(lambda: self.show_episode_page(card.collection_data))
        button_layout.addWidget(episode_button, 0, Qt.AlignmentFlag.AlignRight)
        layout.addLayout(button_layout)
        return info_frame

    def on_more_button_clicked(self, button, collection):
        """更多按钮点击处理"""
        selector = show_status_selector(parent_button=button, on_status_selected=lambda status_text: self.on_status_selected(status_text, collection))
        button.setProperty('status_selector', selector)

    def setup_fixed_layout_constraints(self):
        """布局约束"""
        layout = self.main_window.gridLayout_2
        for col in range(3):
            layout.setColumnMinimumWidth(col, 420)
        layout.setVerticalSpacing(20)
        layout.setHorizontalSpacing(40)
        layout.setAlignment(Qt.AlignmentFlag.AlignTop | Qt.AlignmentFlag.AlignHCenter)

    def show_episode_page(self, collection_data):
        """显示选集页面"""
        current_index = self.main_window.main_stackedWidget.currentIndex()
        self.main_window.page_history.append(current_index)
        self.main_window.repaint()
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
            if widget := item.widget():
                widget.deleteLater()

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
            self.main_window.cache_manager.clear_pending_downloads()
            self.current_page -= 1
            self.display_current_page()

    def next_page(self):
        """下一页"""
        total_items = len(self.filtered_collections)
        total_pages = (total_items + self.items_per_page - 1) // self.items_per_page
        if self.current_page < total_pages:
            self.main_window.cache_manager.clear_pending_downloads()
            self.current_page += 1
            self.display_current_page()

    def on_status_selected(self, status_text, collection_data):
        """选择状态回调"""
        if new_type := self.status_reverse_map.get(status_text):
            thread_manager.update_status(collection_data.get('subject_id'), collection_data.get('type'), new_type, self.on_status_update_complete)

    @staticmethod
    def on_status_update_complete(success, error_msg):
        """状态更新完成回调"""
        if not success:
            print(f"状态更新失败: {error_msg}")