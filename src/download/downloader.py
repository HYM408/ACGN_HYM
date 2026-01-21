import os
import httpx
import threading
import concurrent.futures
from PySide6.QtCore import QObject, Signal
from src.config import get_config_item


class DownloadTask(QObject):
    """下载任务类"""
    progress_changed = Signal(int, int, int)
    status_changed = Signal(str)

    def __init__(self, url, file_name, save_dir=None, max_workers=4):
        super().__init__()
        self.url = url
        self.file_name = file_name
        self.save_dir = save_dir or get_config_item("download_path", "data/download")
        self.save_path = os.path.join(self.save_dir, file_name)
        self.max_workers = max_workers
        self.total_size = 0
        self.downloaded = 0
        self.status = "等待"
        self.is_running = False

    def start(self):
        """开始下载"""
        self.is_running = True
        self.status = "下载中"
        self.status_changed.emit(self.status)
        threading.Thread(target=self._download, daemon=True).start()

    def _download(self):
        """执行下载"""
        try:
            os.makedirs(self.save_dir, exist_ok=True)
            with httpx.Client(timeout=30) as client:
                response = client.head(self.url)
                self.total_size = int(response.headers.get('content-length', 0))
            if self.total_size <= 0:
                self.status = "失败"
                self.status_changed.emit(self.status)
                return
            self._download_multi_thread()
            self.status = "完成" if self.is_running else "已暂停"
            self.status_changed.emit(self.status)
        except Exception as e:
            print(f"下载出错： {e}")
            self.status = "错误"
            self.status_changed.emit(self.status)

    def _download_multi_thread(self):
        """分块下载"""
        chunk_size = self.total_size // self.max_workers
        chunks = [(i * chunk_size, (i + 1) * chunk_size - 1) for i in range(self.max_workers - 1)]
        chunks.append(((self.max_workers - 1) * chunk_size, self.total_size - 1))
        with concurrent.futures.ThreadPoolExecutor(max_workers=self.max_workers) as executor:
            futures = {executor.submit(self._download_chunk, start, end, i): i
                      for i, (start, end) in enumerate(chunks)}
            for future in concurrent.futures.as_completed(futures):
                if not self.is_running:
                    executor.shutdown(wait=False, cancel_futures=True)
                    break
                downloaded = future.result()
                self.downloaded += downloaded
                progress = int((self.downloaded / self.total_size) * 100)
                self.progress_changed.emit(progress, self.downloaded, self.total_size)
        if self.is_running:
            self._merge_chunks()

    def _download_chunk(self, start, end, chunk_id):
        """下载单个分片"""
        temp_file = f"{self.save_path}.part{chunk_id}"
        downloaded = 0
        headers = {'Range': f'bytes={start}-{end}', 'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36'}
        with httpx.Client(timeout=30) as client:
            with client.stream('GET', self.url, headers=headers) as response:
                with open(temp_file, 'wb') as f:
                    for chunk in response.iter_bytes():
                        if not self.is_running:
                            break
                        f.write(chunk)
                        downloaded += len(chunk)
        return downloaded

    def _merge_chunks(self):
        """合并分片"""
        with open(self.save_path, 'wb') as final_file:
            for i in range(self.max_workers):
                temp_file = f"{self.save_path}.part{i}"
                if os.path.exists(temp_file):
                    with open(temp_file, 'rb') as part:
                        final_file.write(part.read())
                    os.remove(temp_file)

    def pause(self):
        """暂停下载"""
        self.is_running = False
        self.status = "已暂停"
        self.status_changed.emit(self.status)

    def stop(self):
        """停止下载"""
        self.is_running = False
        for i in range(self.max_workers):
            temp_file = f"{self.save_path}.part{i}"
            if os.path.exists(temp_file):
                os.remove(temp_file)