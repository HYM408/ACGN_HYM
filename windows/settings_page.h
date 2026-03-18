#ifndef SETTINGS_PAGE_H
#define SETTINGS_PAGE_H

#include "settings_ui.h"
#include <QNetworkAccessManager>

class PikPakApi;
class BangumiAPI;
class BangumiOAuth;
class ChunkDownload;
class DatabaseManager;

class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(QWidget *parent = nullptr);
    ~SettingsPage() override;
    void setManagers(BangumiAPI *api, PikPakApi *pikpakapi, DatabaseManager *db);
    void applyTheme() const;

signals:
    void backButtonClicked();

private slots:
    void onLoginButtonClicked();
    void onCollectionButtonClicked() const;
    static void onBangumiUrlChanged(int index);
    void onSelectDownloadPath();
    void clearDownloadTasks(bool stop);
    void onBackButtonClicked();
    void onPikPakLoginButtonClicked();

private:
    void setupConnections();
    void updateTokenDisplay() const;
    void setBangumiBaseUrl() const;
    bool ensureBangumiCredentials();
    void downloadPublicDate(bool useMirror);
    bool ensurePikPakCredentials();
    void setupDownloadPathUi() const;
    Ui::SettingsPage ui{};
    BangumiAPI *bangumiAPI = nullptr;
    BangumiOAuth *bangumiOAuth = nullptr;
    DatabaseManager *dbManager = nullptr;
    PikPakApi *pikpakApi = nullptr;
    ChunkDownload *m_currentDownload = nullptr;
    static const QString BANGUMI_ARCHIVE_URL;
    QNetworkAccessManager m_networkManager;
};

#endif // SETTINGS_PAGE_H