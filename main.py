import sys
import time
import atexit
from PySide6.QtCore import Qt
from PySide6.QtWidgets import QApplication, QStyle
from src.rss import rss_manager
from src.window import MainWindow
from src.config import create_config


def cleanup():
    """退出时清理资源"""
    from src.sqlite import cleanup
    from src.thread_manager import thread_manager
    cleanup()
    thread_manager.cleanup()
    print("资源清理完成")


if __name__ == "__main__":
    start_time = time.time()
    # 创建配置
    create_config()
    # 注册退出清理函数
    atexit.register(cleanup)
    # 创建应用
    app = QApplication(sys.argv)
    # 创建主窗口
    window = MainWindow()
    window.setGeometry(QStyle.alignedRect(Qt.LeftToRight,Qt.AlignCenter,window.size(),QApplication.primaryScreen().availableGeometry()))
    window.show()
    # 设置RSS定时器
    rss_manager.setup_rss_timer()
    # 启动耗时
    end_time = time.time()
    print(f"{end_time - start_time}秒")
    # 运行
    exit_code = app.exec()
    # 退出
    sys.exit(exit_code)