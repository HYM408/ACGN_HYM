#include "window.h"
#include <QTimer>
#include "sql.h"
#include "rss.h"
#include "config.h"
#include "detail_page.h"
#include "search_page.h"
#include "episode_page.h"
#include "settings_page.h"
#include "api/pikpak_api.h"
#include "api/bangumi_api.h"
#include "player/player_page.h"
#include "utils/cache_image_util.h"
#include "downloader/download_page.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    // 初始化配置文件
    initConfig();
    // 初始化管理器
    bangumiAPI = new BangumiAPI(this);
    dbManager = new DatabaseManager(this);
    cacheImageUtil = new CacheImageUtil(this);
    pikpakApi = new PikPakApi(this);
    dbManager->openDatabase();
    DatabaseManager::initTables();
    // 启动 RSS
    QTimer::singleShot(5000, this, [this] {startRSS(bangumiAPI);});
    // 设置主页面
    setupUi(this);
    mainPageManager = new MainPageManager(this, cacheImageUtil, bangumiAPI, dbManager);
    // 标题栏
    setWindowFlags(Qt::FramelessWindowHint);
    titlebar_frame->installEventFilter(this);
    // 连接信号
    setupConnections();
}

MainWindow::~MainWindow()
{
    delete searchPage;
    delete settingsPage;
    delete downloadPage;
    delete detailPage;
    delete playerPage;
    delete episodeOverlay;
    delete mainPageManager;
    cacheImageUtil->clearPendingDownloads();
    delete cacheImageUtil;
    delete bangumiAPI;
    delete pikpakApi;
    delete dbManager;
}

void MainWindow::setupConnections()
{   // 连接
    // 标题栏
    connect(pushButton, &QPushButton::clicked, this, &MainWindow::minimizeWindow);
    connect(pushButton_8, &QPushButton::clicked, this, &MainWindow::toggleMaximizeWindow);
    connect(pushButton_2, &QPushButton::clicked, this, &MainWindow::close);
    // 搜索
    connect(search_Button, &QPushButton::clicked, this, &MainWindow::onSearchButtonClicked);
    // 设置
    connect(settings_Button, &QPushButton::clicked, this, &MainWindow::onSettingsButtonClicked);
    // 下载
    connect(pushButton_9, &QPushButton::clicked, this, &MainWindow::onDownloadButtonClicked);
    connect(mainPageManager, &MainPageManager::showMainPageRequested, this, [this] {showmain_stackedWidget->setCurrentIndex(0);});
    // 详情
    connect(mainPageManager, &MainPageManager::showDetailPageRequested, this, &MainWindow::onShowDetailPageRequested);
    // 选集
    connect(mainPageManager, &MainPageManager::showEpisodePageRequested, this, &MainWindow::onShowEpisodePageRequested);
}

void MainWindow::ensureSearchPage()
{
    searchPage = new SearchPage();
    searchPage->setManagers(cacheImageUtil, bangumiAPI);
    main_stackedWidget->addWidget(searchPage);
    connect(searchPage, &SearchPage::backButtonClicked, this, &MainWindow::onBackButtonClicked);
    connect(searchPage, &SearchPage::showDetailPage, this, &MainWindow::onShowDetailPageFromSearch);
}

void MainWindow::ensureDetailPage()
{
    detailPage = new DetailPage();
    detailPage->setManagers(cacheImageUtil, bangumiAPI);
    main_stackedWidget->addWidget(detailPage);
    connect(detailPage, &DetailPage::backButtonClicked, this, &MainWindow::onBackButtonClicked);
    connect(detailPage, &DetailPage::showEpisodePageRequested, this, &MainWindow::onShowEpisodePageRequested);
    connect(detailPage, &DetailPage::tagClicked, this, &MainWindow::onTagClicked);
}

void MainWindow::minimizeWindow()
{   // 最小化
    showMinimized();
}

void MainWindow::toggleMaximizeWindow()
{   // 切换最大化/还原窗口
    isMaximized() ? showNormal() : showMaximized();
}

void MainWindow::closeEvent(QCloseEvent *event)
{   // 关闭程序
    if (playerPage) playerPage->cancelAllSearches();
    QMainWindow::closeEvent(event);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{   // 标题栏事件
    if (watched == titlebar_frame) {
        auto *mouseEvent = dynamic_cast<QMouseEvent*>(event);
        if (event->type() == QEvent::MouseButtonDblClick) {
            toggleMaximizeWindow();
            return true;
        }
        if (isMaximized() && (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseMove || event->type() == QEvent::MouseButtonRelease)) return false;
        if (event->type() == QEvent::MouseButtonPress) {
            dragPosition = mouseEvent->globalPosition().toPoint() - frameGeometry().topLeft();
            isDragging = true;
            return true;
        }
        if (event->type() == QEvent::MouseMove && isDragging) {
            move(mouseEvent->globalPosition().toPoint() - dragPosition);
            return true;
        }
        if (event->type() == QEvent::MouseButtonRelease) isDragging = false;
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::onSearchButtonClicked()
{   // 切换到搜索页面
    if (!searchPage) ensureSearchPage();
    searchPage->updateComboBoxByType(mainPageManager->getCurrentSubjectType());
    main_stackedWidget->setCurrentWidget(searchPage);
}

void MainWindow::onTagClicked(const QString &tag)
{   // tag搜索
    if (!searchPage) ensureSearchPage();
    pageHistory.append(main_stackedWidget->currentWidget());
    main_stackedWidget->setCurrentWidget(searchPage);
    searchPage->searchByTag(tag);
}

void MainWindow::onSettingsButtonClicked()
{   // 切换到设置页面
    if (!settingsPage) {
        settingsPage = new SettingsPage(this);
        settingsPage->setManagers(bangumiAPI, pikpakApi);
        main_stackedWidget->addWidget(settingsPage);
        connect(settingsPage, &SettingsPage::backButtonClicked, this, &MainWindow::onBackButtonClicked);
    }
    main_stackedWidget->setCurrentWidget(settingsPage);
}

void MainWindow::onShowDetailPageRequested(const CollectionData &collectionData)
{   // 主页面 to 详情页面
    if (!detailPage) ensureDetailPage();
    detailPage->setCollectionData(collectionData);
    main_stackedWidget->setCurrentWidget(detailPage);
}

void MainWindow::onShowDetailPageFromSearch(const QVariantMap &data)
{   // 搜索页面 to 详情页面
    if (!detailPage) ensureDetailPage();
    pageHistory.append(main_stackedWidget->currentWidget());
    detailPage->setCollectionDataFromMap(data);
    main_stackedWidget->setCurrentWidget(detailPage);
}

void MainWindow::onDownloadButtonClicked()
{   // 切换到下载页面
    if (!downloadPage) {
        downloadPage = new DownloadPage(this);
        downloadPage->setManagers(pikpakApi);
        showmain_stackedWidget->addWidget(downloadPage);
    }
    showmain_stackedWidget->setCurrentIndex(1);
    downloadPage->loadRecentFiles();
}

void MainWindow::onShowEpisodePageRequested(const CollectionData &collectionData)
{   // 创建选集遮罩层
    episodeOverlay = new EpisodeOverlay(this);
    episodeOverlay->setManagers(bangumiAPI);
    connect(episodeOverlay, &EpisodeOverlay::episodeClicked, this, &MainWindow::onEpisodeClicked);
    connect(episodeOverlay, &EpisodeOverlay::overlayClosed, this, [this] {
        episodeOverlay->deleteLater();
        episodeOverlay = nullptr;
    });
    episodeOverlay->setGeometry(0, 0, width(), height());
    episodeOverlay->showWithCollectionData(collectionData);
    if (playerPage) return;
    if (collectionData.subject_type != 2) return;
    QTimer::singleShot(1, this, &MainWindow::precreatePlayerPage);
}

void MainWindow::precreatePlayerPage()
{   // 播放器页面预创建
    playerPage = new PlayerPage(this);
    playerPage->setManagers(cacheImageUtil, pikpakApi);
    main_stackedWidget->addWidget(playerPage);
    connect(playerPage, &PlayerPage::backButtonClicked, this, &MainWindow::onBackButtonClicked);
}

void MainWindow::onEpisodeClicked(const QJsonObject &collectionData, const QJsonObject &episodeData)
{   // 切换播放器页面
    pageHistory.append(main_stackedWidget->currentWidget());
    playerPage->fetchRoutes(collectionData, episodeData);
    main_stackedWidget->setCurrentWidget(playerPage);
}

void MainWindow::onBackButtonClicked()
{   // 返回历史页面
    if (!pageHistory.isEmpty()) {
        QWidget *prevPage = pageHistory.takeLast();
        main_stackedWidget->setCurrentWidget(prevPage);
    } else {
        main_stackedWidget->setCurrentIndex(0);
        QTimer::singleShot(100, this, [this]{mainPageManager->loadCollections(mainPageManager->getCurrentSubjectType(), mainPageManager->getCurrentStatusType(), false);});
    }
}