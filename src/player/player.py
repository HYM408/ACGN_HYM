from PySide6.QtGui import QPixmap
from PySide6.QtCore import Qt, QTimer, Signal, QPoint, QSize
from PySide6.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QPushButton, QLabel, QSlider, QMenu
from src.player.player_core import VideoDisplayWidget, VlcPlayer
from src.thread_manager import thread_manager


class ClickableSlider(QSlider):
    """自定义进度条"""
    clicked = Signal(int)
    hover_time = Signal(float, int)

    def __init__(self, orientation=Qt.Orientation.Horizontal, threshold_percent=2):
        super().__init__(orientation)
        self.setMouseTracking(True)
        self.threshold_percent = threshold_percent

    def _calculate_value_from_pos(self, pos: QPoint) -> float:
        """根据鼠标位置计算值"""
        available_width = max(1, (self.width() - 20))
        value = (pos.x() - 10) / available_width * self.maximum()
        return max(0.0, min(float(self.maximum()), value))

    def mousePressEvent(self, event):
        """鼠标点击事件"""
        if event.button() == Qt.MouseButton.LeftButton:
            pos = event.position().toPoint()
            value = self._calculate_value_from_pos(pos)
            current_value = self.value()
            threshold = self.maximum() * self.threshold_percent / 100
            if abs(value - current_value) <= threshold:
                super().mousePressEvent(event)
            else:
                int_value = int(value)
                self.setValue(int_value)
                self.clicked.emit(int_value)
                self.sliderPressed.emit()
                self.sliderReleased.emit()

    def mouseMoveEvent(self, event):
        """鼠标移动事件"""
        super().mouseMoveEvent(event)
        pos = event.position().toPoint()
        value = self._calculate_value_from_pos(pos)
        self.hover_time.emit(value / self.maximum(), pos.x())

    def leaveEvent(self, event):
        """鼠标离开事件"""
        super().leaveEvent(event)
        self.hover_time.emit(-1, -1)


class ControlOverlay(QWidget):
    """视频控制面板"""
    play_pause_requested = Signal()
    fullscreen_requested = Signal()
    back_requested = Signal()
    volume_changed = Signal(int)
    seek_requested = Signal(float)
    playback_rate_changed = Signal(float)

    def __init__(self, parent=None):
        super().__init__(parent)
        self._init_state()
        self._init_ui()
        self._setup_timers()

    def _init_state(self):
        """初始化状态"""
        self.is_visible = True
        self.dragging_progress = False
        self.dragging_volume = False
        self.is_playing = False
        self.current_volume = 50
        self.current_time = 0
        self.total_time = 0
        self.last_click_time = 0
        self.progress_max_value = 10000
        self.playback_rates = [3.0, 2.0, 1.5, 1.25, 1.0, 0.75, 0.5]
        self.current_playback_rate_index = 4

    def _init_ui(self):
        """初始化UI组件"""
        self.setAttribute(Qt.WidgetAttribute.WA_TransparentForMouseEvents, False)
        # 主布局
        main_layout = QVBoxLayout(self)
        main_layout.setContentsMargins(0, 0, 0, 0)
        main_layout.setSpacing(0)
        # 顶部区域
        self.top_widget = QWidget()
        self.top_widget.setFixedHeight(60)
        self.top_widget.setStyleSheet("background-color: transparent")
        top_layout = QHBoxLayout(self.top_widget)
        top_layout.setContentsMargins(10, 10, 10, 10)
        # 返回按钮
        self.back_button = self._create_control_button("icons/back2.png", 20, self.back_requested.emit)
        top_layout.addWidget(self.back_button)
        top_layout.addStretch()
        main_layout.addWidget(self.top_widget)
        main_layout.addStretch()
        # 底部控制面板
        self.bottom_widget = QWidget()
        self.bottom_widget.setFixedHeight(80)
        self.bottom_widget.setStyleSheet("background-color: rgba(0, 0, 0, 100); border: none")
        bottom_layout = QVBoxLayout(self.bottom_widget)
        bottom_layout.setContentsMargins(20, 0, 20, 10)
        # 进度条区域
        self._setup_progress_area(bottom_layout)
        # 控制按钮区域
        self._setup_control_buttons(bottom_layout)
        main_layout.addWidget(self.bottom_widget)
        self.setMouseTracking(True)

    @staticmethod
    def _create_control_button(icon_path: str, icon_size: int, callback) -> QPushButton:
        """创建控制按钮"""
        button = QPushButton()
        button.setFixedSize(28, 28)
        button.setStyleSheet("QPushButton {border: none; border-radius: 14px; background-color: transparent}")
        button.setIcon(QPixmap(icon_path))
        button.setIconSize(QSize(icon_size, icon_size))
        button.clicked.connect(callback)
        return button

    def _setup_progress_area(self, parent_layout):
        """设置进度条区域"""
        progress_layout = QHBoxLayout()
        self.time_label = QLabel("00:00 / 00:00")
        self.time_label.setStyleSheet("color: rgba(255, 255, 255, 220); font-size: 11px; background-color: transparent")
        progress_layout.addWidget(self.time_label)
        # 时间提示标签
        self.time_tooltip = QLabel(self)
        self.time_tooltip.setStyleSheet("background-color: rgba(0, 0, 0, 100);color: white; padding: 4px 8px; font-size: 12px")
        self.time_tooltip.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.time_tooltip.hide()
        # 进度条
        self.progress_slider = ClickableSlider(Qt.Orientation.Horizontal, threshold_percent=2)
        self.progress_slider.setFixedHeight(30)
        self.progress_slider.setRange(0, self.progress_max_value)
        self.progress_slider.setStyleSheet("QSlider {background-color: transparent}"
                                           "QSlider::groove:horizontal {height: 8px; background: rgba(100, 100, 100, 150); border-radius: 4px}"
                                           "QSlider::sub-page:horizontal {height: 8px; background: #dc3232; border-radius: 4px}"
                                           "QSlider::handle:horizontal {background: rgba(255, 255, 255, 220); width: 16px; height: 16px; margin: -4px 0; border-radius: 8px}")
        self.progress_slider.sliderPressed.connect(self._on_progress_pressed)
        self.progress_slider.sliderReleased.connect(self._on_progress_released)
        self.progress_slider.valueChanged.connect(self._on_progress_changed)
        self.progress_slider.clicked.connect(self._on_progress_clicked)
        self.progress_slider.hover_time.connect(self._on_hover_time)
        progress_layout.addWidget(self.progress_slider, 1)
        parent_layout.addLayout(progress_layout)

    def _setup_control_buttons(self, parent_layout):
        """设置控制按钮区域"""
        controls_layout = QHBoxLayout()
        controls_layout.setSpacing(10)
        # 播放/暂停按钮
        self.play_button = self._create_control_button("", 25, self.play_pause_requested.emit)
        self._update_play_button_icon()
        controls_layout.addWidget(self.play_button)
        # 音量区域
        controls_layout.addLayout(self._create_volume_controls())
        controls_layout.addStretch()
        # 播放速度按钮
        self.playback_rate_button = self._create_playback_rate_button()
        controls_layout.addWidget(self.playback_rate_button)
        # 全屏按钮
        self.fullscreen_button = self._create_control_button("icons/full.png", 20, self.fullscreen_requested.emit)
        controls_layout.addWidget(self.fullscreen_button)
        parent_layout.addLayout(controls_layout)

    def _create_playback_rate_button(self) -> QPushButton:
        """创建播放速度选择按钮"""
        button = QPushButton()
        button.setFixedSize(40, 28)
        button.setStyleSheet("QPushButton {border: 1px solid rgba(255, 255, 255, 150); border-radius: 4px; background-color: rgba(0, 0, 0, 120); color: rgba(255, 255, 255, 220); font-size: 12px; padding: 2px}"
                             "QPushButton:hover {background-color: rgba(50, 50, 50, 150)}")
        self._update_playback_rate_button_text(button)
        button.clicked.connect(self._show_playback_rate_menu)
        return button

    def _update_playback_rate_button_text(self, button):
        """更新播放速度按钮文本"""
        rate = self.playback_rates[self.current_playback_rate_index]
        rate_str = str(rate).rstrip('0').rstrip('.')
        button.setText(f"{rate_str}x")

    def _show_playback_rate_menu(self):
        """显示播放速度选择菜单"""
        menu = QMenu(self)
        menu.setStyleSheet("QMenu {background-color: rgba(40, 40, 40, 240); border: 1px solid rgba(100, 100, 100, 200)}"
                           "QMenu::item {color: white; padding: 6px 16px; font-size: 12px}"
                           "QMenu::item:selected {background-color: rgba(80, 80, 80, 255)}")
        for i, rate in enumerate(self.playback_rates):
            rate_str = str(rate).rstrip('0').rstrip('.')
            action = menu.addAction(f"{rate_str}x")
            action.setCheckable(True)
            action.setChecked(i == self.current_playback_rate_index)
            action.triggered.connect(lambda checked, idx=i: self._on_playback_rate_selected(idx))
        button_rect = self.playback_rate_button.rect()
        button_pos = self.playback_rate_button.mapToGlobal(button_rect.topLeft())
        menu_size = menu.sizeHint()
        x = button_pos.x() + (button_rect.width() - menu_size.width()) // 2
        y = button_pos.y() - menu_size.height()
        menu.exec(QPoint(x, y))

    def _on_playback_rate_selected(self, index):
        """播放速度选项被选中"""
        if index != self.current_playback_rate_index:
            self.current_playback_rate_index = index
            rate = self.playback_rates[index]
            self._update_playback_rate_button_text(self.playback_rate_button)
            self.playback_rate_changed.emit(rate)
            self.show_controls()

    def get_current_playback_rate(self) -> float:
        """获取当前播放速度"""
        return self.playback_rates[self.current_playback_rate_index]

    def set_playback_rate(self, rate: float):
        """设置播放速度并更新按钮显示"""
        closest_index = 0
        min_diff = abs(rate - self.playback_rates[0])
        for i, available_rate in enumerate(self.playback_rates):
            diff = abs(rate - available_rate)
            if diff < min_diff:
                min_diff = diff
                closest_index = i
        if closest_index != self.current_playback_rate_index:
            self.current_playback_rate_index = closest_index
            self._update_playback_rate_button_text(self.playback_rate_button)

    def _create_volume_controls(self) -> QHBoxLayout:
        """创建音量控制区域"""
        volume_layout = QHBoxLayout()
        volume_layout.setSpacing(8)
        volume_layout.setContentsMargins(0, 0, 0, 0)
        # 声音图标
        self.volume_icon_label = QLabel()
        self.volume_icon_label.setFixedSize(20, 20)
        self.volume_icon_label.setPixmap(QPixmap("icons/sound.png").scaled(20, 20, Qt.AspectRatioMode.KeepAspectRatio, Qt.TransformationMode.SmoothTransformation))
        self.volume_icon_label.setStyleSheet("background-color: transparent;")
        volume_layout.addWidget(self.volume_icon_label)
        # 音量控制
        self.volume_slider = ClickableSlider(Qt.Orientation.Horizontal, threshold_percent=10)
        self.volume_slider.setFixedSize(80, 20)
        self.volume_slider.setRange(0, 100)
        self.volume_slider.setValue(self.current_volume)
        self.volume_slider.setStyleSheet("QSlider {background: transparent}"
                                         "QSlider::groove:horizontal {height: 4px; background: rgba(100, 100, 100, 150); border-radius: 2px}"
                                         "QSlider::sub-page:horizontal {height: 4px; background: #dc3232; border-radius: 2px}"
                                         "QSlider::handle:horizontal {background: rgba(255, 255, 255, 220); width: 12px; height: 12px; margin: -4px 0px; border-radius: 6px}")
        self.volume_slider.valueChanged.connect(self.volume_changed.emit)
        self.volume_slider.sliderPressed.connect(self._on_volume_pressed)
        self.volume_slider.sliderReleased.connect(self._on_volume_released)
        self.volume_slider.clicked.connect(lambda v: self.volume_changed.emit(v))
        volume_layout.addWidget(self.volume_slider)
        return volume_layout

    def _setup_timers(self):
        """设置定时器"""
        self.hide_timer = QTimer()
        self.hide_timer.setSingleShot(True)
        self.hide_timer.timeout.connect(self.hide_controls)
        self.click_timer = QTimer()
        self.click_timer.setSingleShot(True)
        self.click_timer.timeout.connect(self.play_pause_requested.emit)

    def set_play_state(self, playing: bool):
        """设置播放状态"""
        if self.is_playing == playing:
            return
        self.is_playing = playing
        self._update_play_button_icon()
        self.hide_timer.stop()
        if self.is_playing:
            self.show_controls()
        else:
            self.show_controls(False)

    def set_progress(self, position: float):
        """设置进度位置 (0.0-1.0)"""
        if not self.dragging_progress:
            self.progress_slider.setValue(int(position * self.progress_max_value))

    def set_time(self, current_seconds: float, total_seconds: float):
        """设置当前时间和总时间"""
        if not self.dragging_progress:
            self.current_time = current_seconds
            self.total_time = total_seconds
            self.time_label.setText(f"{self._format_time(current_seconds)} / {self._format_time(total_seconds)}")

    def set_volume(self, volume: int):
        """设置音量 (0-100)"""
        if not self.dragging_volume:
            self.volume_slider.setValue(volume)

    def show_controls(self, start_timer=True):
        """显示控制面板"""
        if not self.is_visible:
            self.is_visible = True
            self.top_widget.show()
            self.bottom_widget.show()
        if start_timer and self.is_playing:
            self.hide_timer.start(3000)
        self.setCursor(Qt.CursorShape.ArrowCursor)

    def hide_controls(self):
        """隐藏控制面板"""
        if self.is_visible and self.is_playing and not (self.dragging_progress or self.dragging_volume):
            self.is_visible = False
            self.top_widget.hide()
            self.bottom_widget.hide()
            self.setCursor(Qt.CursorShape.BlankCursor)

    def mouseMoveEvent(self, event):
        """鼠标移动事件"""
        super().mouseMoveEvent(event)
        pos = event.position().toPoint()
        progress_rect = self.progress_slider.geometry()
        self.hovering_progress = progress_rect.contains(pos)
        if not self._is_in_control_area(pos):
            if not self.is_visible:
                self.show_controls()
            elif self.is_playing and not (self.dragging_progress or self.dragging_volume):
                self.hide_timer.start(3000)
        else:
            if not self.hovering_progress:
                self.hide_timer.stop()
                self.show_controls(False)

    def mousePressEvent(self, event):
        """鼠标点击事件"""
        super().mousePressEvent(event)
        self.setCursor(Qt.CursorShape.ArrowCursor)
        pos = event.position().toPoint()
        if self._is_in_control_area(pos):
            self.show_controls(False)
        elif event.button() == Qt.MouseButton.LeftButton:
            current_time = event.timestamp()
            if current_time - self.last_click_time < 200:
                self.click_timer.stop()
                self.fullscreen_requested.emit()
            else:
                self.last_click_time = current_time
                self.click_timer.start(200)

    def mouseDoubleClickEvent(self, event):
        """双击事件"""
        if event.button() == Qt.MouseButton.LeftButton:
            self.click_timer.stop()
            self.fullscreen_requested.emit()

    def _is_in_control_area(self, pos: QPoint) -> bool:
        """检查点是否在控制面板区域"""
        return pos.y() <= 60 or pos.y() >= self.height() - 80

    def _update_play_button_icon(self):
        """更新播放按钮图标"""
        icon_name = "stop.png" if self.is_playing else "play.png"
        self.play_button.setIcon(QPixmap(f"icons/{icon_name}"))
        self.play_button.setIconSize(QSize(25, 25))

    @staticmethod
    def _format_time(seconds: float) -> str:
        """格式化时间显示"""
        hours = int(seconds // 3600)
        minutes = int((seconds % 3600) // 60)
        secs = int(seconds % 60)
        if hours > 0:
            return f"{hours:02d}:{minutes:02d}:{secs:02d}"
        return f"{minutes:02d}:{secs:02d}"

    def _on_hover_time(self, time_fraction: float, mouse_x: int):
        """时间提示"""
        if time_fraction >= 0 and self.total_time > 0:
            hover_seconds = time_fraction * self.total_time
            time_text = self._format_time(hover_seconds)
            self.time_tooltip.setText(time_text)
            self.time_tooltip.adjustSize()
            progress_pos = self.progress_slider.mapTo(self, QPoint(0, 0))
            tooltip_x = progress_pos.x() + mouse_x - self.time_tooltip.width() // 2
            tooltip_y = progress_pos.y() - self.time_tooltip.height()
            tooltip_x = max(10, min(self.width() - self.time_tooltip.width() - 10, tooltip_x))
            tooltip_y = max(10, tooltip_y)
            self.time_tooltip.move(tooltip_x, tooltip_y)
            self.time_tooltip.show()
            self.hide_timer.stop()
        else:
            self.time_tooltip.hide()
            if self.is_playing and not self._is_in_control_area(self.mapFromGlobal(self.cursor().pos())):
                self.hide_timer.start(3000)

    def _on_progress_clicked(self, value):
        """进度条点击"""
        position = value / self.progress_max_value
        self.seek_requested.emit(position)
        current_seconds = self.total_time * position
        self.time_label.setText(f"{self._format_time(current_seconds)} / {self._format_time(self.total_time)}")
        self.show_controls()

    def _on_progress_pressed(self):
        """进度条按下"""
        self.dragging_progress = True
        self.hide_timer.stop()
        self.setCursor(Qt.CursorShape.ArrowCursor)

    def _on_progress_released(self):
        """进度条释放"""
        self.dragging_progress = False
        position = self.progress_slider.value() / self.progress_max_value
        self.seek_requested.emit(position)
        if self.is_playing:
            self.show_controls()
        else:
            self.show_controls(False)

    def _on_progress_changed(self, value):
        """进度条值改变"""
        if self.dragging_progress:
            position = value / self.progress_max_value
            current_seconds = self.total_time * position
            self.time_label.setText(f"{self._format_time(current_seconds)} / {self._format_time(self.total_time)}")

    def _on_volume_pressed(self):
        """音量条按下"""
        self.dragging_volume = True
        self.hide_timer.stop()
        self.setCursor(Qt.CursorShape.ArrowCursor)

    def _on_volume_released(self):
        """音量条释放"""
        self.dragging_volume = False
        if self.is_playing:
            self.show_controls()
        else:
            self.show_controls(False)


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
        self.vlc_player: VlcPlayer | None = None
        self.is_fullscreen_mode = False
        self.normal_play_rate = 1.0
        self.is_fast_forward = False
        self.right_key_pressed = False
        self.original_parent = None
        self.original_layout = None
        self.hovering_progress = False

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
        self.setStyleSheet("QWidget { background-color: #000000}")
        self.setFocusPolicy(Qt.FocusPolicy.StrongFocus)

    def _resize_control_overlay(self, event):
        """调整控制面板大小"""
        super(VideoDisplayWidget, self.video_display).resizeEvent(event)
        self.control_overlay.setGeometry(self.video_display.rect())

    def _setup_connections(self):
        """设置信号连接"""
        self.control_overlay.play_pause_requested.connect(self.toggle_play_pause)
        self.control_overlay.fullscreen_requested.connect(self._toggle_fullscreen)
        self.control_overlay.back_requested.connect(self._on_back_button_clicked)
        self.control_overlay.volume_changed.connect(self._adjust_volume)
        self.control_overlay.seek_requested.connect(self.seek_video)
        self.control_overlay.playback_rate_changed.connect(self._set_playback_rate)

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
            self.video_display.current_qimage = None
            self.video_display.update()
            self.vlc_player.cleanup()
            self.vlc_player = None
            self.control_overlay.set_play_state(False)
            self.control_overlay.set_progress(0)
            self.control_overlay.set_time(0, 0)

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

    def _set_playback_rate(self, rate: float):
        """设置播放速度"""
        if self.vlc_player:
            self.vlc_player.set_playback_rate(rate)
            if not self.is_fast_forward:
                self.normal_play_rate = rate
            self.control_overlay.show_controls()

    def _toggle_fullscreen(self):
        """切换全屏状态"""
        if not self.is_fullscreen_mode:
            self._enter_fullscreen()
        else:
            self._exit_fullscreen()

    def _enter_fullscreen(self):
        """进入全屏模式"""
        self.is_fullscreen_mode = True
        self.original_parent = self.parent()
        self.original_layout = self.parent().layout() if self.parent() and self.parent().layout() else None
        self.setParent(None)
        self.showFullScreen()
        self.setFocus()

    def _exit_fullscreen(self):
        """退出全屏模式"""
        self.is_fullscreen_mode = False
        self.showNormal()
        if self.original_parent and self.original_layout:
            self.setParent(self.original_parent)
            self.original_layout.addWidget(self)
        self.setFocus()

    def _on_back_button_clicked(self):
        """处理返回按钮点击"""
        self.stop_video()
        thread_manager.cancel_all_site_searches()
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
        # 左方向键 - 后退5秒
        elif key == Qt.Key.Key_Left:
            self._jump_backward(5)
        # 右方向键 - 前进5秒/3倍速
        elif key == Qt.Key.Key_Right:
            if not event.isAutoRepeat():
                self.right_key_pressed = True
                self.right_key_timer.start(200)
        # 上方向键 - 音量增加
        elif key == Qt.Key.Key_Up:
            self._adjust_volume_delta(5)
        # 下方向键 - 音量减少
        elif key == Qt.Key.Key_Down:
            self._adjust_volume_delta(-5)

    def keyReleaseEvent(self, event):
        """键盘释放事件"""
        if event.key() == Qt.Key.Key_Right and not event.isAutoRepeat():
            self.right_key_pressed = False
            self.right_key_timer.stop()
            if self.is_fast_forward:
                current_rate = self.control_overlay.get_current_playback_rate()
                self._set_playback_rate(current_rate)
                self.is_fast_forward = False
            else:
                self._jump_forward(5)

    def _on_right_key_held(self):
        """右方向键长按处理"""
        if self.right_key_pressed and self.vlc_player and not self.is_fast_forward:
            current_rate = self.control_overlay.get_current_playback_rate()
            if current_rate != 3.0:
                self.normal_play_rate = current_rate
                self._set_playback_rate(3.0)
                self.is_fast_forward = True

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
                new_time = min(current_time + seconds * 1000, total_time)
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