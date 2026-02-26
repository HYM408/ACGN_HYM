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
#include "utils/network_util.h"
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
    mainPageManager = new MainPageManager(this, dbManager, cacheImageUtil, bangumiAPI);
    // 设置其他页面
    QTimer::singleShot(0, this, [this] {setupStackedWidget();});
    // 标题栏
    setWindowFlags(Qt::FramelessWindowHint);
    titlebar_frame->installEventFilter(this);
}

MainWindow::~MainWindow()
{   // 清理资源
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

void MainWindow::setupStackedWidget()
{
    // 搜索
    searchPage = new SearchPage();
    searchPage->setManagers(cacheImageUtil, bangumiAPI);
    // 设置
    settingsPage = new SettingsPage();
    settingsPage->setManagers(dbManager, bangumiAPI, pikpakApi);
    // 下载
    downloadPage = new DownloadPage();
    downloadPage->setManagers(pikpakApi);
    // 详情
    detailPage = new DetailPage();
    detailPage->setManagers(dbManager, cacheImageUtil, bangumiAPI);
    // 播放器
    playerPage = new PlayerPage();
    playerPage->setManagers(dbManager, cacheImageUtil, pikpakApi);
    // 添加页面
    main_stackedWidget->addWidget(searchPage);
    main_stackedWidget->addWidget(settingsPage);
    main_stackedWidget->addWidget(detailPage);
    main_stackedWidget->addWidget(playerPage);
    showmain_stackedWidget->addWidget(downloadPage);
    // 连接信号槽
    setupConnections();
}

void MainWindow::setupConnections()
{   // 连接
    // 标题栏
    connect(pushButton, &QPushButton::clicked, this, &MainWindow::minimizeWindow);
    connect(pushButton_8, &QPushButton::clicked, this, &MainWindow::toggleMaximizeWindow);
    connect(pushButton_2, &QPushButton::clicked, this, &MainWindow::onCloseButtonClicked);
    // 搜索
    connect(search_Button, &QPushButton::clicked, this, &MainWindow::onSearchButtonClicked);
    connect(searchPage, &SearchPage::backButtonClicked, this, &MainWindow::onBackButtonClicked);
    // 设置
    connect(settings_Button, &QPushButton::clicked, this, &MainWindow::onSettingsButtonClicked);
    connect(settingsPage, &SettingsPage::backButtonClicked, this, &MainWindow::onBackButtonClicked);
    // 下载
    connect(pushButton_9, &QPushButton::clicked, this, &MainWindow::onDownloadButtonClicked);
    connect(mainPageManager, &MainPageManager::showMainPageRequested, this, [this] {showmain_stackedWidget->setCurrentIndex(0);});
    // 详情
    connect(searchPage, &SearchPage::showDetailPage, this, &MainWindow::onShowDetailPageFromSearch);
    connect(mainPageManager, &MainPageManager::showDetailPageRequested, this, &MainWindow::onShowDetailPageRequested);
    connect(detailPage, &DetailPage::showEpisodePageRequested, this, &MainWindow::onShowEpisodePageRequested);
    connect(detailPage, &DetailPage::tagClicked, this, &MainWindow::onTagClicked);
    connect(detailPage, &DetailPage::backButtonClicked, this, &MainWindow::onBackButtonClicked);
    // 选集
    connect(mainPageManager, &MainPageManager::showEpisodePageRequested, this, &MainWindow::onShowEpisodePageRequested);
    // 播放器
    connect(playerPage, &PlayerPage::backButtonClicked, this, &MainWindow::onBackButtonClicked);
}

void MainWindow::minimizeWindow()
{   // 最小化
    showMinimized();
}

void MainWindow::toggleMaximizeWindow()
{   // 切换最大化/还原窗口
    isMaximized() ? showNormal() : showMaximized();
}

void MainWindow::onCloseButtonClicked()
{   // 关闭程序
    hide();
    abortNetworkRequests();
    QTimer::singleShot(1000, [] {QApplication::quit();});
    QTimer::singleShot(5000, [] {QApplication::quit();});
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{   // 标题栏事件
    if (watched == titlebar_frame) {
        auto *mouseEvent = dynamic_cast<QMouseEvent*>(event);
        if (event->type() == QEvent::MouseButtonDblClick && mouseEvent->button() == Qt::LeftButton) {
            toggleMaximizeWindow();
            return true;
        }
        if (isMaximized() && (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseMove || event->type() == QEvent::MouseButtonRelease)) return false;
        if (event->type() == QEvent::MouseButtonPress && mouseEvent->button() == Qt::LeftButton) {
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

void MainWindow::onSearchButtonClicked() const
{   // 切换到搜索页面
    searchPage->updateComboBoxByType(mainPageManager->getCurrentSubjectType());
    main_stackedWidget->setCurrentIndex(1);
}

void MainWindow::onTagClicked(const QString &tag)
{   // tag搜索
    pageHistory.append(main_stackedWidget->currentIndex());
    main_stackedWidget->setCurrentIndex(1);
    searchPage->searchByTag(tag);
}

void MainWindow::onSettingsButtonClicked() const
{   // 切换到设置页面
    main_stackedWidget->setCurrentIndex(2);
}

void MainWindow::onShowDetailPageRequested(const CollectionData &collectionData) const
{   // 主页面 to 详情页面
    detailPage->setCollectionData(collectionData);
    main_stackedWidget->setCurrentIndex(3);
}

void MainWindow::onShowDetailPageFromSearch(const QVariantMap &data)
{   // 搜索页面 to 详情页面
    pageHistory.append(main_stackedWidget->currentIndex());
    detailPage->setCollectionDataFromMap(data);
    main_stackedWidget->setCurrentIndex(3);
}

void MainWindow::onDownloadButtonClicked() const
{   // 切换到下载页面
    showmain_stackedWidget->setCurrentIndex(1);
    downloadPage->loadRecentFiles();
}

void MainWindow::onShowEpisodePageRequested(const CollectionData &collectionData)
{   // 创建选集遮罩层
    episodeOverlay = new EpisodeOverlay(this);
    episodeOverlay->setManagers(dbManager, bangumiAPI);
    connect(episodeOverlay, &EpisodeOverlay::episodeClicked, this, &MainWindow::onEpisodeClicked);
    connect(episodeOverlay, &EpisodeOverlay::overlayClosed, this, [this] {
        episodeOverlay->deleteLater();
        episodeOverlay = nullptr;
    });
    episodeOverlay->setGeometry(0, 0, width(), height());
    episodeOverlay->showWithCollectionData(collectionData);
}

void MainWindow::onEpisodeClicked(const QJsonObject &collectionData, const QJsonObject &episodeData)
{   // 切换播放器页面
    pageHistory.append(main_stackedWidget->currentIndex());
    playerPage->fetchRoutes(collectionData, episodeData);
    main_stackedWidget->setCurrentIndex(4);
}

void MainWindow::onBackButtonClicked()
{   // 返回历史页面
    if (!pageHistory.isEmpty()) main_stackedWidget->setCurrentIndex(pageHistory.takeLast());
    else {
        main_stackedWidget->setCurrentIndex(0);
        QTimer::singleShot(100, this, [this]() {mainPageManager->loadCollections(mainPageManager->getCurrentSubjectType(), mainPageManager->getCurrentStatusType(), false);});
    }
}