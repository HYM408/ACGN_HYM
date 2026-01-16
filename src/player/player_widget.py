from typing import Optional
from PySide6.QtCore import Qt, QTimer, Signal
from PySide6.QtWidgets import QWidget, QVBoxLayout
from src.player.player_control import ControlOverlay
from src.player.player_core import VideoDisplayWidget, VlcPlayer


class VideoPlayerWidget(QWidget):
    """播放器主控件"""
    back_requested = Signal()

    def __init__(self, parent=None):
        super().__init__(parent)
        self._init_state()
        self._init_ui()
        self._setup_connections()
        self._setup_timers()
        self.setFocus()

    def _init_state(self):
        """初始化状态"""
        self.vlc_player: Optional[VlcPlayer] = None
        self.is_fullscreen_mode = False
        self.normal_play_rate = 1.0
        self.is_fast_forward = False
        self.right_key_pressed = False
        # 全屏恢复
        self.original_parent = None
        self.original_layout = None

    def _init_ui(self):
        """初始化UI"""
        layout = QVBoxLayout()
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(0)
        self.setLayout(layout)
        self.video_display = VideoDisplayWidget(self)
        layout.addWidget(self.video_display)
        self.control_overlay = ControlOverlay(self.video_display)
        self.video_display.resizeEvent = self._resize_control_overlay
        self.setStyleSheet("QWidget { background-color: #000000; }")
        self.setFocusPolicy(Qt.FocusPolicy.StrongFocus)

    def _resize_control_overlay(self, event):
        """调整控制面板大小"""
        super(VideoDisplayWidget, self.video_display).resizeEvent(event)
        self.control_overlay.setGeometry(self.video_display.rect())

    def _setup_connections(self):
        """设置信号连接"""
        # 控制面板
        self.control_overlay.play_pause_requested.connect(self.toggle_play_pause)
        self.control_overlay.fullscreen_requested.connect(self._toggle_fullscreen)
        self.control_overlay.back_requested.connect(self._on_back_button_clicked)
        self.control_overlay.volume_changed.connect(self._adjust_volume)
        self.control_overlay.seek_requested.connect(self.seek_video)

    def _setup_timers(self):
        """设置定时器"""
        self.update_timer = QTimer()
        self.update_timer.timeout.connect(self._update_video_display)
        self.update_timer.start(41)
        self.right_key_timer = QTimer()
        self.right_key_timer.timeout.connect(self._on_right_key_held)

    def play_video(self, video_url: str) -> bool:
        """播放视频"""
        if not self.vlc_player:
            self.vlc_player = VlcPlayer()
        success = self.vlc_player.play(video_url)
        if success:
            self.setFocus()
            self.control_overlay.set_play_state(True)
        return success

    def stop_video(self):
        """停止播放"""
        if self.vlc_player:
            self.vlc_player.stop()
            self.control_overlay.set_play_state(False)
            self.control_overlay.set_progress(0)
            self.control_overlay.set_time(0, 0)
            self.video_display.current_qimage = None
            self.video_display.update()

    def toggle_play_pause(self):
        """切换播放/暂停"""
        if self.vlc_player:
            self.vlc_player.toggle_play_pause()
            self.control_overlay.set_play_state(self.vlc_player.is_playing())
            self.control_overlay.show_controls()

    def seek_video(self, position: float):
        """跳转到指定位置"""
        if self.vlc_player:
            self.vlc_player.set_position(position)
            self.control_overlay.show_controls()

    def _adjust_volume(self, volume: int):
        """调整音量"""
        if self.vlc_player:
            self.vlc_player.set_volume(volume)

    def _toggle_fullscreen(self):
        """切换全屏状态"""
        if not self.is_fullscreen_mode:
            self._enter_fullscreen()
        else:
            self._exit_fullscreen()

    def _enter_fullscreen(self):
        """进入全屏模式"""
        if self.is_fullscreen_mode:
            return
        self.is_fullscreen_mode = True
        self.original_parent = self.parent()
        self.original_layout = self.parent().layout() if self.parent() and self.parent().layout() else None
        self.setParent(None)
        self.showFullScreen()
        self.setFocus()

    def _exit_fullscreen(self):
        """退出全屏模式"""
        if not self.is_fullscreen_mode:
            return
        self.is_fullscreen_mode = False
        self.showNormal()
        if self.original_parent and self.original_layout:
            self.setParent(self.original_parent)
            self.original_layout.addWidget(self)
        self.setFocus()

    def _on_back_button_clicked(self):
        """处理返回按钮点击"""
        self.stop_video()
        if self.is_fullscreen_mode:
            self._exit_fullscreen()
        self.back_requested.emit()

    def keyPressEvent(self, event):
        """键盘控制"""
        key = event.key()
        # 空格键 - 播放/暂停
        if key == Qt.Key.Key_Space:
            self.toggle_play_pause()
        # ESC键 - 退出全屏
        elif key == Qt.Key.Key_Escape:
            self._exit_fullscreen()
        # F键 - 切换全屏
        elif key == Qt.Key.Key_F:
            self._toggle_fullscreen()
        # 左方向键 - 后退5秒
        elif key == Qt.Key.Key_Left:
            self._jump_backward(5)
        # 右方向键 - 前进5秒或快进
        elif key == Qt.Key.Key_Right:
            if not event.isAutoRepeat():
                self.right_key_pressed = True
                self.right_key_timer.start(500)
                self._jump_forward(5)
        # 上方向键 - 音量增加
        elif key == Qt.Key.Key_Up:
            self._adjust_volume_delta(5)
        # 下方向键 - 音量减少
        elif key == Qt.Key.Key_Down:
            self._adjust_volume_delta(-5)

    def keyReleaseEvent(self, event):
        """键盘释放事件"""
        if not self.vlc_player:
            return
        key = event.key()
        if key == Qt.Key.Key_Right:
            if not event.isAutoRepeat():
                self.right_key_pressed = False
                self.right_key_timer.stop()
                if self.is_fast_forward:
                    self._set_playback_rate(self.normal_play_rate)
                    self.is_fast_forward = False

    def _on_right_key_held(self):
        """右方向键长按处理"""
        if self.right_key_pressed and self.vlc_player:
            if not self.is_fast_forward:
                current_rate = self.normal_play_rate
                if current_rate != 3.0:
                    self.normal_play_rate = current_rate
                    self._set_playback_rate(3.0)
                    self.is_fast_forward = True
            self._jump_forward(1)

    def _jump_backward(self, seconds: int):
        """后退指定秒数"""
        if self.vlc_player:
            current_time, total_time = self.vlc_player.get_time_info()
            if current_time > 0 and total_time > 0:
                new_time = max(0, current_time - seconds * 1000)
                new_position = new_time / total_time
                self.vlc_player.set_position(new_position)
                self.control_overlay.show_controls()

    def _jump_forward(self, seconds: int):
        """前进指定秒数"""
        if self.vlc_player:
            current_time, total_time = self.vlc_player.get_time_info()
            if total_time > 0:
                new_time = current_time + seconds * 1000
                new_time = min(new_time, total_time)
                new_position = new_time / total_time
                self.vlc_player.set_position(new_position)
                self.control_overlay.show_controls()

    def _adjust_volume_delta(self, delta: int):
        """调整音量增量"""
        if self.vlc_player:
            current_volume = self.vlc_player.get_volume()
            new_volume = min(100, max(0, current_volume + delta))
            self.vlc_player.set_volume(new_volume)
            self.control_overlay.set_volume(new_volume)
            self.control_overlay.show_controls()

    def _set_playback_rate(self, rate: float):
        """设置播放速度"""
        if self.vlc_player:
            self.vlc_player.set_playback_rate(rate)

    def _update_video_display(self):
        """更新视频显示"""
        if not self.vlc_player:
            return
        frame_data = self.vlc_player.renderer.get_frame()
        if frame_data:
            self.video_display.update_frame(frame_data)
        if self.vlc_player.is_playing():
            position = self.vlc_player.get_position()
            current_time, total_time = self.vlc_player.get_time_info()
            self.control_overlay.set_progress(position)
            self.control_overlay.set_time(current_time / 1000, total_time / 1000)
            self.control_overlay.set_play_state(True)
        else:
            self.control_overlay.set_play_state(False)

    def closeEvent(self, event):
        """关闭事件"""
        if self.is_fullscreen_mode:
            self._exit_fullscreen()
        self.stop_video()
        if self.vlc_player:
            self.vlc_player.cleanup()
        if self.update_timer.isActive():
            self.update_timer.stop()
        if self.right_key_timer.isActive():
            self.right_key_timer.stop()
        super().closeEvent(event)