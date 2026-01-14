from PySide6.QtWidgets import QWidget
from PySide6.QtCore import Qt, QTimer, QRect, QPoint, Signal
from PySide6.QtGui import QPainter, QColor, QPaintEvent, QMouseEvent, QFont, QPixmap


class ControlOverlay(QWidget):
    """视频控制面板"""
    play_pause_requested = Signal()
    fullscreen_requested = Signal()
    back_requested = Signal()
    volume_changed = Signal(float)

    def __init__(self, parent=None):
        super().__init__(parent)
        self.progress_visible = True
        self.progress_position = 0.0
        self.dragging = False
        self.is_playing = True
        self.current_volume = 50
        # 颜色
        self.bg_color = QColor(0, 0, 0, 180)
        self.prog_color = QColor(220, 50, 50, 220)
        self.point_color = QColor(255, 255, 255, 220)
        self.btn_color = QColor(255, 255, 255, 220)
        self.text_color = QColor(255, 255, 255, 220)
        # 按钮尺寸
        self.btn_radius = 14
        # 返回按钮图片
        self.back_icon = QPixmap("icons/back2.png")
        # 时间
        self.current_time = 0
        self.total_time = 0
        # 定时器
        self.hide_timer = QTimer(self)
        self.hide_timer.timeout.connect(self.hide_progress)
        self.hide_timer.setSingleShot(True)
        self.setMouseTracking(True)
        self.setAttribute(Qt.WA_TransparentForMouseEvents, False)
        # 双击
        self.last_click_time = 0
        self.click_timer = QTimer()
        self.click_timer.setSingleShot(True)
        self.click_timer.timeout.connect(self._handle_single_click)
        self.pending_click_pos = None

    def set_play_state(self, playing):
        """设置播放状态"""
        self.is_playing = playing
        if self.progress_visible:
            self.update()

    def hide_progress(self):
        """隐藏进度条"""
        self.progress_visible = False
        self.update()

    def show_progress(self):
        """显示进度条"""
        self.progress_visible = True
        self.update()
        self.hide_timer.start(3000)

    def set_progress(self, pos):
        """设置进度位置 (0.0-1.0)"""
        self.progress_position = max(0.0, min(1.0, pos))
        if self.progress_visible:
            self.update()

    def set_time(self, current_seconds, total_seconds):
        """设置当前时间和总时间"""
        self.current_time = current_seconds
        self.total_time = total_seconds
        if self.progress_visible:
            self.update()

    def set_volume(self, volume):
        """设置音量 (0-100)"""
        self.current_volume = max(0, min(100, volume))
        if self.progress_visible:
            self.update()

    def format_time(self, seconds):
        """格式化时间显示"""
        hours = int(seconds // 3600)
        minutes = int((seconds % 3600) // 60)
        secs = int(seconds % 60)
        if hours > 0:
            return f"{hours:02d}:{minutes:02d}:{secs:02d}"
        else:
            return f"{minutes:02d}:{secs:02d}"

    def paintEvent(self, event: QPaintEvent):
        """控制面板"""
        if not self.progress_visible:
            return
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        w, h = self.width(), self.height()
        # 返回按钮
        back_btn_center = QPoint(30, 30)
        self.back_btn_center = back_btn_center
        back_btn_radius = self.btn_radius
        # 返回按钮背景
        painter.setPen(Qt.NoPen)
        painter.setBrush(self.bg_color)
        painter.drawEllipse(back_btn_center, back_btn_radius, back_btn_radius)
        # 返回按钮图标
        icon_size = int(back_btn_radius * 1.4)
        icon_x = back_btn_center.x() - icon_size // 2
        icon_y = back_btn_center.y() - icon_size // 2
        scaled_pixmap = self.back_icon.scaled(icon_size, icon_size, Qt.KeepAspectRatio, Qt.SmoothTransformation)
        painter.drawPixmap(icon_x, icon_y, scaled_pixmap)
        # 底部控制面板
        control_height = 80
        control_y = h - control_height
        # 绘制半透明背景
        painter.fillRect(0, control_y, w, control_height, self.bg_color)
        # 进度条
        bar_y = control_y + 30
        bar_x_start = 20
        bar_width = w - 40
        bar_height = 8
        # 进度条背景
        painter.setPen(Qt.NoPen)
        painter.setBrush(QColor(100, 100, 100, 150))
        painter.drawRect(bar_x_start, bar_y, bar_width, bar_height)
        # 进度条
        if self.progress_position > 0:
            prog_width = int(bar_width * self.progress_position)
            painter.setBrush(self.prog_color)
            painter.drawRect(bar_x_start, bar_y, prog_width, bar_height)
            # 进度点
            point_x = bar_x_start + prog_width
            painter.setBrush(self.point_color)
            painter.drawEllipse(QPoint(point_x, bar_y + bar_height // 2), 8, 8)
        # 时间显示
        time_font = QFont("Arial", 11)
        painter.setFont(time_font)
        painter.setPen(self.text_color)
        current_time_str = self.format_time(self.current_time)
        total_time_str = self.format_time(self.total_time)
        time_text = f"{current_time_str} / {total_time_str}"
        text_x = bar_x_start
        painter.drawText(text_x, bar_y - 8, time_text)
        # 按钮区域
        btn_y = control_y + 65
        # 计算按钮位置
        play_btn_center = QPoint(30, btn_y)
        volume_btn_center = QPoint(70, btn_y)
        fullscreen_btn_center = QPoint(w - 30, btn_y)
        # 存储按钮中心点用于鼠标事件
        self.play_btn_center = play_btn_center
        self.volume_btn_center = volume_btn_center
        self.fullscreen_btn_center = fullscreen_btn_center
        # 播放/暂停
        painter.setBrush(self.btn_color)
        painter.drawEllipse(play_btn_center, self.btn_radius, self.btn_radius)
        # 播放/暂停图标
        painter.setBrush(QColor(30, 30, 30, 220))
        icon_size = 9
        if self.is_playing:
            pause_width = 3
            gap = 3
            left_x = play_btn_center.x() - pause_width - gap//2
            right_x = play_btn_center.x() + gap//2
            painter.drawRect(left_x - pause_width//2, btn_y - icon_size//2, pause_width, icon_size)
            painter.drawRect(right_x - pause_width//2, btn_y - icon_size//2, pause_width, icon_size)
        else:
            points = [
                QPoint(play_btn_center.x() - icon_size//2, btn_y - icon_size),
                QPoint(play_btn_center.x() - icon_size//2, btn_y + icon_size),
                QPoint(play_btn_center.x() + icon_size, btn_y)
            ]
            painter.drawPolygon(points)
        # 音量按钮
        painter.setBrush(self.btn_color)
        painter.drawEllipse(volume_btn_center, self.btn_radius, self.btn_radius)
        # 音量图标
        painter.setBrush(QColor(30, 30, 30, 220))
        # 绘制扬声器图标
        speaker_width = 9
        speaker_height = 7
        speaker_x = volume_btn_center.x() - speaker_width//2
        speaker_y = btn_y - speaker_height//2
        # 扬声器主体
        painter.drawRect(speaker_x, speaker_y, 4, speaker_height)
        # 音量条
        volume_bar_x = volume_btn_center.x() + 20
        volume_bar_width = 80
        volume_bar_height = 5
        # 音量条背景
        painter.setBrush(QColor(100, 100, 100, 150))
        painter.drawRect(volume_bar_x, btn_y - volume_bar_height//2, volume_bar_width, volume_bar_height)
        # 音量条
        volume_width = int(volume_bar_width * (self.current_volume / 100))
        painter.setBrush(self.prog_color)
        painter.drawRect(volume_bar_x, btn_y - volume_bar_height//2, volume_width, volume_bar_height)
        # 音量点
        volume_point_x = volume_bar_x + volume_width
        painter.setBrush(self.point_color)
        painter.drawEllipse(QPoint(volume_point_x, btn_y), 5, 5)
        # 全屏按钮
        painter.setBrush(self.btn_color)
        painter.drawEllipse(fullscreen_btn_center, self.btn_radius, self.btn_radius)
        # 全屏图标
        painter.setBrush(QColor(30, 30, 30, 220))
        arrow_size = 7
        # 左上箭头
        points1 = [
            QPoint(fullscreen_btn_center.x() - arrow_size, fullscreen_btn_center.y() - arrow_size),
            QPoint(fullscreen_btn_center.x() - arrow_size, fullscreen_btn_center.y() - arrow_size//2),
            QPoint(fullscreen_btn_center.x() - arrow_size//2, fullscreen_btn_center.y() - arrow_size//2),
            QPoint(fullscreen_btn_center.x() - arrow_size//2, fullscreen_btn_center.y() - arrow_size)
        ]
        painter.drawPolygon(points1)
        # 右下箭头
        points2 = [
            QPoint(fullscreen_btn_center.x() + arrow_size, fullscreen_btn_center.y() + arrow_size),
            QPoint(fullscreen_btn_center.x() + arrow_size, fullscreen_btn_center.y() + arrow_size//2),
            QPoint(fullscreen_btn_center.x() + arrow_size//2, fullscreen_btn_center.y() + arrow_size//2),
            QPoint(fullscreen_btn_center.x() + arrow_size//2, fullscreen_btn_center.y() + arrow_size)
        ]
        painter.drawPolygon(points2)

    def mousePressEvent(self, event: QMouseEvent):
        """鼠标按下事件"""
        if event.button() != Qt.LeftButton:
            return
        pos = event.position().toPoint()
        self.pending_click_pos = pos
        in_back_btn = self._is_in_back_button(pos)
        in_play_btn = self._is_in_play_button(pos)
        in_volume_area = self._is_in_volume_area(pos)
        in_fullscreen_btn = self._is_in_fullscreen_button(pos)
        in_progress_bar = self._is_in_progress_bar(pos)
        if in_back_btn:
            self.click_timer.stop()
            self.back_requested.emit()
            return
        if in_play_btn:
            self.click_timer.stop()
            self.play_pause_requested.emit()
            return
        if in_fullscreen_btn:
            self.click_timer.stop()
            self.fullscreen_requested.emit()
            return
        if in_volume_area:
            self.dragging = True
            self.update_volume_from_pos(pos)
            return
        if in_progress_bar:
            self.dragging = True
            self.update_position(pos)
            return
        current_time = event.timestamp()
        if current_time - self.last_click_time < 300:
            self.click_timer.stop()
            self._handle_double_click()
        else:
            self.last_click_time = current_time
            self.click_timer.start(300)

    def _handle_single_click(self):
        """处理单击事件"""
        if self.pending_click_pos:
            if self._is_in_control_area(self.pending_click_pos):
                self.play_pause_requested.emit()
            self.pending_click_pos = None

    def _handle_double_click(self):
        """处理双击事件"""
        self.fullscreen_requested.emit()
        self.pending_click_pos = None

    def _is_in_back_button(self, pos):
        """检查是否在返回按钮内"""
        if not hasattr(self, 'back_btn_center'):
            return False
        distance = (pos - self.back_btn_center).manhattanLength()
        return distance <= self.btn_radius

    def _is_in_play_button(self, pos):
        """检查是否在播放按钮内"""
        if not hasattr(self, 'play_btn_center'):
            return False
        distance = (pos - self.play_btn_center).manhattanLength()
        return distance <= self.btn_radius

    def _is_in_volume_area(self, pos):
        """检查是否在音量控制区域"""
        if not hasattr(self, 'volume_btn_center'):
            return False
        btn_distance = (pos - self.volume_btn_center).manhattanLength()
        if btn_distance <= self.btn_radius:
            return True
        volume_bar_x = self.volume_btn_center.x() + 20
        volume_bar_width = 80
        volume_bar_rect = QRect(volume_bar_x,
                               self.volume_btn_center.y() - 2,
                               volume_bar_width, 5)
        return volume_bar_rect.contains(pos)

    def _is_in_fullscreen_button(self, pos):
        """检查是否在全屏按钮内"""
        if not hasattr(self, 'fullscreen_btn_center'):
            return False
        distance = (pos - self.fullscreen_btn_center).manhattanLength()
        return distance <= self.btn_radius

    def _is_in_progress_bar(self, pos):
        """检查是否在进度条内"""
        h = self.height()
        control_y = h - 80
        bar_y = control_y + 30
        bar_x_start = 20
        bar_width = self.width() - 40
        bar_rect = QRect(bar_x_start, bar_y - 6, bar_width, 12)
        return bar_rect.contains(pos)

    def _is_in_control_area(self, pos):
        """检查是否在控制面板区域"""
        h = self.height()
        control_y = h - 80
        control_rect = QRect(0, control_y, self.width(), 80)
        return control_rect.contains(pos)

    def mouseMoveEvent(self, event: QMouseEvent):
        """鼠标移动事件"""
        if self.dragging:
            pos = event.position().toPoint()
            if self._is_in_volume_area(pos):
                self.update_volume_from_pos(pos)
            elif self._is_in_progress_bar(pos):
                self.update_position(pos)
        self.show_progress()

    def mouseReleaseEvent(self, event: QMouseEvent):
        """鼠标释放事件"""
        if event.button() == Qt.LeftButton and self.dragging:
            self.dragging = False
            self.hide_timer.start(3000)

    def update_position(self, mouse_pos):
        """根据鼠标位置更新播放进度"""
        h = self.height()
        control_y = h - 80
        bar_y = control_y + 30
        bar_x_start = 20
        bar_width = self.width() - 40
        rel_x = mouse_pos.x() - bar_x_start
        new_pos = max(0, min(1, rel_x / bar_width))
        self.set_progress(new_pos)
        if hasattr(self.parent(), 'seek_requested'):
            self.parent().seek_requested.emit(new_pos)

    def update_volume_from_pos(self, mouse_pos):
        """根据鼠标位置更新音量"""
        if not hasattr(self, 'volume_btn_center'):
            return
        volume_bar_x = self.volume_btn_center.x() + 20
        volume_bar_width = 80
        rel_x = mouse_pos.x() - volume_bar_x
        new_volume = max(0, min(100, int((rel_x / volume_bar_width) * 100)))
        if new_volume != self.current_volume:
            self.set_volume(new_volume)
            self.volume_changed.emit(new_volume / 100.0)