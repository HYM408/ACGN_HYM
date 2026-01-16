import os
vlc_dir = os.path.dirname(r"D:\Python\Project\ACGN_HYM\VLC\libvlc.dll")
os.environ["PATH"] = vlc_dir + os.pathsep + os.environ["PATH"]
import vlc
import ctypes
from typing import Optional, Tuple
from PySide6.QtCore import Qt, Signal
from PySide6.QtWidgets import QWidget
from PySide6.QtGui import QPainter, QImage


class VideoFrameRenderer:
    """VLC回调和解耦渲染逻辑"""

    def __init__(self, width: int = 1920, height: int = 1080):
        self.width = width
        self.height = height
        self.stride = width * 4
        self.buffer = None
        self.frame_ready = False
        self.current_frame = None
        self._callbacks = {}

    def setup_callbacks(self, player):
        """设置VLC回调函数"""
        VideoLockCb = ctypes.CFUNCTYPE(ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p))
        VideoUnlockCb = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p))

        def lock_cb(_, planes):
            if not self.buffer:
                self.buffer = (ctypes.c_uint8 * (self.width * self.height * 4))()
                self.current_frame = None
            planes[0] = ctypes.addressof(self.buffer)
            return None

        def unlock_cb(_, __, _planes):
            self.frame_ready = True

        self._callbacks['lock'] = VideoLockCb(lock_cb)
        self._callbacks['unlock'] = VideoUnlockCb(unlock_cb)
        player.video_set_callbacks(self._callbacks['lock'], self._callbacks['unlock'], None, None)
        player.video_set_format("RV32", self.width, self.height, self.stride)

    def get_frame(self) -> Optional[memoryview]:
        """获取当前帧数据"""
        if not self.frame_ready:
            return None
        self.frame_ready = False
        if self.current_frame is None and self.buffer:
            self.current_frame = memoryview(self.buffer)
        return self.current_frame

    def cleanup(self):
        """清理资源"""
        self.buffer = None
        self.current_frame = None
        self.frame_ready = False
        self._callbacks.clear()


class VlcPlayer:
    """VLC播放器"""

    def __init__(self, vlc_args: list = None):
        if vlc_args is None:
            vlc_args = ["--no-xlib", "--quiet", "--avcodec-fast", "--avcodec-threads=1"]
        self.instance = vlc.Instance(vlc_args)
        self.player = self.instance.media_player_new()
        self.renderer = VideoFrameRenderer()
        self.renderer.setup_callbacks(self.player)

    def play(self, video_url: str) -> bool:
        """播放指定URL的视频"""
        if not self.player:
            return False
        try:
            media = self.instance.media_new(video_url)
            media.add_option(":avcodec-hw=dxva2")
            self.player.set_media(media)
            self.player.play()
            return True
        except Exception as e:
            print(f"播放失败: {e}")
            return False

    def set_position(self, position: float):
        """设置播放位置 (0.0-1.0)"""
        if self.player:
            self.player.set_position(max(0.0, min(1.0, position)))

    def get_position(self) -> float:
        """获取当前播放位置"""
        return self.player.get_position() if self.player else 0.0

    def get_time_info(self) -> Tuple[int, int]:
        """获取当前时间和总时间（毫秒）"""
        if self.player:
            return self.player.get_time(), self.player.get_length()
        return 0, 0

    def set_volume(self, volume: int):
        """设置音量 (0-100)"""
        if self.player:
            self.player.audio_set_volume(max(0, min(100, volume)))

    def get_volume(self) -> int:
        """获取当前音量"""
        return self.player.audio_get_volume() if self.player else 50

    def set_playback_rate(self, rate: float):
        """设置播放速度"""
        if self.player:
            self.player.set_rate(rate)

    def is_playing(self) -> bool:
        """检查是否正在播放"""
        return self.player.is_playing() if self.player else False

    def toggle_play_pause(self):
        """切换播放/暂停状态"""
        if self.player:
            if self.is_playing():
                self.player.pause()
            else:
                self.player.play()

    def stop(self):
        """停止播放"""
        if self.player:
            self.player.stop()

    def cleanup(self):
        """清理资源"""
        self.stop()
        self.renderer.cleanup()
        if self.player:
            self.player.release()
        if self.instance:
            self.instance.release()


class VideoDisplayWidget(QWidget):
    """视频画面渲染"""
    seek_requested = Signal(float)

    def __init__(self, parent=None):
        super().__init__(parent)
        self.video_width = 1920
        self.video_height = 1080
        self.current_qimage = None
        self.setMouseTracking(True)
        self.setFocusPolicy(Qt.FocusPolicy.StrongFocus)

    def update_frame(self, buffer):
        """更新视频帧"""
        if buffer:
            if self.current_qimage is None:
                self.current_qimage = QImage(buffer, self.video_width, self.video_height, self.video_width * 4, QImage.Format.Format_ARGB32)
            self.update()

    def paintEvent(self, event):
        """绘制视频帧"""
        if not self.current_qimage:
            return
        painter = QPainter(self)
        painter.setRenderHint(QPainter.RenderHint.Antialiasing)
        painter.setRenderHint(QPainter.RenderHint.SmoothPixmapTransform)
        scaled = self.current_qimage.scaled(self.size(),Qt.AspectRatioMode.KeepAspectRatio,Qt.TransformationMode.SmoothTransformation)
        x = (self.width() - scaled.width()) // 2
        y = (self.height() - scaled.height()) // 2
        painter.drawImage(x, y, scaled)