#ifndef SETTINGS_PAGE_H
#define SETTINGS_PAGE_H

#include "settings_ui.h"

class PikPakApi;
class BangumiAPI;
class BangumiOAuth;
class DatabaseManager;

class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(QWidget *parent = nullptr);
    ~SettingsPage() override;
    void setManagers(BangumiAPI *api, PikPakApi *pikpakapi, DatabaseManager *db);

signals:
    void backButtonClicked();

private slots:
    void onLoginButtonClicked();
    void onCollectionButtonClicked();
    static void onBangumiUrlChanged(int index);
    void onSelectDownloadPath();
    void onBackButtonClicked();
    void onPikPakLoginButtonClicked();

private:
    void setupConnections();
    void setupDownloadPathUi() const;
    void updateTokenDisplay() const;
    void setBangumiBaseUrl() const;
    void ensureBangumiCredentials();
    void loginBangumi();
    void ensurePikPakCredentials();
    Ui::SettingsPage ui{};
    BangumiAPI *bangumiAPI = nullptr;
    BangumiOAuth *bangumiOAuth = nullptr;
    DatabaseManager *dbManager = nullptr;
    PikPakApi *pikpakApi = nullptr;
};

#endif // SETTINGS_PAGE_H