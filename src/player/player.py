import ctypes
from PySide6.QtCore import Qt, QTimer, QRect, QPoint
from PySide6.QtGui import QPainter, QColor, QPaintEvent, QMouseEvent, QKeyEvent, QImage
from PySide6.QtWidgets import QWidget, QVBoxLayout
import os
vlc_dir = os.path.dirname(r"D:\Python\Project\ACGN_HYM\VLC\libvlc.dll")
os.environ["PATH"] = vlc_dir + os.pathsep + os.environ["PATH"]
import vlc


class VlcPlayer:
    """VLC播放器"""

    def __init__(self):
        vlc_args = ["--no-xlib", "--quiet", "--avcodec-fast", "--avcodec-threads=1"]
        self.instance = vlc.Instance(vlc_args)
        self.player = self.instance.media_player_new()
        self.width, self.height = 1920, 1080
        self.stride = self.width * 4
        self.buffer = None
        self.frame_ready = False
        self.current_frame = None
        self.setup_callbacks()

    def setup_callbacks(self):
        """设置VLC回调函数"""
        VideoLockCb = ctypes.CFUNCTYPE(ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p))
        VideoUnlockCb = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p))
        VideoDisplayCb = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_void_p)

        def lock_cb(opaque, planes):
            if not self.buffer:
                self.buffer = (ctypes.c_uint8 * (self.width * self.height * 4))()
                self.current_frame = None
            planes[0] = ctypes.addressof(self.buffer)
            return None

        def unlock_cb(opaque, picture, planes):
            self.frame_ready = True

        def display_cb(opaque, picture):
            pass

        self._lock_cb = VideoLockCb(lock_cb)
        self._unlock_cb = VideoUnlockCb(unlock_cb)
        self._display_cb = VideoDisplayCb(display_cb)
        self.player.video_set_callbacks(self._lock_cb, self._unlock_cb, self._display_cb, None)
        self.player.video_set_format("RV32", self.width, self.height, self.stride)

    def play(self, video_url):
        """播放视频"""
        if not self.player:
            print("播放器未初始化")
            return False
        try:
            media = self.instance.media_new(video_url)
            media.add_option(":avcodec-hw=dxva2")
            media.add_option(":network-caching=100")
            media.add_option(":file-caching=100")
            media.add_option(":disc-caching=100")
            self.player.set_media(media)
            self.player.play()
            print(f"开始播放: {video_url}")
            return True
        except Exception as e:
            print(f"播放失败: {e}")
            return False

    def cleanup(self):
        """清理资源"""
        if self.player:
            self.player.stop()
            self.player.release()
        self.buffer = None
        self.current_frame = None
        self.frame_ready = False
        # 清理回调函数
        self._lock_cb = None
        self._unlock_cb = None
        self._display_cb = None
        if self.instance:
            self.instance.release()
        print("VLC播放器已清理")

    def get_frame_data(self):
        """获取帧数据"""
        if not self.frame_ready:
            return None
        self.frame_ready = False
        if self.current_frame is None and self.buffer:
            self.current_frame = memoryview(self.buffer)
        return self.current_frame

    def is_playing(self):
        return self.player.is_playing() if self.player else False

    def get_position(self):
        return self.player.get_position() if self.player else 0

    def set_position(self, pos):
        if self.player:
            self.player.set_position(pos)


class VideoWidget(QWidget):
    """视频显示部件"""

    def __init__(self, parent=None):
        super().__init__(parent)
        self.video_width = 1920
        self.video_height = 1080
        self.current_qimage = None
        self.progress_visible = True
        self.progress_position = 0.0
        self.dragging = False
        self.bg_color = QColor(100, 100, 100, 150)
        self.prog_color = QColor(220, 50, 50, 180)
        self.point_color = QColor(255, 255, 255, 200)
        self.hide_timer = QTimer(self)
        self.hide_timer.timeout.connect(self._hide_progress)
        self.hide_timer.setSingleShot(True)
        self.setMouseTracking(True)
        self.setFocusPolicy(Qt.StrongFocus)

    def _hide_progress(self):
        """隐藏进度条"""
        self.progress_visible = False
        self.update()

    def update_frame(self, buffer):
        """更新视频帧数据"""
        if buffer:
            if self.current_qimage is None:
                self.current_qimage = QImage(buffer, self.video_width, self.video_height,  self.video_width * 4, QImage.Format_ARGB32)
            self.update()

    def paintEvent(self, event: QPaintEvent):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        painter.setRenderHint(QPainter.SmoothPixmapTransform)
        if self.current_qimage:
            scaled = self.current_qimage.scaled(self.size(), Qt.KeepAspectRatio, Qt.SmoothTransformation)
            x = (self.width() - scaled.width()) // 2
            y = (self.height() - scaled.height()) // 2
            painter.drawImage(x, y, scaled)
        if self.progress_visible:
            self._draw_progress_bar(painter)

    def _draw_progress_bar(self, painter):
        """绘制进度条"""
        w, h = self.width(), self.height()
        bar_y = h - 30
        painter.setPen(Qt.NoPen)
        painter.setBrush(self.bg_color)
        painter.drawRect(20, bar_y - 3, w - 40, 6)
        if self.progress_position > 0:
            prog_width = int((w - 40) * self.progress_position)
            painter.setBrush(self.prog_color)
            painter.drawRect(20, bar_y - 3, prog_width, 6)
            point_x = 20 + prog_width
            painter.setBrush(self.point_color)
            painter.drawEllipse(QPoint(point_x, bar_y), 4, 4)

    def set_progress(self, pos):
        self.progress_position = pos
        if self.progress_visible:
            self.update()

    def show_progress(self):
        self.progress_visible = True
        self.update()
        self.hide_timer.start(3000)

    def mousePressEvent(self, event: QMouseEvent):
        if event.button() == Qt.LeftButton:
            self.dragging = True
            self._update_position(event.position().toPoint())

    def mouseMoveEvent(self, event: QMouseEvent):
        if self.dragging:
            self._update_position(event.position().toPoint())
        self.show_progress()

    def mouseReleaseEvent(self, event: QMouseEvent):
        if event.button() == Qt.LeftButton and self.dragging:
            self.dragging = False
            self.hide_timer.start(3000)

    def _update_position(self, mouse_pos):
        """更新播放位置"""
        bar_y = self.height() - 30
        bar_rect = QRect(20, bar_y - 3, self.width() - 40, 6)
        if bar_rect.contains(mouse_pos):
            rel_x = mouse_pos.x() - 20
            bar_width = self.width() - 40
            new_pos = max(0, min(1, rel_x / bar_width))
            if self.parent() and hasattr(self.parent(), 'seek_video'):
                self.parent().seek_video(new_pos)

    def enterEvent(self, event):
        self.show_progress()

    def leaveEvent(self, event):
        if not self.dragging:
            self.hide_timer.start(3000)

    def keyPressEvent(self, event: QKeyEvent):
        if self.parent() and hasattr(self.parent(), 'keyPressEvent'):
            self.parent().keyPressEvent(event)


class VideoPlayerWidget(QWidget):
    """播放器部件"""

    def __init__(self, parent=None):
        super().__init__(parent)
        self.vlc_player = None
        self.video_widget = None
        self.setup_ui()
        self.update_timer = QTimer()
        self.update_timer.timeout.connect(self._update_display)
        self.update_timer.start(41)  # 约24fps

    def setup_ui(self):
        """设置UI"""
        layout = QVBoxLayout()
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(0)
        self.setLayout(layout)

        self.video_widget = VideoWidget(self)
        layout.addWidget(self.video_widget)
        self.setStyleSheet("QWidget { background-color: #000000; }")
        self.setFocusPolicy(Qt.StrongFocus)

    def play_video(self, video_url):
        """播放视频"""
        if not self.vlc_player:
            self.vlc_player = VlcPlayer()

        if self.vlc_player:
            success = self.vlc_player.play(video_url)
            if success:
                print(f"开始播放: {video_url}")
            return success
        return False

    def _update_display(self):
        """更新显示"""
        if self.vlc_player and self.vlc_player.player:
            # 获取帧数据
            frame_data = self.vlc_player.get_frame_data()
            if frame_data:
                self.video_widget.update_frame(frame_data)

            # 更新进度
            if self.vlc_player.is_playing():
                position = self.vlc_player.get_position()
                self.video_widget.set_progress(position)

    def seek_video(self, position):
        """跳转到指定位置"""
        if self.vlc_player:
            self.vlc_player.set_position(position)
            self.video_widget.show_progress()

    def keyPressEvent(self, event):
        """键盘控制"""
        if not self.vlc_player:
            return

        key = event.key()
        if key == Qt.Key_Space:
            if self.vlc_player.is_playing():
                self.vlc_player.player.pause()
            else:
                self.vlc_player.player.play()
        elif key == Qt.Key_Escape:
            if self.isFullScreen():
                self.showNormal()
        elif key == Qt.Key_F:
            if not self.isFullScreen():
                self.showFullScreen()
            else:
                self.showNormal()
        elif key == Qt.Key_Left:
            current_pos = self.vlc_player.get_position()
            self.vlc_player.set_position(max(0, current_pos - 0.01))
        elif key == Qt.Key_Right:
            current_pos = self.vlc_player.get_position()
            self.vlc_player.set_position(min(1, current_pos + 0.01))

    def mouseDoubleClickEvent(self, event):
        """双击全屏"""
        if not self.isFullScreen():
            self.showFullScreen()
        else:
            self.showNormal()

    def closeEvent(self, event):
        """关闭事件"""
        if self.vlc_player:
            self.vlc_player.cleanup()
        if self.update_timer.isActive():
            self.update_timer.stop()
        super().closeEvent(event)


def setup_video_player(player_widget):
    """设置视频播放器"""
    video_container = player_widget.ui.video_container
    # 清空原有布局
    layout = video_container.layout()
    if layout:
        while layout.count():
            item = layout.takeAt(0)
            if item.widget():
                item.widget().deleteLater()
    # 创建视频播放器部件
    video_player = VideoPlayerWidget(video_container)
    layout.addWidget(video_player)
    return video_player


def play_video_in_player(player_widget, video_url):
    """在播放器中播放视频"""
    video_container = player_widget.ui.video_container
    layout = video_container.layout()
    video_player = None
    for i in range(layout.count()):
        widget = layout.itemAt(i).widget()
        if isinstance(widget, VideoPlayerWidget):
            video_player = widget
            break
    if not video_player:
        video_player = VideoPlayerWidget(video_container)
        layout.addWidget(video_player)
    return video_player.play_video(video_url)