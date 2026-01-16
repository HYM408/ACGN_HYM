from PySide6.QtWidgets import QWidget
from src.player.player_widget import VideoPlayerWidget


def setup_video_player(player_widget: QWidget) -> VideoPlayerWidget:
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


def play_video_in_player(player_widget: QWidget, video_url: str) -> bool:
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