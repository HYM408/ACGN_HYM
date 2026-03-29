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

SettingsPage::SettingsPage(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    bangumiOAuth = new BangumiOAuth(this);
    applyTheme();
    setupConnections();
    setupDownloadPathUi();
    updateTokenDisplay();
    setUiSelection();
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
    const QColor color1 = getColor("color1", 0xfdf7ff);
    const QColor color4 = getColor("color4", 0xe9ddff);
    ui.stackedWidget->setStyleSheet(QString("QFrame {background-color: %1}").arg(color1.name()));
    const QString buttonStyle = QString("QPushButton:hover {background-color: rgba(0, 0, 0, 20)}"
                                        "QPushButton:checked {background-color: %1}"
                                        "QPushButton {border:none; border-radius: 10px}").arg(color4.name());
    for (const auto button : {ui.btnBangumi, ui.btnPikPak, ui.btnDownloadMenu}) button->setStyleSheet(buttonStyle);
}

void SettingsPage::setupConnections()
{   // 设置连接
    ui.btnBangumi->setChecked(true);
    ui.stackedWidget->setCurrentIndex(0);
    // 返回
    connect(ui.btnBack, &QPushButton::clicked, this, &SettingsPage::onBackButtonClicked);
    // Bangumi
    connect(ui.btnBangumi, &QPushButton::clicked, [this] {ui.stackedWidget->setCurrentWidget(ui.bangumiPage);});
    connect(ui.btnBangumiAuth, &QPushButton::clicked, this, &SettingsPage::onLoginButtonClicked);
    connect(ui.btnGetCollection, &QPushButton::clicked, this, &SettingsPage::onCollectionButtonClicked);
    connect(ui.comboBangumiDomain, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SettingsPage::onBangumiUrlChanged);
    connect(ui.btnDownloadPublic, &QPushButton::clicked, this, [this] {downloadPublicDate(false);});
    connect(ui.btnDownloadMirror, &QPushButton::clicked, this, [this] {downloadPublicDate(true);});
    connect(ui.checkBoxNsfw, &QCheckBox::toggled, this, [this](const bool checked) {
        setConfig("Bangumi/nsfw", checked);
        emit nsfwSettingChanged(checked);
    });
    // PikPak
    connect(ui.btnPikPak, &QPushButton::clicked, [this] {ui.stackedWidget->setCurrentWidget(ui.pikpakPage);});
    connect(ui.btnPikPakLogin, &QPushButton::clicked, this, &SettingsPage::onPikPakLoginButtonClicked);
    // 下载
    setupLineEditCustomContextMenu(ui.lineEditDownloadPath, CMO_Copy | CMO_SelectAll);
    connect(ui.btnDownloadMenu, &QPushButton::clicked, [this] {ui.stackedWidget->setCurrentWidget(ui.downloadPage);});
    connect(ui.btnSelectDownloadPath, &QPushButton::clicked, this, &SettingsPage::onSelectDownloadPath);
}

void SettingsPage::updateTokenDisplay() const
{   // 显示token
    ui.labelUserId->setText("ID: " + getConfig("Bangumi/user_id").toString());
    ui.labelAccessToken->setText("Access Token: " + getConfig("Bangumi/access_token").toString());
    ui.labelRefreshToken->setText("Refresh Token: " + getConfig("Bangumi/refresh_token").toString());
    ui.labelPikPakUsername->setText("username: " + getConfig("PikPak/username").toString());
    ui.labelPikPakPassword->setText("password: " + getConfig("PikPak/password").toString());
    ui.labelPikPakAccessToken->setText("access token: " + getConfig("PikPak/access_token").toString().left(100));
    ui.labelPikPakRefreshToken->setText("refresh token: " + getConfig("PikPak/refresh_token").toString());
}

void SettingsPage::setUiSelection()
{   // 设置ui
    const QMap<QString, int> urlToIndex = {{"https://bangumi.tv/", 0}, {"https://bgm.tv/", 1}, {"https://chii.in/", 2}};
    ui.comboBangumiDomain->setCurrentIndex(urlToIndex.value(getConfig("Bangumi/bangumi_base_url").toString()));
    int mask = getConfig("Bangumi/bangumi_public_date_type").toInt();
    ui.checkBoxBook->setChecked(mask & 1);
    ui.checkBoxAnime->setChecked(mask & 2);
    ui.checkBoxGame->setChecked(mask & 4);
    ui.checkBoxNsfw->setChecked(getConfig("Bangumi/nsfw").toBool());
}

void SettingsPage::onBangumiUrlChanged(const int index)
{   // 改变Bangumi域名
    QMap<int, QString> indexToUrl = {{0, "https://bangumi.tv/"}, {1, "https://bgm.tv/"}, {2, "https://chii.in/"}};
    setConfig("Bangumi/bangumi_base_url", indexToUrl[index]);
}

void SettingsPage::onLoginButtonClicked()
{   // 点击Bangumi授权
    if (!ensureBangumiCredentials()) return;
    ui.btnBangumiAuth->setText("授权中...");
    ui.btnBangumiAuth->setEnabled(false);
    QDesktopServices::openUrl(bangumiOAuth->generateAuthUrl());
    const QString code = bangumiOAuth->listenForCode(30);
    if (code.isEmpty()) {
        ui.btnBangumiAuth->setText("Code获取失败");
        return;
    }
    if (bangumiOAuth->exchangeCodeForToken(code, "")) {
        updateTokenDisplay();
        ui.btnBangumiAuth->setText("Bangumi授权成功！");
    } else ui.btnBangumiAuth->setText("Token交换失败");
    ui.btnBangumiAuth->setEnabled(true);
}

bool SettingsPage::ensureBangumiCredentials()
{   // Bangumi 授权凭证
    QDialog dialog(this);
    dialog.setWindowTitle("Bangumi授权");
    dialog.setFixedSize(400, 300);
    const auto layout = new QVBoxLayout(&dialog);
    const auto linkLabel = new QLabel("前往创建应用(需登录bangumi):<a href=\"https://bgm.tv/dev/app\">https://bgm.tv/dev/app</a>");
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
    ui.btnGetCollection->setEnabled(false);
    ui.btnGetCollection->setText("进度：0");
    bangumiAPI->getUserCollections(true, 3, [this](const int current, const int total) {
        ui.btnGetCollection->setText(QString("进度：%1/%2").arg(current).arg(total));
    }, [this](const QJsonArray &collections, const QString &error) {
        if (!error.isEmpty()) ui.btnGetCollection->setText("获取收藏失败: " + error);
        else if (!collections.isEmpty()) {
            dbManager->clearCollectionTable();
            if (DatabaseManager::insertManyCollectionData(collections)) ui.btnGetCollection->setText(QString("获取完成，共%1条").arg(collections.size()));
            else ui.btnGetCollection->setText("保存收藏到数据库失败");
        } else ui.btnGetCollection->setText("获取收藏失败");
        ui.btnGetCollection->setEnabled(true);
    });
}

void SettingsPage::downloadPublicDate(const bool useMirror)
{   // 下载Bangumi公共数据
    int mask = 0;
    if (ui.checkBoxBook->isChecked()) mask |= 1;
    if (ui.checkBoxAnime->isChecked()) mask |= 2;
    if (ui.checkBoxGame->isChecked()) mask |= 4;
    setConfig("Bangumi/bangumi_public_date_type", mask);
    QList<int> selectedTypes;
    if (ui.checkBoxAnime->isChecked()) selectedTypes.append(2);
    if (ui.checkBoxBook->isChecked()) selectedTypes.append(1);
    if (ui.checkBoxGame->isChecked()) selectedTypes.append(4);
    if (selectedTypes.isEmpty()) return ui.labelDownloadStatus->setText("请至少选择一种类型");
    if (m_currentDownload) clearDownloadTasks(true);
    ui.labelDownloadStatus->setText("获取下载链接");
    static const QString bangumiArchiveUrl = "https://raw.githubusercontent.com/HYM408/BangumiDateProcessing/refs/heads/main/latest.json";
    const QString url = useMirror ? "https://hk.gh-proxy.org/" + bangumiArchiveUrl : bangumiArchiveUrl;
    const QNetworkRequest request(url);
    sendRequestJson(m_networkManager, request, "GET", QByteArray(), 2, [this, useMirror, selectedTypes](const QJsonObject &json, int, const QString &error) {
        if (!error.isEmpty() || json.isEmpty()) return ui.labelDownloadStatus->setText("错误：无法获取下载链接");
        QStringList typeStrs;
        for (const int type : selectedTypes) typeStrs << QString::number(type);
        typeStrs.sort();
        const QString key = "public_date_" + typeStrs.join("");
        const QString downloadUrl = json[key].toString();
        const QString targetDir = QDir::currentPath() + "/data";
        const QString targetPath = targetDir + "/public_date.db";
        const QString tempPath   = targetDir + "/public_date.db.tmp";
        const QDir dir(targetDir);
        if (!dir.exists()) (void)dir.mkpath(".");
        const QString finalDownloadUrl = useMirror ? "https://hk.gh-proxy.org/" + downloadUrl : downloadUrl;
        m_currentDownload = new ChunkDownload(finalDownloadUrl, tempPath, 4, this);
        connect(m_currentDownload, &ChunkDownload::progressChanged, this, [this](const int percent, qint64, qint64) {ui.labelDownloadStatus->setText(QString("下载中 %1%").arg(percent));});
        connect(m_currentDownload, &ChunkDownload::statusChanged, this, [this, targetPath, tempPath](const QString &status) {
            if (status == "完成") {
                dbManager->closePublicDatabase();
                QFile::remove(targetPath);
                QFile tempFile(tempPath);
                tempFile.rename(targetPath);
                dbManager->openDatabase();
                ui.labelDownloadStatus->setText("完成");
            } else if (status == "错误") {
                ui.labelDownloadStatus->setText("失败");
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
    ui.btnPikPakLogin->setText("登录中...");
    ui.btnPikPakLogin->setEnabled(false);
    pikpakApi->loginPikPak([this](const bool success, const QString &error) {
        if (success) {
            updateTokenDisplay();
            ui.btnPikPakLogin->setText("PikPak 登录成功！");
        } else ui.btnPikPakLogin->setText("PikPak 登录失败:" + error);
        ui.btnPikPakLogin->setEnabled(true);
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
    ui.lineEditDownloadPath->setText(downloadPath);
}

void SettingsPage::onSelectDownloadPath()
{   // 选择下载路径
    const QString currentPath = ui.lineEditDownloadPath->text();
    const QString dirPath = QFileDialog::getExistingDirectory(this, "选择下载路径", currentPath);
    if (!dirPath.isEmpty()) {
        ui.lineEditDownloadPath->setText(dirPath);
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
    ui.btnBangumi->setChecked(true);
    ui.stackedWidget->setCurrentIndex(0);
    ui.btnBangumiAuth->setText("开始授权");
    ui.btnGetCollection->setText("获取收藏");
    ui.btnPikPakLogin->setText("开始登录");
    emit backButtonClicked();
}