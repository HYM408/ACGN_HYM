from PySide6.QtGui import QFont, QCursor
from PySide6.QtCore import Qt
from PySide6.QtWidgets import QFrame, QHBoxLayout, QLabel, QVBoxLayout, QSizePolicy, QSpacerItem
from src.api import BangumiAPI
from src.image_utils import load_image_with_cache
from src.sqlite import get_by_subject_id
from src.cache_manager import CacheManager
from src.thread_manager import thread_manager


class SearchPageManager:
    def __init__(self, widget, ui, main_window):
        self.ui = ui
        self.main_window = main_window
        self.bangumi_api = BangumiAPI()
        self.status_label = None
        self.cache_manager = CacheManager(widget)
        self.setup_connections()

    def setup_connections(self):
        """设置连接"""
        # 搜索框
        self.ui.search_lineEdit.returnPressed.connect(self.search_by_keyword)
        # 连接线程管理器
        thread_manager.search_finished.connect(self.on_search_finished)
        thread_manager.search_error.connect(self.on_search_error)

    def update_combo_box_by_type(self, current_type):
        """更新下拉框状态"""
        if current_type == 2:
            self.ui.comboBox.setCurrentIndex(0)
        elif current_type in [7, 8]:
            self.ui.comboBox.setCurrentIndex(1)
        elif current_type == 4:
            self.ui.comboBox.setCurrentIndex(2)

    def search_by_keyword(self):
        """执行关键词搜索"""
        keyword = self.ui.search_lineEdit.text().strip()
        if not keyword:
            return
        self._do_search(keyword=keyword)

    def search_by_tag(self, tag):
        """执行tag搜索"""
        if not tag:
            return
        # 清空搜索框，避免混淆
        self.ui.search_lineEdit.setText(tag)
        self._do_search(tag=tag)

    def _do_search(self, keyword=None, tag=None):
        """执行搜索的内部方法"""
        type_mapping = {0: 2, 1: 1, 2: 4}
        search_type = type_mapping.get(self.ui.comboBox.currentIndex())
        self.show_search_status("搜索中...")
        thread_manager.search_subjects(keyword, tag, search_type)

    def on_search_finished(self, results):
        """搜索完成处理"""
        if results:
            self.display_search_results(results)
        else:
            self.clear_search_results()
            self.show_search_status("未找到相关结果")

    def on_search_error(self, error_msg):
        """搜索错误处理"""
        self.clear_search_results()
        self.show_search_status("搜索失败")

    def display_search_results(self, results):
        """显示搜索结果"""
        self.clear_search_results()
        layout = self.ui.verticalLayout_10
        for result in results:
            layout.addWidget(self.create_result_frame(result))
        layout.addItem(QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding))

    def create_result_frame(self, result):
        """创建搜索结果框架"""
        # 创建主框架
        animation_frame = QFrame()
        animation_frame.setMinimumSize(0, 130)
        animation_frame.setMaximumSize(450, 130)
        animation_frame.setStyleSheet("""
            QFrame{
                background-color: rgb(242, 236, 244);
            }
        """)
        animation_frame.setFrameShape(QFrame.StyledPanel)
        animation_frame.setFrameShadow(QFrame.Raised)
        animation_frame.setCursor(QCursor(Qt.PointingHandCursor))
        # 传入数据
        subject_id = result.get('id')
        collection_data = get_by_subject_id(subject_id)
        if collection_data:
            animation_frame.result_data = collection_data
        else:
            animation_frame.result_data = {
                'subject_id': subject_id,
                'subject_name': result.get('name'),
                'subject_name_cn': result.get('name_cn'),
                'subject_images_common': result.get('images', {}).get('common', ''),
                'type': 0,
            }
        def on_frame_clicked(event):
            if event.button() == Qt.LeftButton:
                current_index = self.main_window.main_stackedWidget.currentIndex()
                self.main_window.page_history.append(current_index)
                self.main_window.show_detail_page(animation_frame.result_data)
        animation_frame.mousePressEvent = on_frame_clicked
        # 水平布局
        horizontal_layout = QHBoxLayout(animation_frame)
        horizontal_layout.setSpacing(0)
        horizontal_layout.setContentsMargins(0, 0, 0, 0)
        # 封面
        cover_label = QLabel(animation_frame)
        cover_label.setFixedSize(100, 130)
        cover_label.setStyleSheet("""
            background-color: rgb(242, 236, 244);
            border-top-left-radius: 15px;
            border-bottom-left-radius: 15px;
        """)
        cover_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        cover_label.setText("加载中...")
        cover_label.setCursor(QCursor(Qt.PointingHandCursor))
        cover_label.mousePressEvent = on_frame_clicked
        # 加载图片
        image_url = result.get('images', {}).get('common', '')
        if image_url:
            load_image_with_cache(self.cache_manager, image_url, cover_label)
        else:
            cover_label.setText("暂无封面")
        horizontal_layout.addWidget(cover_label)
        # 垂直布局
        info_layout = QVBoxLayout()
        info_layout.setContentsMargins(5, -1, -1, -1)
        # 标题
        title_label = QLabel(animation_frame)
        title_label.setMinimumSize(0, 30)
        title_label.setMaximumSize(16777215, 30)
        title_label.setWordWrap(False)
        # 设置标题
        title = result.get('name', '未知标题')
        if result.get('name_cn'):
            title = result.get('name_cn')
        title_label.setText(title)
        # 字体
        font = QFont()
        font.setFamilies(["微软雅黑"])
        font.setPointSize(13)
        font.setBold(False)
        title_label.setFont(font)
        info_layout.addWidget(title_label)
        # 弹簧
        info_layout.addItem(QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding))
        horizontal_layout.addLayout(info_layout)
        return animation_frame

    def show_search_status(self, text):
        """显示搜索状态标签"""
        self.clear_search_results()
        layout = self.ui.verticalLayout_10
        if not self.status_label:
            self.status_label = QLabel()
            self.status_label.setAlignment(Qt.AlignCenter)
            font = QFont()
            font.setFamilies(["微软雅黑"])
            font.setPointSize(14)
            self.status_label.setFont(font)
            self.status_label.setStyleSheet("color: #666; padding: 20px;")
        self.status_label.setText(text)
        layout.addWidget(self.status_label)
        layout.addItem(QSpacerItem(20, 40, QSizePolicy.Minimum, QSizePolicy.Policy.Expanding))
        layout.addItem(QSpacerItem(20, 40, QSizePolicy.Minimum, QSizePolicy.Policy.Expanding))

    def clear_search_results(self):
        """清空布局和状态标签"""
        layout = self.ui.verticalLayout_10
        if self.status_label:
            layout.removeWidget(self.status_label)
            self.status_label.deleteLater()
            self.status_label = None
        while layout.count():
            item = layout.takeAt(0)
            if item.widget():
                item.widget().deleteLater()
            elif item.layout():
                sub_layout = item.layout()
                while sub_layout.count():
                    sub_item = sub_layout.takeAt(0)
                    if sub_item.widget():
                        sub_item.widget().deleteLater()
        layout.addItem(QSpacerItem(20, 40, QSizePolicy.Minimum, QSizePolicy.Policy.Expanding))