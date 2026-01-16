import weakref
from PySide6.QtCore import Qt
from PySide6.QtGui import QPixmap, QPainter, QPainterPath


def create_rounded_pixmap(pixmap: QPixmap, radius: int, all_corners: bool = False) -> QPixmap:
    """图片圆角处理工具函数"""
    try:
        result = QPixmap(pixmap.size())
        result.fill(Qt.GlobalColor.transparent)
        painter = QPainter(result)
        painter.setRenderHint(QPainter.RenderHint.Antialiasing)
        painter.setRenderHint(QPainter.RenderHint.SmoothPixmapTransform)
        path = QPainterPath()
        width = pixmap.width()
        height = pixmap.height()
        if all_corners:
            path.addRoundedRect(0, 0, width, height, radius, radius)
        else:
            diameter = 2 * radius
            path.moveTo(radius, 0)
            path.lineTo(width, 0)
            path.lineTo(width, height)
            path.lineTo(radius, height)
            path.arcTo(0, height - diameter, diameter, diameter, 270, -90)
            path.arcTo(0, 0, diameter, diameter, 180, -90)
            path.closeSubpath()
        painter.setClipPath(path)
        painter.drawPixmap(0, 0, pixmap)
        painter.end()
        return result
    except Exception:
        return pixmap

def load_image_with_cache(cache_manager, url, cover_label, radius: int = 0, all_corners: bool = False):
    """加载图片并应用缓存的通用函数"""
    weak_label = weakref.ref(cover_label)
    def on_image_loaded(pixmap):
        """图片加载回调"""
        label = weak_label()
        if label is None:
            return
        if pixmap and not pixmap.isNull():
            width = pixmap.width()
            height = pixmap.height()
            if width >= height:
                scaled_pixmap = pixmap.scaledToWidth(label.width(), Qt.TransformationMode.SmoothTransformation)
            else:
                if radius > 0:
                    rounded_pixmap = create_rounded_pixmap(pixmap, radius, all_corners)
                else:
                    rounded_pixmap = pixmap
                scaled_pixmap = rounded_pixmap.scaled(label.width(),label.height(),Qt.AspectRatioMode.IgnoreAspectRatio,Qt.TransformationMode.SmoothTransformation)
            label.setPixmap(scaled_pixmap)
            label.setText("")
        else:
            label.setText("加载失败")
    cache_manager.get_image_async(url, on_image_loaded)