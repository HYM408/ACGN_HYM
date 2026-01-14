from PySide6.QtWidgets import QDialog, QVBoxLayout, QLabel, QLineEdit, QPushButton, QMessageBox
from src.thread_manager import thread_manager
from src.sqlite import insert_many_data, clear_table
from src.config import get_config_item, set_config_items


class ClientCredentialDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("授权信息缺失")
        self.setFixedSize(400, 300)
        layout = QVBoxLayout()
        link_label = QLabel(
            '前往创建(需登录bangumi):<br>'
            '<a href="https://bangumi.tv/dev/app">https://bangumi.tv/dev/app</a><br>'
            '<a href="https://bgm.tv/dev/app">https://bgm.tv/dev/app</a><br>'
            '<a href="https://chii.in/dev/app">https://chii.in/dev/app</a>'
        )
        link_label.setOpenExternalLinks(True)
        layout.addWidget(link_label)
        layout.addWidget(QLabel("Client ID:"))
        self.client_id_input = QLineEdit()
        self.client_id_input.setPlaceholderText("输入 App ID")
        layout.addWidget(self.client_id_input)
        layout.addWidget(QLabel("Client Secret:"))
        self.client_secret_input = QLineEdit()
        self.client_secret_input.setPlaceholderText("输入 App Secret")
        layout.addWidget(self.client_secret_input)
        layout.addWidget(QLabel("Redirect URI:"))
        self.redirect_uri_input = QLineEdit()
        self.redirect_uri_input.setPlaceholderText("输入 回调地址")
        layout.addWidget(self.redirect_uri_input)
        self.confirm_button = QPushButton("确定")
        self.confirm_button.clicked.connect(self.accept)
        layout.addWidget(self.confirm_button)
        self.setLayout(layout)


class SettingsPageManager:
    def __init__(self, settings_page_widget):
        self.settings_page = settings_page_widget
        # 连接线程管理器
        thread_manager.auth_success.connect(self.on_auth_success)
        thread_manager.auth_error.connect(self.on_auth_error)
        thread_manager.collection_ready.connect(self.on_collection_data_ready)
        thread_manager.collection_error.connect(self.on_collection_error)
        self.update_token_display()

    def get_client_credentials(self):
        """授权信息缺失提示"""
        required_fields = ["client_id", "client_secret", "redirect_uri"]
        for field in required_fields:
            if not get_config_item(field, ""):
                dialog = ClientCredentialDialog(self.settings_page)
                if not dialog.exec():
                    return False
                client_id = dialog.client_id_input.text().strip()
                client_secret = dialog.client_secret_input.text().strip()
                redirect_uri = dialog.redirect_uri_input.text().strip()
                if not all([client_id, client_secret, redirect_uri]):
                    QMessageBox.warning(self.settings_page, "警告", "所有字段都必须填写完整！")
                    return False
                set_config_items(client_id=client_id, client_secret=client_secret, redirect_uri=redirect_uri)
                break
        return True

    def update_token_display(self):
        """显示token"""
        access_token = get_config_item("access_token", "")
        refresh_token = get_config_item("refresh_token", "")
        user_id = get_config_item("user_id", "")
        self.settings_page.ui.pushButton_2.setText(f"ID: {user_id}")
        self.settings_page.ui.pushButton_9.setText(f"Access Token: {access_token}")
        self.settings_page.ui.pushButton_8.setText(f"Refresh Token: {refresh_token}")

    def login_action(self):
        """Bangumi授权"""
        if not self.get_client_credentials():
            return
        thread_manager.start_auth()

    def on_auth_success(self):
        """认证成功处理"""
        self.update_token_display()

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