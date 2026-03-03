#include "settings_page.h"
#include <QTimer>
#include <QLabel>
#include <QJsonArray>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include "sql.h"
#include "config.h"
#include "api/pikpak_api.h"
#include "api/bangumi_api.h"
#include "api/bangumi_oauth.h"

SettingsPage::SettingsPage(QWidget *parent) : QWidget(parent)
{
    // 初始化UI
    ui.setupUi(this);
    // 初始化Bangumi OAuth
    bangumiOAuth = new BangumiOAuth(this);
    // 设置UI连接
    setupConnections();
    // 设置下载路径
    setupDownloadPathUi();
    // 更新token显示
    updateTokenDisplay();
    // Bangumi域名
    setBangumiBaseUrl();
}

SettingsPage::~SettingsPage()
{
    delete bangumiOAuth;
}

void SettingsPage::setManagers(BangumiAPI *api, PikPakApi *pikpakapi, DatabaseManager *db)
{   // 初始化实例
    bangumiAPI = api;
    pikpakApi = pikpakapi;
    dbManager = db;
}

void SettingsPage::setupConnections()
{   // 设置连接
    // 返回
    connect(ui.back_Button_2, &QPushButton::clicked, this, &SettingsPage::onBackButtonClicked);
    // Bangumi
    connect(ui.pushButton_3, &QPushButton::clicked, [this] {ui.stackedWidget_2->setCurrentWidget(ui.login_page);});
    ui.pushButton_3->setChecked(true);
    ui.stackedWidget_2->setCurrentWidget(ui.login_page);
    connect(ui.login_Button, &QPushButton::clicked, this, &SettingsPage::onLoginButtonClicked);
    connect(ui.collection_Button, &QPushButton::clicked, this, &SettingsPage::onCollectionButtonClicked);
    connect(ui.comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SettingsPage::onBangumiUrlChanged);
    // PikPak
    connect(ui.pushButton_4, &QPushButton::clicked, [this]() {ui.stackedWidget_2->setCurrentWidget(ui.pikpak_page);});
    connect(ui.login_Button_2, &QPushButton::clicked, this, &SettingsPage::onPikPakLoginButtonClicked);
    // 下载
    connect(ui.pushButton_13, &QPushButton::clicked, [this]() {ui.stackedWidget_2->setCurrentWidget(ui.download_page);});
    connect(ui.login_Button_3, &QPushButton::clicked, this, &SettingsPage::onSelectDownloadPath);
}

void SettingsPage::setupDownloadPathUi() const
{   // 设置下载路径UI
    QString downloadPath = getConfig("Download/download_path", "data/download").toString();
    ui.lineEdit->setText(downloadPath);
}

void SettingsPage::onSelectDownloadPath()
{   // 选择下载路径
    QString currentPath = ui.lineEdit->text();
    QString dirPath = QFileDialog::getExistingDirectory(this, "选择下载路径", currentPath);
    if (!dirPath.isEmpty()) {
        ui.lineEdit->setText(dirPath);
        setConfig("Download/download_path", dirPath);
    }
}

void SettingsPage::updateTokenDisplay() const
{   // 显示token
    ui.pushButton_2->setText("ID: " + getConfig("Bangumi/user_id").toString());
    ui.pushButton_9->setText("Access Token: " + getConfig("Bangumi/access_token").toString());
    ui.pushButton_8->setText("Refresh Token: " + getConfig("Bangumi/refresh_token").toString());
    ui.pushButton_6->setText("username: " + getConfig("PikPak/username").toString());
    ui.pushButton_10->setText("password: " + getConfig("PikPak/password").toString());
    ui.pushButton_11->setText("access token: " + getConfig("PikPak/access_token").toString().left(100));
    ui.pushButton_15->setText("refresh token: " + getConfig("PikPak/refresh_token").toString());
}

void SettingsPage::setBangumiBaseUrl() const
{   // 设置Bangumi域名
    QMap<QString, int> urlToIndex = {{"https://bangumi.tv/", 0}, {"https://bgm.tv/", 1}, {"https://chii.in/", 2}};
    ui.comboBox->setCurrentIndex(urlToIndex.value(getConfig("Bangumi/bangumi_base_url").toString()));
}

void SettingsPage::onBangumiUrlChanged(int index)
{   // 改变Bangumi域名
    QMap<int, QString> indexToUrl = {{0, "https://bangumi.tv/"}, {1, "https://bgm.tv/"}, {2, "https://chii.in/"}};
    setConfig("Bangumi/bangumi_base_url", indexToUrl[index]);
}

void SettingsPage::onLoginButtonClicked()
{   // 点击Bangumi授权
    if (getConfig("Bangumi/client_id").toString().isEmpty() || getConfig("Bangumi/client_secret").toString().isEmpty() || getConfig("Bangumi/redirect_uri").toString().isEmpty()) ensureBangumiCredentials();
    loginBangumi();
}

void SettingsPage::ensureBangumiCredentials()
{   // Bangumi 授权凭证
    QDialog dialog(this);
    dialog.setWindowTitle("Bangumi授权");
    dialog.setFixedSize(400, 300);
    auto layout = new QVBoxLayout(&dialog);
    auto linkLabel = new QLabel(
        "前往创建(需登录bangumi):<br>"
        "<a href=\"https://bangumi.tv/dev/app\">https://bangumi.tv/dev/app</a><br>"
        "<a href=\"https://bgm.tv/dev/app\">https://bgm.tv/dev/app</a><br>"
        "<a href=\"https://chii.in/dev/app\">https://chii.in/dev/app</a>");
    linkLabel->setOpenExternalLinks(true);
    layout->addWidget(linkLabel);
    layout->addWidget(new QLabel("Client ID:"));
    auto clientIdInput = new QLineEdit();
    clientIdInput->setPlaceholderText("输入 App ID");
    layout->addWidget(clientIdInput);
    layout->addWidget(new QLabel("Client Secret:"));
    auto clientSecretInput = new QLineEdit();
    clientSecretInput->setPlaceholderText("输入 App Secret");
    layout->addWidget(clientSecretInput);
    layout->addWidget(new QLabel("Redirect URI:"));
    auto redirectUriInput = new QLineEdit();
    redirectUriInput->setPlaceholderText("输入 回调地址");
    layout->addWidget(redirectUriInput);
    auto confirmButton = new QPushButton("授权");
    layout->addWidget(confirmButton);
    connect(confirmButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    if (dialog.exec() == QDialog::Accepted) {
        QString clientId = clientIdInput->text().trimmed();
        QString clientSecret = clientSecretInput->text().trimmed();
        QString redirectUri = redirectUriInput->text().trimmed();
        if (!clientId.isEmpty() && !clientSecret.isEmpty() && !redirectUri.isEmpty()) {
            setConfig("Bangumi/client_id", clientId);
            setConfig("Bangumi/client_secret", clientSecret);
            setConfig("Bangumi/redirect_uri", redirectUri);
        } else QMessageBox::warning(this, "警告", "所有字段都必须填写完整！");
    }
}

void SettingsPage::loginBangumi()
{   // Bangumi 授权
    QString authUrl = bangumiOAuth->generateAuthUrl();
    QDesktopServices::openUrl(QUrl(authUrl));
    QString code = bangumiOAuth->listenForCode(60);
    if (code.isEmpty()) {
        QMessageBox::warning(this, "错误", "授权失败: 授权失败或超时");
        return;
    }
    if (bangumiOAuth->exchangeCodeForToken(code, "")) {
        updateTokenDisplay();
        QMessageBox::information(this, "成功", "Bangumi授权成功！");
    } else QMessageBox::warning(this, "错误", "授权失败: Token交换失败");
}

void SettingsPage::onCollectionButtonClicked()
{   // 获取Bangumi收藏
    QJsonArray collections = bangumiAPI->getUserCollections(true, 3);
    if (!collections.isEmpty()) {
        dbManager->clearCollectionTable();
        if (DatabaseManager::insertManyCollectionData(collections)) QMessageBox::information(this, "成功", QString("获取Bangumi收藏完成，共%1条记录").arg(collections.size()));
        else QMessageBox::warning(this, "错误", "保存收藏到数据库失败");
    } else QMessageBox::warning(this, "错误", "获取收藏失败");
}

void SettingsPage::onPikPakLoginButtonClicked()
{   // 点击PikPak登录
    if (getConfig("PikPak/username").toString().isEmpty() || getConfig("PikPak/password").toString().isEmpty()) ensurePikPakCredentials();
    if (pikpakApi->loginPikPak()) {
        updateTokenDisplay();
        QMessageBox::information(this, "成功", "PikPak 登录成功！");
    } else QMessageBox::warning(this, "错误", "PikPak 登录失败");
}

void SettingsPage::ensurePikPakCredentials()
{   // PikPak登录凭证
    QDialog dialog(this);
    dialog.setWindowTitle("PikPak登录(ip不能在中国大陆)");
    dialog.setFixedSize(400, 200);
    auto layout = new QVBoxLayout(&dialog);
    auto infoLabel = new QLabel("请输入PikPak账号和密码（IP不能在中国大陆）");
    infoLabel->setWordWrap(true);
    layout->addWidget(infoLabel);
    layout->addWidget(new QLabel("用户名:"));
    auto userInput = new QLineEdit();
    userInput->setPlaceholderText("输入 PikPak 账号");
    layout->addWidget(userInput);
    layout->addWidget(new QLabel("密码:"));
    auto passInput = new QLineEdit();
    passInput->setPlaceholderText("输入 PikPak 密码");
    layout->addWidget(passInput);
    auto confirmButton = new QPushButton("登录");
    layout->addWidget(confirmButton);
    connect(confirmButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    if (dialog.exec() == QDialog::Accepted) {
        QString newUsername = userInput->text().trimmed();
        QString newPassword = passInput->text().trimmed();
        if (!newUsername.isEmpty() && !newPassword.isEmpty()) {
            setConfig("PikPak/username", newUsername);
            setConfig("PikPak/password", newPassword);
        } else QMessageBox::warning(this, "警告", "所有字段都必须填写完整！");
    }
}

void SettingsPage::onBackButtonClicked()
{   // 返回
    emit backButtonClicked();
}