from PySide6.QtCore import Qt, QPoint
from PySide6.QtWidgets import QWidget, QVBoxLayout, QPushButton, QFrame


def create_status_selector(parent_button, on_status_selected=None):
    """状态下拉菜单"""
    status_list = ["想看", "在看", "看过", "搁置", "抛弃", "取消追番"]
    selector_widget = QWidget()
    selector_widget.setWindowFlags(Qt.WindowType.Popup | Qt.WindowType.FramelessWindowHint)
    selector_widget.setFixedWidth(120)
    main_layout = QVBoxLayout(selector_widget)
    main_layout.setSpacing(0)
    main_layout.setContentsMargins(0, 0, 0, 0)
    content_frame = QFrame()
    content_frame.setObjectName("StatusSelectorFrame")
    content_frame.setStyleSheet("""
        #StatusSelectorFrame {
            background-color: white;
            border: 1px solid #e0e0e0;
        }
        QPushButton {
            background-color: white;
            border: none;
            padding: 10px 12px;
            text-align: left;
            font-family: 'Microsoft YaHei';
            font-size: 13px;
            color: #333;
        }
        QPushButton:hover {
            background-color: #f5f5f5;
        }
    """)
    frame_layout = QVBoxLayout(content_frame)
    frame_layout.setSpacing(0)
    frame_layout.setContentsMargins(0, 0, 0, 0)
    for status in status_list:
        btn = QPushButton(status)
        btn.clicked.connect(lambda checked, s=status: handle_status_click(s, selector_widget, on_status_selected))
        frame_layout.addWidget(btn)
    main_layout.addWidget(content_frame)
    if parent_button:
        global_pos = parent_button.mapToGlobal(QPoint(0, parent_button.height()))
        selector_widget.move(global_pos)
    return selector_widget

def handle_status_click(status_text, selector_widget, on_status_selected):
    """处理状态按钮点击"""
    selector_widget.hide()
    if on_status_selected:
        on_status_selected(status_text)

def show_status_selector(parent_button, on_status_selected=None):
    """显示状态下拉菜单"""
    selector = create_status_selector(parent_button, on_status_selected)
    selector.show()
    return selector