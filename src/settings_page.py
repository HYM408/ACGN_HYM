from src.thread_manager import thread_manager
from src.sqlite import insert_many_data, clear_table


class SettingsPageManager:
    def __init__(self, settings_page_widget):
        self.settings_page = settings_page_widget
        # 连接线程管理器
        thread_manager.auth_success.connect(self.on_auth_success)
        thread_manager.auth_error.connect(self.on_auth_error)
        thread_manager.collection_ready.connect(self.on_collection_data_ready)
        thread_manager.collection_error.connect(self.on_collection_error)

    def login_action(self):
        """Bangumi授权"""
        thread_manager.start_auth()

    def on_auth_success(self, message):
        """认证成功处理"""
        print(message)

    def on_auth_error(self, message):
        """认证错误处理"""
        print(message)

    # ===============收藏===============
    def get_collections_action(self):
        """获取收藏"""
        thread_manager.fetch_collections()

    def on_collection_data_ready(self, collections):
        """在主线程中处理收藏数据"""
        try:
            clear_table()
            insert_many_data(collections)
            print("收藏数据同步完成")
        except Exception as e:
            print(f"处理收藏数据失败: {e}")

    def on_collection_error(self):
        """获取收藏错误处理"""
        print("获取收藏失败")