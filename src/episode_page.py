from PySide6.QtGui import QFont
from PySide6.QtWidgets import QPushButton, QLabel
from PySide6.QtCore import QObject, Qt, Signal
from src.sqlite import get_episodes_by_subject_id
from src.thread_manager import thread_manager


class EpisodeManager(QObject):
    episodes_layout_updated = Signal()

    def __init__(self, episode_page_widget, main_window=None):
        super().__init__()
        self.episode_page = episode_page_widget
        self.main_window = main_window
        self.collection_data = None
        self.episodes = []
        self.total_rows = 0
        self.episode_page.pushButton_15.clicked.connect(self.mark_all_episodes_watched)
        # 连接线程管理器
        thread_manager.episodes_fetched.connect(self.on_episodes_fetched)
        thread_manager.episodes_marked.connect(self.on_mark_all_watched_finished)

    def episode_page_data(self, collection_data):
        """剧集数据"""
        self.collection_data = collection_data
        title = collection_data.get('subject_name_cn') or collection_data.get('subject_name')
        self.episode_page.pushButton_13.setText(title)
        self.clear_buttons()
        self.load_episodes_data()

    def clear_buttons(self):
        """清空所有按钮"""
        layout = self.episode_page.gridLayout_3
        while layout.count():
            item = layout.takeAt(0)
            if item.widget():
                item.widget().deleteLater()

    def load_episodes_data(self):
        """加载剧集数据"""
        subject_id = self.collection_data.get('subject_id')
        db_episodes = get_episodes_by_subject_id(subject_id)
        if db_episodes:
            self.episodes = db_episodes
            self.create_episode_buttons(db_episodes)
        else:
            thread_manager.fetch_episodes(subject_id)

    def on_episodes_fetched(self, episodes):
        """剧集数据获取成功"""
        self.episodes = episodes
        self.create_episode_buttons(episodes)

    def create_episode_buttons(self, episodes):
        """动态创建剧集按钮"""
        layout = self.episode_page.gridLayout_3
        self.clear_buttons()
        if not episodes:
            no_episodes_label = QLabel("暂无剧集数据")
            no_episodes_label.setAlignment(Qt.AlignCenter)
            no_episodes_label.setFont(QFont(["微软雅黑"], 12))
            no_episodes_label.setStyleSheet("color: #888")
            layout.addWidget(no_episodes_label, 0, 0, 1, 12)
            self.total_rows = 1
            self.episodes_layout_updated.emit()
            return
        cols_per_row, row, col = 12, 0, 0
        normal_eps, sp_eps = [], []
        for ep in episodes:
            ep_type = ep.get('episode_type', 0) if 'episode' not in ep else ep.get('episode', {}).get('type', 0)
            sort_num = ep.get('sort') or ep.get('episode', {}).get('sort')
            col_type = ep.get('collection_type', 0) or ep.get('type', 0)
            if sort_num:
                item = (sort_num, col_type)
                (normal_eps if ep_type == 0 else sp_eps).append(item)

        def create_buttons(ep_list, prefix=""):
            nonlocal row, col
            for sort_num, col_type in ep_list:
                btn = QPushButton(f"{prefix}{sort_num}")
                btn.setFixedSize(40, 40)
                btn.setFont(QFont(["微软雅黑"], 10, QFont.Bold))
                color = "rgb(76, 175, 80)" if col_type == 2 else "white"
                btn.setStyleSheet(f"background-color: {color}; border: 1px solid #ddd; border-radius: 4px;")
                episode_data = next((ep for ep in episodes if (ep.get('sort') or ep.get('episode', {}).get('sort')) == sort_num),None)
                btn.clicked.connect(lambda checked, ep=episode_data: self.on_episode_clicked(ep))
                layout.addWidget(btn, row, col)
                col += 1
                if col >= cols_per_row:
                    col = 0
                    row += 1
        create_buttons(sorted(normal_eps))
        if sp_eps:
            if col != 0:
                col, row = 0, row + 1
            sp_label = QLabel("SP")
            sp_label.setFont(QFont(["微软雅黑"], 10, QFont.Bold))
            layout.addWidget(sp_label, row, 0, 1, 12)
            row += 1
            create_buttons(sorted(sp_eps))
        self.total_rows = row + 1
        layout.addItem(self.episode_page.verticalSpacer_5, row + 1, 0, 1, cols_per_row)
        self.episodes_layout_updated.emit()

    def get_total_rows(self):
        """获取总行数"""
        return self.total_rows

    def on_episode_clicked(self, episode_data):
        """跳转到播放器页面"""
        self.main_window.close_episode_overlay()
        self.main_window.PlayerManager.show_player_page(episode_data)

    def mark_all_episodes_watched(self):
        """标记全部剧集为已看"""
        if not self.episodes:
            return
        thread_manager.mark_episodes_watched(self.collection_data.get('subject_id'), self.episodes)
        self.episode_page.pushButton_15.setEnabled(False)
        self.episode_page.pushButton_15.setText("标记中...")

    def on_mark_all_watched_finished(self, success):
        """标记全部已看操作完成"""
        self.episode_page.pushButton_15.setEnabled(True)
        self.episode_page.pushButton_15.setText("全部已看")
        if success:
            self.create_episode_buttons(self.episodes)