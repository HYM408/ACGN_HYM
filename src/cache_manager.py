import hashlib
from pathlib import Path
from PySide6.QtGui import QPixmap
from PySide6.QtCore import QObject, Signal, QUrl
from PySide6.QtNetwork import QNetworkAccessManager, QNetworkRequest, QNetworkReply


class CacheManager(QObject):
    """批量缓存图片"""
    download_complete = Signal(str, QPixmap)

    def __init__(self, parent=None):
        super().__init__(parent)
        self.cache_dir = Path('data') / 'images'
        self.cache_dir.mkdir(parents=True, exist_ok=True)
        self.network_manager = QNetworkAccessManager(self)
        self.network_manager.finished.connect(self._on_download_finished)
        self.pending_callbacks = {}
        self.download_complete.connect(self._on_download_complete)

    def get_image_async(self, url, callback):
        """异步获取图片"""
        if not url or not callback:
            return
        url_hash = hashlib.md5(url.encode()).hexdigest()
        cached_path = self.cache_dir / f"{url_hash}.jpg"
        if cached_path.exists():
            callback(QPixmap(str(cached_path)))
            return
        if url_hash not in self.pending_callbacks:
            self.pending_callbacks[url_hash] = []
        self.pending_callbacks[url_hash].append(callback)
        if len(self.pending_callbacks[url_hash]) == 1:
            request = QNetworkRequest(QUrl(url))
            request.setAttribute(QNetworkRequest.Attribute.User, url_hash)
            self.network_manager.get(request)

    def _on_download_finished(self, reply):
        """下载完成处理"""
        url_hash = reply.request().attribute(QNetworkRequest.Attribute.User)
        if reply.error() == QNetworkReply.NetworkError.NoError:
            image_data = reply.readAll()
            pixmap = QPixmap()
            if pixmap.loadFromData(image_data):
                file_path = self.cache_dir / f"{url_hash}.jpg"
                file_path.write_bytes(image_data.data())
                self.download_complete.emit(url_hash, pixmap)
            else:
                self.download_complete.emit(url_hash, QPixmap())
        else:
            self.download_complete.emit(url_hash, QPixmap())
        reply.deleteLater()

    def _on_download_complete(self, url_hash, pixmap):
        """下载完成回调"""
        if url_hash in self.pending_callbacks:
            for callback in self.pending_callbacks[url_hash]:
                if callback:
                    callback(pixmap)
            del self.pending_callbacks[url_hash]

    def clear_pending_downloads(self):
        """清空所有待处理的下载任务"""
        self.pending_callbacks.clear()