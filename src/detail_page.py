import re
from datetime import datetime
from PySide6.QtGui import QFont, QDesktopServices
from PySide6.QtCore import QObject, Signal, QUrl, Qt
from PySide6.QtWidgets import QLabel, QVBoxLayout, QWidget, QHBoxLayout
from src.cache_manager import CacheManager
from src.thread_manager import thread_manager
from src.menu_utils import show_status_selector
from src.image_utils import load_image_with_cache
from src.sqlite import get_subject_by_id, insert_subject_from_api


class DetailManager(QObject):
    request_show_episode = Signal(dict)
    subject_data_loaded = Signal(dict)
    tag_clicked = Signal(str)

    def __init__(self, detail_page_widget):
        super().__init__()
        self.detail_page = detail_page_widget
        self.collection_data = None
        self.cache_manager = CacheManager(self)
        # 状态
        self.status_map = {0: "追番", 1: "想看", 2: "看过", 3: "在看", 4: "搁置", 5: "抛弃"}
        self.status_reverse_map = {"取消追番": 0, "想看": 1, "看过": 2, "在看": 3, "搁置": 4, "抛弃": 5}
        self.setup_connections()
        self.subject_data_loaded.connect(self.update_detail_page)

    def setup_connections(self):
        """连接"""
        self.detail_page.pushButton_27.clicked.connect(self.on_episode_clicked)
        self.detail_page.pushButton_26.clicked.connect(self.on_status_button_clicked)
        self.detail_page.pushButton_20.clicked.connect(self.on_open_bangumi_page)
        # 连接线程管理器
        thread_manager.subject_data_fetched.connect(self.on_subject_data_fetched)

    def on_episode_clicked(self):
        """选集"""
        self.request_show_episode.emit(self.collection_data)

    def on_status_button_clicked(self):
        """状态下拉菜单"""
        self.status_selector = show_status_selector(parent_button=self.detail_page.pushButton_26,on_status_selected=self.on_status_selected)

    def on_open_bangumi_page(self):
        """跳转Bangumi"""
        if self.collection_data and 'subject_id' in self.collection_data:
            from src.config import get_config_item
            base_url = get_config_item("bangumi_base_url")
            bangumi_url = f"{base_url}subject/{self.collection_data['subject_id']}"
            QDesktopServices.openUrl(QUrl(bangumi_url))

    def on_status_selected(self, status_text):
        """选择状态回调"""
        new_type = self.status_reverse_map.get(status_text)
        if new_type is None:
            return
        old_type = self.collection_data.get('type')
        self.collection_data['type'] = new_type
        self.status_show()
        thread_manager.update_status(self.collection_data.get('subject_id'),old_type,new_type,self.on_status_update_complete)

    def on_status_update_complete(self, success, error_msg):
        """状态更新完成回调"""
        if not success:
            print(f"状态更新失败: {error_msg}")

    def status_show(self):
        """状态映射"""
        status_type = self.collection_data.get('type')
        self.detail_page.pushButton_26.setText(self.status_map.get(status_type, "追番"))

    def detail_page_data(self, collection_data):
        """collection_data传入数据"""
        self.collection_data = collection_data
        self.status_show()
        load_image_with_cache(self.cache_manager, collection_data.get('subject_images_common'),self.detail_page.cover_label_3, radius=15, all_corners=True)
        self.detail_page.textEdit.setText(collection_data.get('subject_name_cn') or collection_data.get('subject_name'))
        db_data = get_subject_by_id(collection_data.get('subject_id'))
        if db_data:
            self.update_detail_page(db_data)
        else:
            thread_manager.fetch_subject_data(collection_data.get('subject_id'), self.on_subject_data_fetched)

    def on_subject_data_fetched(self, subject_data):
        """subject数据获取完成的回调"""
        if subject_data:
            subject_data['collection_data'] = None
            insert_subject_from_api(subject_data)
            self.subject_data_loaded.emit(subject_data)

    def update_detail_page(self, subject_data):
        """详情页信息"""
        # 评分
        rating_info = subject_data.get('rating', {})
        score = rating_info.get('score', subject_data.get('rating_score', 'N/A'))
        total = rating_info.get('total', subject_data.get('rating_total', 'N/A'))
        rank = rating_info.get('rank', subject_data.get('rating_rank', 'N/A'))
        self.detail_page.pushButton_21.setText(f"{score}|{total}人评|#{rank}")
        # 话数
        self.detail_page.pushButton_24.setText(f"全{subject_data.get('total_episodes')}话")
        # 收藏数
        collection_info = subject_data.get('collection', {})
        collect = collection_info.get('collect', subject_data.get('collect', 0))
        on_hold = collection_info.get('on_hold', subject_data.get('on_hold', 0))
        dropped = collection_info.get('dropped', subject_data.get('dropped', 0))
        wish = collection_info.get('wish', subject_data.get('wish', 0))
        doing = collection_info.get('doing', subject_data.get('doing', 0))
        total_collect = collect + on_hold + dropped + wish + doing
        self.detail_page.pushButton_25.setText(f"{total_collect}收藏/{doing}在看/{dropped}抛弃")
        # 简介
        self.detail_page.textEdit_2.setText(subject_data.get('summary'))
        # tags
        tags_list = subject_data.get('tags', [])
        formatted_tags = []
        for tag in tags_list:
            if isinstance(tag, dict):
                formatted_tags.append([tag.get('count'), tag.get('name')])
            elif isinstance(tag, list):
                formatted_tags.append(tag)
        self.tags_display(formatted_tags)
        # 时间
        time_tag = self.find_time_tag(formatted_tags)
        if time_tag:
            self.detail_page.pushButton_23.setText(time_tag)
        else:
            date_str = subject_data.get('date')
            time_tag = self.extract_time_from_date(date_str)
            self.detail_page.pushButton_23.setText(time_tag)

    def find_time_tag(self, tags_list):
        """从tags中查找时间"""
        time_pattern = re.compile(r'\d{4}年\d{1,2}月')
        for tag in tags_list:
            if isinstance(tag, list) and len(tag) >= 2:
                tag_name = tag[1]
                match = time_pattern.search(tag_name)
                if match:
                    return match.group()
        return None

    def extract_time_from_date(self, date_str):
        """从date中提取时间"""
        if not date_str:
            return "TBA"
        try:
            date_obj = datetime.strptime(date_str, '%Y-%m-%d')
            return f"{date_obj.year}年{date_obj.month}月"
        except ValueError:
            return "TBA"

    def tags_display(self, tags_list):
        """tags显示"""
        frame_5 = self.detail_page.frame_5
        layout = frame_5.layout()
        if layout is None:
            layout = QVBoxLayout(frame_5)
            layout.setSpacing(5)
            layout.setContentsMargins(0, 0, 0, 0)
        else:
            while layout.count():
                item = layout.takeAt(0)
                if item.widget():
                    item.widget().deleteLater()
        line_layout = None
        current_width = 0
        max_width = frame_5.width()
        for tag in tags_list:
            if isinstance(tag, list) and len(tag) >= 2:
                tag_count, tag_name = tag[0], tag[1]
                tag_label = self.create_tag_label(tag_name, tag_count, frame_5)
                tag_label.adjustSize()
                label_width = tag_label.width()
                if line_layout is None or current_width + label_width > max_width:
                    line_widget = QWidget()
                    line_layout = QHBoxLayout(line_widget)
                    line_layout.setSpacing(8)
                    line_layout.setContentsMargins(0, 0, 0, 0)
                    line_layout.addStretch(1)
                    layout.addWidget(line_widget)
                    current_width = 0
                line_layout.insertWidget(line_layout.count() - 1, tag_label)
                current_width += label_width + 8

    def create_tag_label(self, tag_name, tag_count, parent):
        """创建标签控件"""
        tag_label = QLabel(f'<span style="color:black">{tag_name}</span> <span style="color:gray">{tag_count}</span>', parent)
        tag_label.setFont(QFont("微软雅黑", 10))
        tag_label.setStyleSheet("""
            QLabel {
                border: 1px solid gray;
                border-radius: 8px;
                padding: 2px 8px;
                background-color: white;
            }
            QLabel:hover {
                background-color: #f0f0f0;
            }
        """)
        tag_label.setCursor(Qt.PointingHandCursor)
        tag_label.setProperty("tag_name", tag_name)
        tag_label.setProperty("tag_count", tag_count)
        tag_label.mousePressEvent = lambda event: self.on_tag_clicked_handler(tag_label, event)
        return tag_label

    def on_tag_clicked_handler(self, tag_label, event):
        """标签点击事件处理"""
        if event.button() == Qt.LeftButton:
            tag_name = tag_label.property("tag_name")
            self.tag_clicked.emit(tag_name)

    def clear_detail_page(self):
        """清空详情页数据"""
        self.detail_page.cover_label_3.clear()
        self.detail_page.cover_label_3.setText("封面")
        self.detail_page.textEdit.clear()
        self.detail_page.textEdit_2.clear()
        self.detail_page.pushButton_23.setText("TBA")
        self.detail_page.pushButton_21.setText("|人评|#")
        self.detail_page.pushButton_24.setText("全-话")
        self.detail_page.pushButton_25.setText("收藏/在看/抛弃")
        self.detail_page.pushButton_26.setText("追番")
        frame_5 = self.detail_page.frame_5
        if layout := frame_5.layout():
            while layout.count():
                item = layout.takeAt(0)
                if item.widget():
                    item.widget().deleteLater()
        self.collection_data = None