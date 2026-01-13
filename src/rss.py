from PySide6.QtCore import QObject, Signal
from src.thread_manager import thread_manager


class RSSManager(QObject):
    """RSS定期检查更新"""
    update_finished = Signal(bool)

    def __init__(self, parent=None):
        super().__init__(parent)
        self.parent = parent
        self.rss_timer = None

    def handle_update_result(self, success: bool, message: str):
        """处理更新结果"""
        if success:
            print(f"RSS: {message}")
        else:
            print(f"RSS: {message}")
        self.update_finished.emit(success)

    def setup_rss_timer(self):
        """设置RSS定时器"""
        # 连接线程管理器
        thread_manager.rss_update_finished.connect(self.handle_update_result)
        # 设置RSS
        thread_manager.setup_rss_timer()


rss_manager = RSSManager()