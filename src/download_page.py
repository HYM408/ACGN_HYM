from PySide6.QtCore import QObject, Qt
from PySide6.QtWidgets import QLabel, QFrame, QVBoxLayout, QHBoxLayout, QPushButton, QSpacerItem, QSizePolicy
from src.thread_manager import thread_manager
from src.downloader.downloader import DownloadTask


class DownloadPageManager(QObject):
    """下载页面管理器"""
    def __init__(self, download_ui):
        super().__init__()
        self.download_ui = download_ui
        self.download_tasks = {}
        thread_manager.pikpak_events_loaded.connect(self.display_events)
        thread_manager.pikpak_download_url_loaded.connect(self.handle_download_url)
        self.setup_download_list()

    def setup_download_list(self):
        """设置下载列表UI"""
        layout = self.download_ui.verticalLayout_3
        layout.setSpacing(5)
        layout.setContentsMargins(5, 5, 5, 5)
        layout.addSpacerItem(QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding))

    @staticmethod
    def load_recent_files():
        """加载最近添加的文件列表"""
        thread_manager.fetch_pikpak_events()

    def display_events(self, events):
        """在UI中显示事件列表"""
        layout = self.download_ui.verticalLayout_2
        while layout.count():
            layout.takeAt(0).widget().deleteLater()
        if not events:
            label = QLabel("暂无最近添加的文件")
            label.setAlignment(Qt.AlignmentFlag.AlignCenter)
            label.setStyleSheet("font-size: 14px; color: #999; padding: 20px")
            layout.addWidget(label)
        for event in events:
            self._create_event_item(event, layout)
        layout.addStretch()

    def _create_event_item(self, event, parent_layout):
        """创建单个事件项"""
        ref_resource = event.get('reference_resource', {})
        file_name = event.get('file_name', '')
        # 框架
        item = QFrame()
        item.setFixedHeight(60)
        item.setStyleSheet("QFrame {border: 1px solid #e0e0e0}"
                           "QFrame:hover {background-color: #f0f0f0}")
        # 事件数据
        item.event_data = event
        # 点击事件
        item.mousePressEvent = lambda e, clicked_item=item: self._on_item_clicked(clicked_item)
        item_layout = QVBoxLayout(item)
        item_layout.setContentsMargins(10, 5, 10, 5)
        # 文件名
        name_label = QLabel(file_name)
        name_label.setStyleSheet("font-size: 14px; background-color: transparent; border: none")
        name_label.setWordWrap(True)
        item_layout.addWidget(name_label)
        # 类型和大小
        type_label = QLabel(self._get_type_size_text(ref_resource))
        type_label.setStyleSheet("font-size: 12px; background-color: transparent; border: none")
        item_layout.addWidget(type_label)
        parent_layout.addWidget(item)

    @staticmethod
    def _on_item_clicked(item):
        """点击事件"""
        ref_resource = item.event_data.get('reference_resource', {})
        if ref_resource.get('kind') == 'drive#folder':
            return
        file_id = ref_resource.get('id')
        thread_manager.fetch_pikpak_download_url(file_id)

    def handle_download_url(self, result):
        """处理获取到的下载链接"""
        if not result:
            return
        download_url = self._extract_download_url(result)
        if download_url:
            file_name = result.get('name', '')
            if not file_name:
                file_name = download_url.split('/')[-1].split('?')[0] or "download.file"
            self.add_download_task(download_url, file_name)

    @staticmethod
    def _extract_download_url(result):
        """从结果中提取下载链接"""
        for media in result.get('medias', []):
            if media.get('link', {}).get('url'):
                return media['link']['url']
        if result.get('web_content_link'):
            return result['web_content_link']
        for link_data in result.get('links', {}).values():
            if link_data.get('url'):
                return link_data['url']
        return None

    def add_download_task(self, url, file_name):
        """添加下载任务并显示在UI中"""
        task = DownloadTask(url, file_name)
        self.download_tasks[file_name] = task
        task_widget = self._create_download_item(file_name)
        task.progress_changed.connect(lambda progress, downloaded, total, w=task_widget: self._update_download_progress(w, progress, downloaded, total))
        task.status_changed.connect(lambda status, w=task_widget: self._update_download_status(w, status))
        layout = self.download_ui.verticalLayout_3
        if layout.count() > 0:
            layout.takeAt(layout.count() - 1)
        layout.addWidget(task_widget)
        layout.addSpacerItem(QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding))
        task.start()

    def _create_download_item(self, file_name):
        """创建下载任务UI项"""
        widget = QFrame()
        widget.setFixedHeight(50)
        widget.setStyleSheet("QFrame {border: 2px solid #9E9E9E; border-radius: 4px; background-color: transparent}")
        # 创建进度背景
        progress_frame = QFrame(widget)
        progress_frame.setGeometry(0, 0, 0, 50)
        progress_frame.setStyleSheet("background-color: rgba(76, 175, 80, 0.3);border: none; border-radius: 3px")
        progress_frame.lower()
        # 内容布局
        layout = QHBoxLayout(widget)
        layout.setContentsMargins(10, 5, 10, 5)
        # 状态
        status_label = QLabel("等待")
        status_label.setStyleSheet("font-size: 13px; font-weight: bold; background-color: transparent; border: none; min-width: 40px")
        status_label.setFixedWidth(40)
        layout.addWidget(status_label)
        # 文件名
        name_label = QLabel(file_name)
        name_label.setStyleSheet("font-size: 13px; font-weight: bold; background-color: transparent; border: none")
        name_label.setWordWrap(True)
        layout.addWidget(name_label, 1)
        # 进度文字
        progress_label = QLabel("0B/0B")
        progress_label.setStyleSheet("font-size: 13px; font-weight: bold; background-color: transparent; border: none")
        layout.addWidget(progress_label)
        # 删除按钮
        remove_btn = QPushButton("删除")
        remove_btn.setFixedSize(50, 25)
        remove_btn.setStyleSheet("QPushButton {font-size: 11px; color: #ff6b6b; background-color: transparent; border: 1px solid #ff6b6b; border-radius: 3px}"
                                 "QPushButton:hover {background-color: rgba(255, 107, 107, 0.1)}")
        remove_btn.clicked.connect(lambda: self._remove_task(widget))
        layout.addWidget(remove_btn)
        # 保存引用
        widget.file_name = file_name
        widget.progress_frame = progress_frame
        widget.status_label = status_label
        widget.name_label = name_label
        widget.progress_label = progress_label
        widget.remove_btn = remove_btn
        # 点击暂停/继续
        widget.mousePressEvent = lambda e, w=widget: self._toggle_pause(w)
        return widget

    def _update_download_progress(self, widget, progress, downloaded, total):
        """更新下载进度"""
        if progress > 0:
            progress_width = int(widget.width() * progress / 100)
            widget.progress_frame.setFixedWidth(progress_width)
        downloaded_str = self._format_size(downloaded)
        total_str = self._format_size(total)
        widget.progress_label.setText(f"{downloaded_str}/{total_str}")

    @staticmethod
    def _update_download_status(widget, status):
        """更新下载状态"""
        widget.status_label.setText(status)
        color_map = {"下载中": "#4CAF50", "已暂停": "#FF9800", "完成": "#2196F3", "错误": "#F44336", "等待": "#9E9E9E"}
        border_color = color_map.get(status, "#9E9E9E")
        widget.setStyleSheet(f"QFrame {{border: 2px solid {border_color}; border-radius: 4px; background-color: transparent}}")

    def _toggle_pause(self, widget):
        """切换暂停/继续"""
        file_name = widget.file_name
        if file_name in self.download_tasks:
            task = self.download_tasks[file_name]
            if task.status == "下载中":
                task.pause()
            elif task.status == "已暂停":
                task.status = "下载中"
                task.is_running = True
                task.start()

    def _remove_task(self, widget):
        """删除下载任务"""
        file_name = widget.file_name
        if file_name in self.download_tasks:
            task = self.download_tasks[file_name]
            task.stop()
            del self.download_tasks[file_name]
        widget.setParent(None)
        widget.deleteLater()

    @staticmethod
    def _format_size(size):
        """格式化文件大小"""
        for unit in ['B', 'KB', 'MB', 'GB']:
            if size < 1024.0:
                return f"{size:.1f}{unit}"
            size /= 1024.0
        return f"{size:.1f}TB"

    @staticmethod
    def _get_type_size_text(ref_resource):
        """获取类型和大小"""
        if ref_resource.get('kind') == 'drive#folder':
            return "文件夹"
        mime_type = ref_resource.get('mime_type', '')
        size = ref_resource.get('size', '0')
        file_type = "视频" if 'video' in mime_type else "文件"
        if size.isdigit():
            size_int = int(size)
            return f"{file_type} {DownloadPageManager._format_size(size_int)}"
        return f"{file_type} 0B"