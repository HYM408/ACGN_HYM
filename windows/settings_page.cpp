#include "settings_page.h"
#include <QJsonArray>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include "config.h"
#include "sql/sql.h"
#include "api/pikpak_api.h"
#include "api/bangumi_api.h"
#include "api/bangumi_oauth.h"
#include "utils/network_util.h"
#include "utils/context_menu_util.h"
#include "downloader/chunk_download.h"

const QString SettingsPage::BANGUMI_ARCHIVE_URL = "https://raw.githubusercontent.com/HYM408/BangumiDateProcessing/refs/heads/main/latest.json";

SettingsPage::SettingsPage(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    bangumiOAuth = new BangumiOAuth(this);
    applyTheme();
    setupConnections();
    setupDownloadPathUi();
    updateTokenDisplay();
    setBangumiBaseUrl();
}

SettingsPage::~SettingsPage()
{
    if (m_currentDownload) clearDownloadTasks(true);
    delete bangumiOAuth;
}

void SettingsPage::setManagers(BangumiAPI *api, PikPakApi *pikpakapi, DatabaseManager *db)
{   // 初始化实例
    bangumiAPI = api;
    pikpakApi = pikpakapi;
    dbManager = db;
}

void SettingsPage::applyTheme() const
{   // 主题
    const QColor color1 = getColor("color1", QColor("#fdf7ff"));
    const QColor color4 = getColor("color4", QColor("#e9ddff"));
    ui.stackedWidget_2->setStyleSheet(QString("QFrame {background-color: %1}").arg(color1.name()));
    const QString buttonStyle = QString("QPushButton:hover {background-color: rgba(0, 0, 0, 20)}"
                                        "QPushButton:checked {background-color: %1}"
                                        "QPushButton {border:none; border-radius: 10px}").arg(color4.name());
    for (const auto button : {ui.pushButton_3, ui.pushButton_4, ui.pushButton_13}) button->setStyleSheet(buttonStyle);
}

void SettingsPage::setupConnections()
{   // 设置连接
    ui.pushButton_3->setChecked(true);
    ui.stackedWidget_2->setCurrentIndex(0);
    // 返回
    connect(ui.back_Button_2, &QPushButton::clicked, this, &SettingsPage::onBackButtonClicked);
    // Bangumi
    connect(ui.pushButton_3, &QPushButton::clicked, [this] {ui.stackedWidget_2->setCurrentWidget(ui.login_page);});
    connect(ui.login_Button, &QPushButton::clicked, this, &SettingsPage::onLoginButtonClicked);
    connect(ui.collection_Button, &QPushButton::clicked, this, &SettingsPage::onCollectionButtonClicked);
    connect(ui.comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SettingsPage::onBangumiUrlChanged);
    connect(ui.pushButton_17, &QPushButton::clicked, this, [this] {downloadPublicDate(false);});
    connect(ui.pushButton_19, &QPushButton::clicked, this, [this] {downloadPublicDate(true);});
    // PikPak
    connect(ui.pushButton_4, &QPushButton::clicked, [this] {ui.stackedWidget_2->setCurrentWidget(ui.pikpak_page);});
    connect(ui.login_Button_2, &QPushButton::clicked, this, &SettingsPage::onPikPakLoginButtonClicked);
    // 下载
    setupLineEditCustomContextMenu(ui.lineEdit, CMO_Copy | CMO_SelectAll);
    connect(ui.pushButton_13, &QPushButton::clicked, [this] {ui.stackedWidget_2->setCurrentWidget(ui.download_page);});
    connect(ui.login_Button_3, &QPushButton::clicked, this, &SettingsPage::onSelectDownloadPath);
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
    const QMap<QString, int> urlToIndex = {{"https://bangumi.tv/", 0}, {"https://bgm.tv/", 1}, {"https://chii.in/", 2}};
    ui.comboBox->setCurrentIndex(urlToIndex.value(getConfig("Bangumi/bangumi_base_url").toString()));
}

void SettingsPage::onBangumiUrlChanged(const int index)
{   // 改变Bangumi域名
    QMap<int, QString> indexToUrl = {{0, "https://bangumi.tv/"}, {1, "https://bgm.tv/"}, {2, "https://chii.in/"}};
    setConfig("Bangumi/bangumi_base_url", indexToUrl[index]);
}

void SettingsPage::onLoginButtonClicked()
{   // 点击Bangumi授权
    if (!ensureBangumiCredentials()) return;
    ui.login_Button->setText("授权中...");
    ui.login_Button->setEnabled(false);
    QDesktopServices::openUrl(bangumiOAuth->generateAuthUrl());
    const QString code = bangumiOAuth->listenForCode(30);
    if (code.isEmpty()) {
        ui.login_Button->setText("Code获取失败");
        return;
    }
    if (bangumiOAuth->exchangeCodeForToken(code, "")) {
        updateTokenDisplay();
        ui.login_Button->setText("Bangumi授权成功！");
    } else ui.login_Button->setText("Token交换失败");
    ui.login_Button->setEnabled(true);
}

bool SettingsPage::ensureBangumiCredentials()
{   // Bangumi 授权凭证
    QDialog dialog(this);
    dialog.setWindowTitle("Bangumi授权");
    dialog.setFixedSize(400, 300);
    const auto layout = new QVBoxLayout(&dialog);
    const auto linkLabel = new QLabel(
        "前往创建(需登录bangumi):<br>"
        "<a href=\"https://bangumi.tv/dev/app\">https://bangumi.tv/dev/app</a><br>"
        "<a href=\"https://bgm.tv/dev/app\">https://bgm.tv/dev/app</a><br>"
        "<a href=\"https://chii.in/dev/app\">https://chii.in/dev/app</a>");
    linkLabel->setOpenExternalLinks(true);
    layout->addWidget(linkLabel);
    layout->addWidget(new QLabel("Client ID:"));
    const auto clientIdInput = new QLineEdit();
    clientIdInput->setPlaceholderText("输入 App ID");
    clientIdInput->setText(getConfig("Bangumi/client_id").toString());
    layout->addWidget(clientIdInput);
    layout->addWidget(new QLabel("Client Secret:"));
    const auto clientSecretInput = new QLineEdit();
    clientSecretInput->setPlaceholderText("输入 App Secret");
    clientSecretInput->setText(getConfig("Bangumi/client_secret").toString());
    layout->addWidget(clientSecretInput);
    layout->addWidget(new QLabel("Redirect URI:"));
    const auto redirectUriInput = new QLineEdit();
    redirectUriInput->setPlaceholderText("输入 回调地址");
    redirectUriInput->setText(getConfig("Bangumi/redirect_uri").toString());
    layout->addWidget(redirectUriInput);
    const auto confirmButton = new QPushButton("授权");
    layout->addWidget(confirmButton);
    connect(confirmButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    if (dialog.exec() == QDialog::Accepted) {
        const QString clientId = clientIdInput->text();
        const QString clientSecret = clientSecretInput->text();
        const QString redirectUri = redirectUriInput->text();
        if (!clientId.isEmpty() && !clientSecret.isEmpty() && !redirectUri.isEmpty()) {
            setConfig("Bangumi/client_id", clientId);
            setConfig("Bangumi/client_secret", clientSecret);
            setConfig("Bangumi/redirect_uri", redirectUri);
            return true;
        }
        QMessageBox::warning(this, "警告", "所有字段都必须填写完整！");
    }
    return false;
}

void SettingsPage::onCollectionButtonClicked() const
{   // 获取Bangumi收藏
    ui.collection_Button->setEnabled(false);
    ui.collection_Button->setText("进度：0");
    bangumiAPI->getUserCollections(true, 3, [this](const int current, const int total) {
        ui.collection_Button->setText(QString("进度：%1/%2").arg(current).arg(total));
    }, [this](const QJsonArray &collections, const QString &error) {
        if (!error.isEmpty()) ui.collection_Button->setText("获取收藏失败: " + error);
        else if (!collections.isEmpty()) {
            dbManager->clearCollectionTable();
            if (DatabaseManager::insertManyCollectionData(collections)) ui.collection_Button->setText(QString("获取完成，共%1条").arg(collections.size()));
            else ui.collection_Button->setText("保存收藏到数据库失败");
        } else ui.collection_Button->setText("获取收藏失败");
        ui.collection_Button->setEnabled(true);
    });
}

void SettingsPage::downloadPublicDate(const bool useMirror)
{   // 下载Bangumi公共数据
    QList<int> selectedTypes;
    if (ui.checkBox->isChecked()) selectedTypes.append(2);
    if (ui.checkBox_2->isChecked()) selectedTypes.append(1);
    if (ui.checkBox_3->isChecked()) selectedTypes.append(4);
    if (selectedTypes.isEmpty()) return ui.pushButton_20->setText("请至少选择一种要导入的类型");
    if (m_currentDownload) clearDownloadTasks(true);
    ui.pushButton_20->setText("获取下载链接");
    const QString url = useMirror ? "https://hk.gh-proxy.org/" + BANGUMI_ARCHIVE_URL : BANGUMI_ARCHIVE_URL;
    const QNetworkRequest request(url);
    sendRequestJson(m_networkManager, request, "GET", QByteArray(), 2, [this, useMirror, selectedTypes](const QJsonObject &json, int, const QString &error) {
        if (!error.isEmpty() || json.isEmpty()) return ui.pushButton_20->setText("错误：无法获取下载链接");
        QStringList typeStrs;
        for (const int type : selectedTypes) typeStrs << QString::number(type);
        typeStrs.sort();
        const QString key = "public_date_" + typeStrs.join("");
        const QString downloadUrl = json[key].toString();
        const QString targetDir = QDir::currentPath() + "/data";
        const QString targetPath = targetDir + "/public_date.db";
        const QString tempPath   = targetDir + "/public_date.db.tmp";
        const QDir dir(targetDir);
        if (!dir.exists()) dir.mkpath(".");
        const QString finalDownloadUrl = useMirror ? "https://hk.gh-proxy.org/" + downloadUrl : downloadUrl;
        m_currentDownload = new ChunkDownload(finalDownloadUrl, tempPath, 4, this);
        connect(m_currentDownload, &ChunkDownload::progressChanged, this, [this](const int percent, qint64, qint64) {ui.pushButton_20->setText(QString("下载中 %1%").arg(percent));});
        connect(m_currentDownload, &ChunkDownload::statusChanged, this, [this, targetPath, tempPath](const QString &status) {
            if (status == "完成") {
                dbManager->closePublicDatabase();
                QFile::remove(targetPath);
                QFile tempFile(tempPath);
                tempFile.rename(targetPath);
                dbManager->openDatabase();
                ui.pushButton_20->setText("完成");
            } else if (status == "错误") {
                ui.pushButton_20->setText("失败");
                QFile::remove(tempPath);
                clearDownloadTasks(false);
            }
        });
        m_currentDownload->start();
    }, nullptr);
}

void SettingsPage::onPikPakLoginButtonClicked()
{   // 点击PikPak登录
    if (!ensurePikPakCredentials()) return;
    ui.login_Button_2->setText("登录中...");
    ui.login_Button_2->setEnabled(false);
    pikpakApi->loginPikPak([this](const bool success, const QString &error) {
        if (success) {
            updateTokenDisplay();
            ui.login_Button_2->setText("PikPak 登录成功！");
        } else ui.login_Button_2->setText("PikPak 登录失败:" + error);
        ui.login_Button_2->setEnabled(true);
    });
}

bool SettingsPage::ensurePikPakCredentials()
{   // PikPak登录凭证
    QDialog dialog(this);
    dialog.setWindowTitle("PikPak登录(ip不能在中国大陆)");
    dialog.setFixedSize(400, 200);
    const auto layout = new QVBoxLayout(&dialog);
    const auto infoLabel = new QLabel("请输入PikPak账号和密码（IP不能在中国大陆）");
    infoLabel->setWordWrap(true);
    layout->addWidget(infoLabel);
    layout->addWidget(new QLabel("用户名:"));
    const auto userInput = new QLineEdit();
    userInput->setPlaceholderText("输入 PikPak 账号");
    userInput->setText(getConfig("PikPak/username").toString());
    layout->addWidget(userInput);
    layout->addWidget(new QLabel("密码:"));
    const auto passInput = new QLineEdit();
    passInput->setPlaceholderText("输入 PikPak 密码");
    passInput->setText(getConfig("PikPak/password").toString());
    layout->addWidget(passInput);
    const auto confirmButton = new QPushButton("登录");
    layout->addWidget(confirmButton);
    connect(confirmButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    if (dialog.exec() == QDialog::Accepted) {
        const QString newUsername = userInput->text();
        const QString newPassword = passInput->text();
        if (!newUsername.isEmpty() && !newPassword.isEmpty()) {
            setConfig("PikPak/username", newUsername);
            setConfig("PikPak/password", newPassword);
            return true;
        }
        QMessageBox::warning(this, "警告", "所有字段都必须填写完整！");
    }
    return false;
}

void SettingsPage::setupDownloadPathUi() const
{   // 设置下载路径UI
    const QString downloadPath = getConfig("Download/download_path", "data/download").toString();
    ui.lineEdit->setText(downloadPath);
}

void SettingsPage::onSelectDownloadPath()
{   // 选择下载路径
    const QString currentPath = ui.lineEdit->text();
    const QString dirPath = QFileDialog::getExistingDirectory(this, "选择下载路径", currentPath);
    if (!dirPath.isEmpty()) {
        ui.lineEdit->setText(dirPath);
        setConfig("Download/download_path", dirPath);
    }
}

void SettingsPage::clearDownloadTasks(const bool stop)
{   // 清理下载任务
    if (stop) m_currentDownload->stop();
    m_currentDownload->deleteLater();
    m_currentDownload = nullptr;
}

void SettingsPage::onBackButtonClicked()
{   // 返回
    ui.pushButton_3->setChecked(true);
    ui.stackedWidget_2->setCurrentIndex(0);
    ui.login_Button->setText("开始授权");
    ui.collection_Button->setText("获取收藏");
    ui.login_Button_2->setText("开始登录");
    emit backButtonClicked();
}