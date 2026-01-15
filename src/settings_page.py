from PySide6.QtWidgets import QDialog, QVBoxLayout, QLabel, QLineEdit, QPushButton, QMessageBox
from src.thread_manager import thread_manager
from src.sqlite import insert_many_data, clear_table
from src.config import get_config_item, set_config_items


class SettingsPageManager:
    def __init__(self, settings_page_widget):
        self.settings_page = settings_page_widget
        self.setup_ui_connections()
        # Bangumi认证
        thread_manager.auth_success.connect(self.on_auth_success)
        thread_manager.auth_error.connect(self.on_auth_error)
        thread_manager.collection_ready.connect(self.on_collection_data_ready)
        thread_manager.collection_error.connect(self.on_collection_error)
        # PikPak登录
        thread_manager.pikpak_login_success.connect(self.on_pikpak_login_success)
        thread_manager.pikpak_login_error.connect(self.on_pikpak_login_error)
        self.update_token_display()
        self.setup_sidebar_connections()
        # 默认选中
        self.set_default_page()

    def set_default_page(self):
        """默认页面Bangumi"""
        self.settings_page.ui.pushButton_3.setChecked(True)
        self.settings_page.ui.stackedWidget_2.setCurrentWidget(self.settings_page.ui.login_page)

    def setup_ui_connections(self):
        """设置连接"""
        self.settings_page.ui.login_Button.clicked.connect(self.login_action)
        self.settings_page.ui.collection_Button.clicked.connect(self.get_collections_action)
        self.settings_page.ui.login_Button_2.clicked.connect(self.pikpak_login_action)

    def setup_sidebar_connections(self):
        """设置侧边栏按钮连接"""
        # Bangumi
        self.settings_page.ui.pushButton_3.clicked.connect(lambda: self.settings_page.ui.stackedWidget_2.setCurrentWidget(self.settings_page.ui.login_page))
        # PikPak
        self.settings_page.ui.pushButton_4.clicked.connect(lambda: self.settings_page.ui.stackedWidget_2.setCurrentWidget(self.settings_page.ui.pikpak_page))

    def update_token_display(self):
        """显示token"""
        self.settings_page.ui.pushButton_2.setText(f"ID: {get_config_item('user_id')}")
        self.settings_page.ui.pushButton_9.setText(f"Access Token: {get_config_item('access_token')}")
        self.settings_page.ui.pushButton_8.setText(f"Refresh Token: {get_config_item('refresh_token')}")
        self.settings_page.ui.pushButton_6.setText(f"username: {get_config_item('username')}")
        self.settings_page.ui.pushButton_10.setText(f"password: {get_config_item('password')}")
        self.settings_page.ui.pushButton_11.setText(f"encoded token: {get_config_item('encoded_token')[:30]}")

    # ==========Bangumi==========
    def show_client_credential_dialog(self):
        dialog = QDialog(self.settings_page)
        dialog.setWindowTitle("Bangumi授权")
        dialog.setFixedSize(400, 300)
        layout = QVBoxLayout()
        link_label = QLabel('前往创建(需登录bangumi):<br>''<a href="https://bangumi.tv/dev/app">https://bangumi.tv/dev/app</a><br>''<a href="https://bgm.tv/dev/app">https://bgm.tv/dev/app</a><br>''<a href="https://chii.in/dev/app">https://chii.in/dev/app</a>')
        link_label.setOpenExternalLinks(True)
        layout.addWidget(link_label)
        layout.addWidget(QLabel("Client ID:"))
        client_id_input = QLineEdit()
        client_id_input.setPlaceholderText("输入 App ID")
        layout.addWidget(client_id_input)
        layout.addWidget(QLabel("Client Secret:"))
        client_secret_input = QLineEdit()
        client_secret_input.setPlaceholderText("输入 App Secret")
        layout.addWidget(client_secret_input)
        layout.addWidget(QLabel("Redirect URI:"))
        redirect_uri_input = QLineEdit()
        redirect_uri_input.setPlaceholderText("输入 回调地址")
        layout.addWidget(redirect_uri_input)
        confirm_button = QPushButton("授权")
        confirm_button.clicked.connect(dialog.accept)
        layout.addWidget(confirm_button)
        dialog.setLayout(layout)
        if dialog.exec():
            client_id = client_id_input.text().strip()
            client_secret = client_secret_input.text().strip()
            redirect_uri = redirect_uri_input.text().strip()
            return client_id, client_secret, redirect_uri
        return None, None, None

    def get_client_credentials(self):
        """Bangumi授权信息缺失提示"""
        required_fields = ["client_id", "client_secret", "redirect_uri"]
        for field in required_fields:
            if not get_config_item(field, ""):
                client_id, client_secret, redirect_uri = self.show_client_credential_dialog()
                if client_id is None or client_secret is None or redirect_uri is None:
                    return False
                if not all([client_id, client_secret, redirect_uri]):
                    QMessageBox.warning(self.settings_page, "警告", "所有字段都必须填写完整！")
                    return False
                set_config_items(client_id=client_id, client_secret=client_secret, redirect_uri=redirect_uri)
                break
        return True

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

    # ==========PikPak==========
    def show_pikpak_login_dialog(self):
        dialog = QDialog(self.settings_page)
        dialog.setWindowTitle("PikPak登录")
        dialog.setFixedSize(400, 200)
        layout = QVBoxLayout()
        layout.addWidget(QLabel("用户名:"))
        username_input = QLineEdit()
        username_input.setPlaceholderText("输入 PikPak 账号")
        layout.addWidget(username_input)
        layout.addWidget(QLabel("密码:"))
        password_input = QLineEdit()
        password_input.setPlaceholderText("输入 PikPak 密码")
        layout.addWidget(password_input)
        confirm_button = QPushButton("登录")
        confirm_button.clicked.connect(dialog.accept)
        layout.addWidget(confirm_button)
        dialog.setLayout(layout)
        if dialog.exec():
            username = username_input.text().strip()
            password = password_input.text().strip()
            return username, password
        return None, None

    def get_pikpak_credentials(self):
        """PikPak授权信息缺失提示"""
        username = get_config_item("username", "")
        password = get_config_item("password", "")
        if not username or not password:
            username, password = self.show_pikpak_login_dialog()
            if username is None or password is None:
                return False
            if not username or not password:
                QMessageBox.warning(self.settings_page, "警告", "用户名和密码都不能为空！")
                return False
            set_config_items(username=username, password=password)
        return True

    def pikpak_login_action(self):
        """PikPak登录"""
        if not self.get_pikpak_credentials():
            return
        username = get_config_item("username")
        password = get_config_item("password")
        thread_manager.login_pikpak(username, password)

    def on_pikpak_login_success(self, encoded_token):
        """PikPak登录成功处理"""
        set_config_items(encoded_token=encoded_token)
        self.update_token_display()
        QMessageBox.information(self.settings_page, "成功", "PikPak登录成功！")

    def on_pikpak_login_error(self, error_msg):
        """PikPak登录错误处理"""
        QMessageBox.warning(self.settings_page, "登录失败", f"登录失败: {error_msg}")