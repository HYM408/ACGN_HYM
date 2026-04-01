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
    void nsfwSettingChanged(bool checked);
    void hotkeyChanged(int keyCode);

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
    void loadBangumiPage() const;
    void loadPikpakPage();
    void loadDownloadPage();
    void loadGamePage();
    void updateBangumiTokenDisplay() const;
    void updatePikpakTokenDisplay() const;
    bool ensureBangumiCredentials();
    void downloadPublicDate(bool useMirror);
    bool ensurePikPakCredentials();
    Ui::SettingsPage ui{};
    BangumiAPI *bangumiAPI = nullptr;
    BangumiOAuth *bangumiOAuth = nullptr;
    DatabaseManager *dbManager = nullptr;
    PikPakApi *pikpakApi = nullptr;
    ChunkDownload *m_currentDownload = nullptr;
    QNetworkAccessManager m_networkManager;
    bool m_pikpakLoaded = false;
    bool m_downloadLoaded = false;
    bool m_gameLoaded = false;
};

#endif // SETTINGS_PAGE_H