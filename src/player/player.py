import os
vlc_dir = os.path.dirname(r"D:\Python\Project\ACGN_HYM\VLC\libvlc.dll")
os.environ["PATH"] = vlc_dir + os.pathsep + os.environ["PATH"]
import vlc
import ctypes
from PySide6.QtCore import Qt, QTimer, Signal, QSize
from PySide6.QtGui import QPainter, QImage, QKeyEvent, QIcon
from PySide6.QtWidgets import QWidget, QVBoxLayout, QPushButton
from src.player.player_control import ControlOverlay


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

    def stop(self):
        """停止播放"""
        if self.player:
            self.player.stop()

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

    def get_time(self):
        """获取当前时间(毫秒)"""
        return self.player.get_time() if self.player else 0

    def get_length(self):
        """获取视频总长度(毫秒)"""
        return self.player.get_length() if self.player else 0


class VideoWidget(QWidget):
    """视频显示部件"""
    seek_requested = Signal(float)

    def __init__(self, parent=None):
        super().__init__(parent)
        self.video_width = 1920
        self.video_height = 1080
        self.current_qimage = None
        self.control_overlay = ControlOverlay(self)
        self.seek_requested.connect(self.control_overlay.set_progress)
        self.control_overlay.volume_changed.connect(self._on_volume_changed)
        self.setMouseTracking(True)
        self.setFocusPolicy(Qt.StrongFocus)

    def resizeEvent(self, event):
        """重设大小时调整控制面板"""
        super().resizeEvent(event)
        self.control_overlay.setGeometry(self.rect())

    def update_frame(self, buffer):
        """更新视频帧数据"""
        if buffer:
            if self.current_qimage is None:
                self.current_qimage = QImage(buffer, self.video_width, self.video_height, self.video_width * 4, QImage.Format_ARGB32)
            self.update()

    def paintEvent(self, event):
        """绘制视频帧"""
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        painter.setRenderHint(QPainter.SmoothPixmapTransform)
        if self.current_qimage:
            scaled = self.current_qimage.scaled(self.size(), Qt.KeepAspectRatio, Qt.SmoothTransformation)
            x = (self.width() - scaled.width()) // 2
            y = (self.height() - scaled.height()) // 2
            painter.drawImage(x, y, scaled)

    def _on_volume_changed(self, volume):
        """处理音量变化"""
        if self.parent() and hasattr(self.parent(), '_adjust_volume_from_ui'):
            self.parent()._adjust_volume_from_ui(volume)

    def mouseMoveEvent(self, event):
        """鼠标移动时显示控制面板"""
        self.control_overlay.show_progress()
        super().mouseMoveEvent(event)

    def enterEvent(self, event):
        """鼠标进入时显示控制面板"""
        self.control_overlay.show_progress()
        super().enterEvent(event)

    def leaveEvent(self, event):
        """鼠标离开时隐藏控制面板"""
        if not self.control_overlay.dragging:
            self.control_overlay.hide_timer.start(3000)
        super().leaveEvent(event)

    def keyPressEvent(self, event: QKeyEvent):
        """键盘事件传递给父组件"""
        if self.parent() and hasattr(self.parent(), 'keyPressEvent'):
            self.parent().keyPressEvent(event)


class VideoPlayerWidget(QWidget):
    """播放器部件"""
    back_requested = Signal()

    def __init__(self, parent=None):
        super().__init__(parent)
        self.vlc_player = None
        self.video_widget = None
        self.setup_ui()
        self.right_key_timer = QTimer()
        self.right_key_timer.timeout.connect(self._on_right_key_held)
        self.right_key_pressed = False
        self.normal_play_rate = 1.0
        self.is_fast_forward = False
        self.update_timer = QTimer()
        self.update_timer.timeout.connect(self._update_display)
        self.update_timer.start(41)
        self.setFocus()
        self.original_parent = None
        self.original_layout = None
        self.is_fullscreen_mode = False

    def setup_ui(self):
        """设置UI"""
        layout = QVBoxLayout()
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(0)
        self.setLayout(layout)
        self.video_widget = VideoWidget(self)
        self.video_widget.seek_requested.connect(self.seek_video)
        self.video_widget.control_overlay.play_pause_requested.connect(self.toggle_play_pause)
        self.video_widget.control_overlay.fullscreen_requested.connect(self._toggle_fullscreen)
        self.video_widget.control_overlay.back_requested.connect(self._on_back_button_clicked)
        layout.addWidget(self.video_widget)
        self.setStyleSheet("QWidget { background-color: #000000; }")
        self.setFocusPolicy(Qt.StrongFocus)

    def _on_back_button_clicked(self):
        """返回按钮点击事件"""
        self.stop_video()
        if self.is_fullscreen_mode:
            self._exit_fullscreen()
        self.back_requested.emit()

    def stop_video(self):
        """停止播放视频"""
        if self.vlc_player:
            self.vlc_player.stop()
            self.video_widget.control_overlay.set_play_state(False)
            self.video_widget.control_overlay.set_progress(0)
            self.video_widget.control_overlay.set_time(0, 0)
            self.video_widget.current_qimage = None
            self.video_widget.update()

    def play_video(self, video_url):
        """播放视频"""
        if not self.vlc_player:
            self.vlc_player = VlcPlayer()
        if self.vlc_player:
            success = self.vlc_player.play(video_url)
            if success:
                print(f"开始播放: {video_url}")
                self.setFocus()
            return success
        return False

    def _update_display(self):
        """更新显示"""
        if self.vlc_player and self.vlc_player.player:
            frame_data = self.vlc_player.get_frame_data()
            if frame_data:
                self.video_widget.update_frame(frame_data)
            if self.vlc_player.is_playing():
                position = self.vlc_player.get_position()
                current_time = self.vlc_player.get_time() / 1000
                total_time = self.vlc_player.get_length() / 1000
                self.video_widget.control_overlay.set_progress(position)
                self.video_widget.control_overlay.set_time(current_time, total_time)
                self.video_widget.control_overlay.set_play_state(True)
            else:
                self.video_widget.control_overlay.set_play_state(False)

    def toggle_play_pause(self):
        """切换播放/暂停状态"""
        if self.vlc_player and self.vlc_player.player:
            if self.vlc_player.is_playing():
                self.vlc_player.player.pause()
                self.video_widget.control_overlay.set_play_state(False)
            else:
                self.vlc_player.player.play()
                self.video_widget.control_overlay.set_play_state(True)
            self.video_widget.control_overlay.show_progress()

    def seek_video(self, position):
        """跳转到指定位置"""
        if self.vlc_player:
            self.vlc_player.set_position(position)
            self.video_widget.control_overlay.show_progress()

    def _adjust_volume_from_ui(self, volume):
        """UI调整音量"""
        if self.vlc_player and self.vlc_player.player:
            new_volume = int(volume * 100)
            self.vlc_player.player.audio_set_volume(new_volume)
            print(f"音量: {new_volume}%")

    def keyPressEvent(self, event):
        """键盘控制"""
        key = event.key()
        # 空格键 - 播放/暂停
        if key == Qt.Key_Space or key == Qt.Key_K:
            self.toggle_play_pause()
        # ESC键 - 退出全屏
        elif key == Qt.Key_Escape:
            self._exit_fullscreen()
        # F键 - 切换全屏
        elif key == Qt.Key_F:
            self._toggle_fullscreen()
        # 左方向键 - 后退5秒
        elif key == Qt.Key_Left:
            self._jump_backward(5)
        # 右方向键 - 短按前进5秒,长按3倍速播放
        elif key == Qt.Key_Right:
            if not event.isAutoRepeat():
                self.right_key_pressed = True
                self.right_key_timer.start(500)
                self._jump_forward(5)
        # 上方向键 - 音量增加
        elif key == Qt.Key_Up:
            self._adjust_volume(5)
        # 下方向键 - 音量减少
        elif key == Qt.Key_Down:
            self._adjust_volume(-5)

    def _toggle_fullscreen(self):
        """全屏"""
        if not self.is_fullscreen_mode:
            self._enter_fullscreen()
        else:
            self._exit_fullscreen()

    def _enter_fullscreen(self):
        """全屏模式"""
        if not self.is_fullscreen_mode:
            self.is_fullscreen_mode = True
            self.original_parent = self.parent()
            self.original_layout = self.parent().layout() if self.parent() and self.parent().layout() else None
            self.setParent(None)
            self.showFullScreen()
            self.setFocus()

    def _exit_fullscreen(self):
        """退出全屏"""
        if self.is_fullscreen_mode:
            self.is_fullscreen_mode = False
            self.showNormal()
            if self.original_parent and self.original_layout:
                self.setParent(self.original_parent)
                self.original_layout.addWidget(self)
            self.setFocus()

    def keyReleaseEvent(self, event):
        """键盘释放事件"""
        if not self.vlc_player:
            return
        key = event.key()
        if key == Qt.Key_Right:
            if not event.isAutoRepeat():
                self.right_key_pressed = False
                self.right_key_timer.stop()
                if self.is_fast_forward:
                    self._set_playback_rate(self.normal_play_rate)
                    self.is_fast_forward = False

    def _on_right_key_held(self):
        """右方向键长按处理"""
        if self.right_key_pressed and self.vlc_player and self.vlc_player.player:
            if not self.is_fast_forward:
                current_rate = self.vlc_player.player.get_rate()
                if current_rate != 3.0:
                    self.normal_play_rate = current_rate
                    self._set_playback_rate(3.0)
                    self.is_fast_forward = True
            self._jump_forward(1)

    def _jump_backward(self, seconds):
        """后退指定秒数"""
        if self.vlc_player and self.vlc_player.player:
            current_time = self.vlc_player.player.get_time()
            if current_time > 0:
                new_time = max(0, current_time - seconds * 1000)
                self.vlc_player.player.set_time(new_time)
                self.video_widget.control_overlay.show_progress()

    def _jump_forward(self, seconds):
        """前进指定秒数"""
        if self.vlc_player and self.vlc_player.player:
            current_time = self.vlc_player.player.get_time()
            new_time = current_time + seconds * 1000
            self.vlc_player.player.set_time(new_time)
            self.video_widget.control_overlay.show_progress()

    def _adjust_volume(self, delta):
        """调整音量"""
        if self.vlc_player and self.vlc_player.player:
            current_volume = self.vlc_player.player.audio_get_volume()
            new_volume = min(100, max(0, current_volume + delta))
            self.vlc_player.player.audio_set_volume(new_volume)
            print(f"音量: {new_volume}%")
            # 更新UI
            self.video_widget.control_overlay.set_volume(new_volume)
            self.video_widget.control_overlay.show_progress()

    def _set_playback_rate(self, rate):
        """设置播放速度"""
        if self.vlc_player and self.vlc_player.player:
            self.vlc_player.player.set_rate(rate)

    def mouseDoubleClickEvent(self, event):
        """双击全屏"""
        event.accept()
        self._toggle_fullscreen()

    def closeEvent(self, event):
        """关闭事件"""
        if self.isFullScreen():
            self._exit_fullscreen()
        self.stop_video()
        if self.vlc_player:
            self.vlc_player.cleanup()
        if self.update_timer.isActive():
            self.update_timer.stop()
        if self.right_key_timer.isActive():
            self.right_key_timer.stop()
        super().closeEvent(event)


def setup_video_player(player_widget):
    """设置视频播放器"""
    video_container = player_widget.ui.video_container
    layout = video_container.layout()
    if layout:
        while layout.count():
            item = layout.takeAt(0)
            if item.widget():
                item.widget().deleteLater()
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