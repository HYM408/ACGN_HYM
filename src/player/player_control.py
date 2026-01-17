from PySide6.QtGui import QPixmap
from PySide6.QtCore import Qt, QTimer, Signal, QPoint, QSize
from PySide6.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QPushButton, QLabel, QSlider


class ClickableSlider(QSlider):
    """自定义进度条"""
    clicked = Signal(int)
    hover_time = Signal(float, int)

    def __init__(self, orientation=Qt.Orientation.Horizontal, threshold_percent=2):
        super().__init__(orientation)
        self.setMouseTracking(True)
        self.threshold_percent = threshold_percent

    def mousePressEvent(self, event):
        """鼠标点击事件"""
        if event.button() == Qt.MouseButton.LeftButton:
            pos = event.position().toPoint()
            available_width = max(1, (self.width() - 20))
            value = (pos.x() - 10) / available_width * self.maximum()
            value = int(max(0, min(self.maximum(), value)))
            current_value = self.value()
            threshold = self.maximum() * self.threshold_percent / 100
            if abs(value - current_value) <= threshold:
                super().mousePressEvent(event)
            else:
                self.setValue(value)
                self.clicked.emit(value)
                self.sliderPressed.emit()
                self.sliderReleased.emit()

    def mouseMoveEvent(self, event):
        """鼠标移动事件"""
        super().mouseMoveEvent(event)
        pos = event.position().toPoint()
        available_width = max(1, (self.width() - 20))
        value = (pos.x() - 10) / available_width * self.maximum()
        value = max(0, min(self.maximum(), value))
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
        self.back_button = QPushButton()
        self.back_button.setFixedSize(28, 28)
        self.back_button.setStyleSheet("QPushButton {border: none; border-radius: 14px; background-color: transparent}")
        self.back_button.setIcon(QPixmap("icons/back2.png"))
        self.back_button.setIconSize(QSize(20, 20))
        self.back_button.clicked.connect(self.back_requested.emit)
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
        bottom_layout.addLayout(progress_layout)
        # 控制按钮区域
        controls_layout = QHBoxLayout()
        controls_layout.setSpacing(10)
        # 播放/暂停按钮
        self.play_button = QPushButton()
        self.play_button.setFixedSize(28, 28)
        self.play_button.setStyleSheet("QPushButton {border: none; border-radius: 14px; background-color: transparent}")
        self.play_button.clicked.connect(self.play_pause_requested.emit)
        self._update_play_button_icon()
        controls_layout.addWidget(self.play_button)
        # 音量区域
        volume_container = QHBoxLayout()
        volume_container.setSpacing(8)
        volume_container.setContentsMargins(0, 0, 0, 0)
        # 声音图标
        self.volume_icon_label = QLabel()
        self.volume_icon_label.setFixedSize(20, 20)
        self.volume_icon_label.setPixmap(QPixmap("icons/sound.png").scaled(20, 20, Qt.AspectRatioMode.KeepAspectRatio, Qt.TransformationMode.SmoothTransformation))
        self.volume_icon_label.setStyleSheet("background-color: transparent;")
        volume_container.addWidget(self.volume_icon_label)
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
        volume_container.addWidget(self.volume_slider)
        controls_layout.addLayout(volume_container)
        controls_layout.addStretch()
        # 全屏按钮
        self.fullscreen_button = QPushButton()
        self.fullscreen_button.setFixedSize(28, 28)
        self.fullscreen_button.setStyleSheet("QPushButton {border: none; border-radius: 14px; background-color: transparent}")
        self.fullscreen_button.setIcon(QPixmap("icons/full.png"))
        self.fullscreen_button.setIconSize(QSize(20, 20))
        self.fullscreen_button.clicked.connect(self.fullscreen_requested.emit)
        controls_layout.addWidget(self.fullscreen_button)
        bottom_layout.addLayout(controls_layout)
        main_layout.addWidget(self.bottom_widget)
        self.setMouseTracking(True)

    def _setup_timers(self):
        """设置定时器"""
        self.hide_timer = QTimer()
        self.hide_timer.setSingleShot(True)
        self.hide_timer.timeout.connect(self.hide_controls)
        # 单击检测定时器
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
            self.hide_timer.start(3000)
        else:
            self.show_controls()

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

    def show_controls(self):
        """显示控制面板"""
        if not self.is_visible:
            self.is_visible = True
            self.top_widget.show()
            self.bottom_widget.show()
        if self.is_playing:
            self.hide_timer.start(3000)
        self.setCursor(Qt.CursorShape.ArrowCursor)

    def hide_controls(self):
        """隐藏控制面板"""
        if self.is_visible and self.is_playing and not self.dragging_progress and not self.dragging_volume:
            self.is_visible = False
            self.top_widget.hide()
            self.bottom_widget.hide()
            self.setCursor(Qt.CursorShape.BlankCursor)

    def mouseMoveEvent(self, event):
        """鼠标移动事件"""
        super().mouseMoveEvent(event)
        pos = event.position().toPoint()
        if not self._is_in_control_area(pos):
            if not self.is_visible:
                self.show_controls()
            elif self.is_playing and not self.dragging_progress and not self.dragging_volume:
                self.hide_timer.start(3000)
        else:
            self.hide_timer.stop()
            self.is_visible = True
            self.top_widget.show()
            self.bottom_widget.show()
            self.setCursor(Qt.CursorShape.ArrowCursor)

    def mousePressEvent(self, event):
        """鼠标点击事件"""
        super().mousePressEvent(event)
        self.setCursor(Qt.CursorShape.ArrowCursor)
        pos = event.position().toPoint()
        if self._is_in_control_area(pos):
            self.show_controls()
        else:
            if event.button() == Qt.MouseButton.LeftButton:
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
        if self.is_playing:
            self.play_button.setIcon(QPixmap("icons/stop.png"))
            self.play_button.setIconSize(QSize(25, 25))
        else:
            self.play_button.setIcon(QPixmap("icons/play.png"))
            self.play_button.setIconSize(QSize(25, 25))

    def _format_time(self, seconds: float) -> str:
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
        else:
            self.time_tooltip.hide()

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
            self.hide_timer.start(3000)
        self.setCursor(Qt.CursorShape.ArrowCursor)

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
            self.hide_timer.start(3000)
        self.setCursor(Qt.CursorShape.ArrowCursor)