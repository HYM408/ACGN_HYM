import os
import httpx
import threading
from PySide6.QtCore import QObject, Signal
from src.config import get_config_item


class DownloadTask(QObject):
    """下载任务类"""
    progress_changed = Signal(int, int, int)
    status_changed = Signal(str)

    def __init__(self, url, file_name, save_dir=None):
        super().__init__()
        self.url = url
        self.file_name = file_name
        if save_dir is None:
            save_dir = get_config_item("download_path", "data/download")
        self.save_path = os.path.join(save_dir, file_name)
        self.total_size = 0
        self.downloaded = 0
        self.status = "等待"
        self.is_running = False
        self.client = None

    def start(self):
        """开始下载"""
        self.is_running = True
        self.status = "下载中"
        self.status_changed.emit(self.status)
        threading.Thread(target=self._download, daemon=True).start()

    def _download(self):
        """执行下载"""
        try:
            os.makedirs(os.path.dirname(self.save_path), exist_ok=True)
            self.client = httpx.Client(timeout=30)
            response = self.client.head(self.url)
            self.total_size = int(response.headers.get('content-length', 0))
            self.progress_changed.emit(0, 0, self.total_size)
            if self.total_size == 0:
                self.status = "失败"
                self.status_changed.emit(self.status)
                return
            if os.path.exists(self.save_path):
                self.downloaded = os.path.getsize(self.save_path)
            headers = {'Range': f'bytes={self.downloaded}-'} if self.downloaded > 0 else {}
            mode = 'ab' if self.downloaded > 0 else 'wb'
            with self.client.stream('GET', self.url, headers=headers) as response:
                with open(self.save_path, mode) as f:
                    for chunk in response.iter_bytes():
                        if not self.is_running:
                            break
                        f.write(chunk)
                        self.downloaded += len(chunk)
                        progress = int((self.downloaded / self.total_size) * 100)
                        self.progress_changed.emit(progress, self.downloaded, self.total_size)
            self.status = "完成" if self.is_running else "已暂停"
            self.status_changed.emit(self.status)
        except Exception as e:
            print(f"下载错误: {e}")
            self.status = "错误"
            self.status_changed.emit(self.status)
        finally:
            if self.client:
                self.client.close()

    def pause(self):
        """暂停下载"""
        self.is_running = False
        self.status = "已暂停"
        self.status_changed.emit(self.status)

    def stop(self):
        """停止下载"""
        self.is_running = False
        if self.client:
            self.client.close()