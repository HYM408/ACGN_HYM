from PySide6.QtWidgets import QWidget
from PySide6.QtCore import Qt, QTimer, QRect, QPoint, Signal
from PySide6.QtGui import QPainter, QColor, QPaintEvent, QMouseEvent, QFont, QPixmap


class ControlStyle:
    """控制面板样式"""
    BG_COLOR = QColor(0, 0, 0, 180)
    PROGRESS_COLOR = QColor(220, 50, 50, 220)
    BUTTON_COLOR = QColor(255, 255, 255, 220)
    TEXT_COLOR = QColor(255, 255, 255, 220)

    CONTROL_HEIGHT = 80
    BUTTON_RADIUS = 14
    PROGRESS_BAR_HEIGHT = 8
    VOLUME_BAR_WIDTH = 80


class ControlOverlay(QWidget):
    """视频控制面板"""
    play_pause_requested = Signal()
    fullscreen_requested = Signal()
    back_requested = Signal()
    volume_changed = Signal(int)
    seek_requested = Signal(float)

    def __init__(self, parent=None):
        super().__init__(parent)
        self.style = ControlStyle()
        self._init_state()
        self._init_ui()
        self._setup_timers()
        self.setMouseTracking(True)

    def _init_state(self):
        """初始化状态"""
        self.progress_visible = True
        self.progress_position = 0.0
        self.dragging = False
        self.is_playing = True
        self.current_volume = 50
        # 按钮位置
        self.button_positions = {}
        # 时间
        self.current_time = 0
        self.total_time = 0
        # 双击
        self.last_click_time = 0
        self.pending_click_pos = None

    def _init_ui(self):
        """初始化图片资源"""
        self.back_icon = QPixmap("icons/back2.png")

    def _setup_timers(self):
        """设置定时器"""
        # 自动隐藏定时器
        self.hide_timer = QTimer(self)
        self.hide_timer.timeout.connect(self.hide_controls)
        self.hide_timer.setSingleShot(True)
        # 单击检测定时器
        self.click_timer = QTimer(self)
        self.click_timer.setSingleShot(True)
        self.click_timer.timeout.connect(self._handle_single_click)
        # 音量防抖定时器
        self.volume_debounce_timer = QTimer(self)
        self.volume_debounce_timer.setSingleShot(True)
        self.volume_debounce_timer.timeout.connect(self._emit_volume_change)
        self.debounced_volume = 50

    def set_play_state(self, playing: bool):
        """设置播放状态"""
        self.is_playing = playing
        if self.progress_visible:
            self.update()

    def set_progress(self, position: float):
        """设置进度位置 (0.0-1.0)"""
        self.progress_position = max(0.0, min(1.0, position))
        if self.progress_visible:
            self.update()

    def set_time(self, current_seconds: float, total_seconds: float):
        """设置当前时间和总时间"""
        self.current_time = current_seconds
        self.total_time = total_seconds
        if self.progress_visible:
            self.update()

    def set_volume(self, volume: int):
        """设置音量 (0-100)"""
        self.current_volume = max(0, min(100, volume))
        if self.progress_visible:
            self.update()

    def show_controls(self):
        """显示控制面板"""
        self.progress_visible = True
        self.update()
        self.hide_timer.start(3000)
    def hide_controls(self):
        """隐藏控制面板"""
        self.progress_visible = False
        self.update()

    def paintEvent(self, event: QPaintEvent):
        """控制面板"""
        if not self.progress_visible:
            return
        painter = QPainter(self)
        painter.setRenderHint(QPainter.RenderHint.Antialiasing)
        w, h = self.width(), self.height()
        # 返回按钮
        self._draw_back_button(painter)
        # 底部控制面板
        control_y = h - self.style.CONTROL_HEIGHT
        painter.fillRect(0, control_y, w, self.style.CONTROL_HEIGHT, self.style.BG_COLOR)
        # 进度条
        self._draw_progress_bar(painter, control_y)
        # 控制按钮
        self._draw_control_buttons(painter, control_y)

    def _draw_back_button(self, painter: QPainter):
        """返回按钮"""
        center = QPoint(30, 30)
        self.button_positions['back'] = (center, self.style.BUTTON_RADIUS)
        painter.setPen(Qt.PenStyle.NoPen)
        painter.setBrush(self.style.BG_COLOR)
        painter.drawEllipse(center, self.style.BUTTON_RADIUS, self.style.BUTTON_RADIUS)
        # 图标
        icon_size = int(self.style.BUTTON_RADIUS * 1.4)
        scaled_pixmap = self.back_icon.scaled(icon_size, icon_size, Qt.AspectRatioMode.KeepAspectRatio, Qt.TransformationMode.SmoothTransformation)
        painter.drawPixmap(center.x() - icon_size//2,
                          center.y() - icon_size//2,
                          scaled_pixmap)

    def _draw_progress_bar(self, painter: QPainter, control_y: int):
        """进度条"""
        bar_y = control_y + 30
        bar_x_start = 20
        bar_width = self.width() - 40
        # 进度条背景
        painter.setBrush(QColor(100, 100, 100, 150))
        painter.drawRect(bar_x_start, bar_y, bar_width, self.style.PROGRESS_BAR_HEIGHT)
        # 进度条前景
        if self.progress_position > 0:
            prog_width = int(bar_width * self.progress_position)
            painter.setBrush(self.style.PROGRESS_COLOR)
            painter.drawRect(bar_x_start, bar_y, prog_width, self.style.PROGRESS_BAR_HEIGHT)
            # 进度点
            point_x = bar_x_start + prog_width
            painter.setBrush(QColor(255, 255, 255, 220))
            painter.drawEllipse(QPoint(point_x, bar_y + self.style.PROGRESS_BAR_HEIGHT//2), 8, 8)
        # 时间显示
        time_font = QFont("Arial", 11)
        painter.setFont(time_font)
        painter.setPen(self.style.TEXT_COLOR)
        current_str = self._format_time(self.current_time)
        total_str = self._format_time(self.total_time)
        painter.drawText(bar_x_start, bar_y - 8, f"{current_str} / {total_str}")

    def _draw_control_buttons(self, painter: QPainter, control_y: int):
        """控制按钮"""
        btn_y = control_y + 65
        w = self.width()
        positions = {'play': QPoint(30, btn_y), 'volume': QPoint(70, btn_y), 'fullscreen': QPoint(w - 30, btn_y)}
        for key, center in positions.items():
            self.button_positions[key] = (center, self.style.BUTTON_RADIUS)
            # 按钮背景
            painter.setBrush(self.style.BUTTON_COLOR)
            painter.drawEllipse(center, self.style.BUTTON_RADIUS, self.style.BUTTON_RADIUS)
            # 图标
            painter.setBrush(QColor(30, 30, 30, 220))
            if key == 'play':
                self._draw_play_icon(painter, center, btn_y)
            elif key == 'volume':
                self._draw_volume_icon(painter, center, btn_y)
            elif key == 'fullscreen':
                self._draw_fullscreen_icon(painter, center)

    def _draw_play_icon(self, painter: QPainter, center: QPoint, btn_y: int):
        """播放/暂停"""
        icon_size = 9
        if self.is_playing:
            pause_width = 3
            gap = 3
            left_x = center.x() - pause_width - gap//2
            right_x = center.x() + gap//2
            painter.drawRect(left_x - pause_width//2, btn_y - icon_size//2, pause_width, icon_size)
            painter.drawRect(right_x - pause_width//2, btn_y - icon_size//2, pause_width, icon_size)
        else:
            points = [
                QPoint(center.x() - icon_size//2, btn_y - icon_size),
                QPoint(center.x() - icon_size//2, btn_y + icon_size),
                QPoint(center.x() + icon_size, btn_y)
            ]
            painter.drawPolygon(points)

    def _draw_volume_icon(self, painter: QPainter, center: QPoint, btn_y: int):
        """音量图标和滑块"""
        # 扬声器图标
        speaker_width = 9
        speaker_height = 7
        painter.drawRect(center.x() - speaker_width//2, btn_y - speaker_height//2, 4, speaker_height)
        # 音量滑块
        volume_bar_x = center.x() + 20
        volume_bar_width = self.style.VOLUME_BAR_WIDTH
        # 背景
        painter.setBrush(QColor(100, 100, 100, 150))
        painter.drawRect(volume_bar_x, btn_y - 2, volume_bar_width, 5)
        # 前景
        volume_width = int(volume_bar_width * (self.current_volume / 100))
        painter.setBrush(self.style.PROGRESS_COLOR)
        painter.drawRect(volume_bar_x, btn_y - 2, volume_width, 5)
        # 滑块点
        volume_point_x = volume_bar_x + volume_width
        painter.setBrush(QColor(255, 255, 255, 220))
        painter.drawEllipse(QPoint(volume_point_x, btn_y), 5, 5)

    @staticmethod
    def _draw_fullscreen_icon(painter: QPainter, center: QPoint):
        """全屏图标"""
        arrow_size = 7
        # 左上箭头
        points1 = [
            QPoint(center.x() - arrow_size, center.y() - arrow_size),
            QPoint(center.x() - arrow_size, center.y() - arrow_size//2),
            QPoint(center.x() - arrow_size//2, center.y() - arrow_size//2),
            QPoint(center.x() - arrow_size//2, center.y() - arrow_size)
        ]
        painter.drawPolygon(points1)
        # 右下箭头
        points2 = [
            QPoint(center.x() + arrow_size, center.y() + arrow_size),
            QPoint(center.x() + arrow_size, center.y() + arrow_size//2),
            QPoint(center.x() + arrow_size//2, center.y() + arrow_size//2),
            QPoint(center.x() + arrow_size//2, center.y() + arrow_size)
        ]
        painter.drawPolygon(points2)

    @staticmethod
    def _format_time(seconds: float) -> str:
        """格式化时间显示"""
        hours = int(seconds // 3600)
        minutes = int((seconds % 3600) // 60)
        secs = int(seconds % 60)
        if hours > 0:
            return f"{hours:02d}:{minutes:02d}:{secs:02d}"
        return f"{minutes:02d}:{secs:02d}"

    def _is_point_in_button(self, pos: QPoint, button_name: str) -> bool:
        """检查点是否在按钮内"""
        if button_name not in self.button_positions:
            return False
        center, radius = self.button_positions[button_name]
        distance = (pos - center).manhattanLength()
        return distance <= radius

    def _is_point_in_volume_slider(self, pos: QPoint) -> bool:
        """检查点是否在音量滑块内"""
        if 'volume' not in self.button_positions:
            return False
        center, radius = self.button_positions['volume']
        if (pos - center).manhattanLength() <= radius:
            return True
        volume_bar_x = center.x() + 20
        volume_bar_rect = QRect(volume_bar_x, center.y() - 2, self.style.VOLUME_BAR_WIDTH, 5)
        return volume_bar_rect.contains(pos)

    def _is_point_in_progress_bar(self, pos: QPoint) -> bool:
        """检查点是否在进度条内"""
        h = self.height()
        control_y = h - self.style.CONTROL_HEIGHT
        bar_y = control_y + 30
        bar_x_start = 20
        bar_width = self.width() - 40
        bar_rect = QRect(bar_x_start, bar_y - 6, bar_width, 12)
        return bar_rect.contains(pos)

    def _is_point_in_control_area(self, pos: QPoint) -> bool:
        """检查点是否在控制面板区域"""
        h = self.height()
        control_y = h - self.style.CONTROL_HEIGHT
        control_rect = QRect(0, control_y, self.width(), self.style.CONTROL_HEIGHT)
        return control_rect.contains(pos)

    def mousePressEvent(self, event: QMouseEvent):
        """鼠标按下事件"""
        if event.button() != Qt.MouseButton.LeftButton:
            return
        pos = event.position().toPoint()
        # 检查按钮点击
        if self._is_point_in_button(pos, 'back'):
            self.click_timer.stop()
            self.back_requested.emit()
            return
        elif self._is_point_in_button(pos, 'play'):
            self.click_timer.stop()
            self.play_pause_requested.emit()
            return
        elif self._is_point_in_button(pos, 'fullscreen'):
            self.click_timer.stop()
            self.fullscreen_requested.emit()
            return
        elif self._is_point_in_volume_slider(pos):
            self.dragging = True
            self._update_volume_from_pos(pos)
            return
        elif self._is_point_in_progress_bar(pos):
            self.dragging = True
            self._update_progress_from_pos(pos)
            return
        elif self._is_point_in_control_area(pos):
            return
        # 双击检测
        current_time = event.timestamp()
        if current_time - self.last_click_time < 200:
            self.click_timer.stop()
            self.fullscreen_requested.emit()
        else:
            self.last_click_time = current_time
            self.pending_click_pos = pos
            self.click_timer.start(200)

    def mouseMoveEvent(self, event: QMouseEvent):
        """鼠标移动事件"""
        if self.dragging:
            pos = event.position().toPoint()
            if self._is_point_in_volume_slider(pos):
                self._update_volume_from_pos(pos)
            elif self._is_point_in_progress_bar(pos):
                self._update_progress_from_pos(pos)
        self.show_controls()

    def mouseReleaseEvent(self, event: QMouseEvent):
        """鼠标释放事件"""
        if event.button() == Qt.MouseButton.LeftButton and self.dragging:
            self.dragging = False
            if self.volume_debounce_timer.isActive():
                self.volume_debounce_timer.stop()
                self._emit_volume_change()
            self.hide_timer.start(3000)

    def _handle_single_click(self):
        """处理单击事件"""
        self.play_pause_requested.emit()
        self.pending_click_pos = None

    def _update_progress_from_pos(self, mouse_pos: QPoint):
        """根据鼠标位置更新进度"""
        bar_x_start = 20
        bar_width = self.width() - 40
        rel_x = mouse_pos.x() - bar_x_start
        new_pos = max(0.0, min(1.0, rel_x / bar_width))
        self.set_progress(new_pos)
        self.seek_requested.emit(new_pos)

    def _update_volume_from_pos(self, mouse_pos: QPoint):
        """根据鼠标位置更新音量"""
        if 'volume' not in self.button_positions:
            return
        center, _ = self.button_positions['volume']
        volume_bar_x = center.x() + 20
        rel_x = mouse_pos.x() - volume_bar_x
        new_volume = max(0, min(100, int((rel_x / self.style.VOLUME_BAR_WIDTH) * 100)))
        if new_volume != self.current_volume:
            self.set_volume(new_volume)
        self.debounced_volume = new_volume
        self.volume_debounce_timer.start(300)

    def _emit_volume_change(self):
        """发送音量变化信号"""
        volume_int = int(max(0, min(100, self.debounced_volume)))
        self.volume_changed.emit(volume_int)