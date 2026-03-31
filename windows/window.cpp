#include "window.h"
#include <QDir>
#include <QMenu>
#include <QTimer>
#include <QDialog>
#include <QWindow>
#include <QMouseEvent>
#include "rss.h"
#include "config.h"
#include "sql/sql.h"
#include "search_page.h"
#include "settings_page.h"
#include "api/pikpak_api.h"
#include "api/bangumi_api.h"
#include "player/player_page.h"
#include "utils/cache_image_util.h"
#include "utils/game_monitor_util.h"
#include "detail_page/detail_page.h"
#include "downloader/download_page.h"
#include "utils/global_hotkey_manager.h"
#include "episode_page/episode_overlay.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);
    initConfig();
    initializeManagers();
    dbManager->openDatabase();
    DatabaseManager::initTables();
    mainPageManager = new MainPageManager(this, cacheImageUtil, bangumiAPI, dbManager, gameMonitorUtil);
    setWindowFlags(Qt::FramelessWindowHint);
    QCoreApplication::instance()->installEventFilter(this);
    setupTrayIcon();
    setupConnections();
    checkCacheCleanup();
    QTimer::singleShot(5000, rss, &Rss::startRSS);
}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{   // 原生事件
    if (eventType == "windows_generic_MSG") {
        const auto msg = static_cast<MSG*>(message);
        if (msg->message == WM_HOTKEY) {
            if (hotkeyManager->handleHotKey(static_cast<UINT>(msg->wParam))) {
                *result = 0;
                return true;
            }
        }
    }
    return QMainWindow::nativeEvent(eventType, message, result);
}

void MainWindow::initializeManagers()
{   // 初始化管理器
    bangumiAPI = new BangumiAPI(this);
    dbManager = new DatabaseManager(this);
    cacheImageUtil = new CacheImageUtil(this);
    pikpakApi = new PikPakApi(this);
    rss = new Rss(bangumiAPI, this);
    hotkeyManager = new GlobalHotkeyManager(this, this);
    gameMonitorUtil = new GameMonitorUtil(hotkeyManager, this);
    trayMenu = new QMenu(this);
    trayIcon = new QSystemTrayIcon(this);
}

void MainWindow::setupTrayIcon() const
{   // 初始化系统托盘图标
    trayIcon->setIcon(QIcon::fromTheme("application-exit"));
    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();
}

void MainWindow::setupConnections()
{   // 连接
    // 系统托盘
    connect(trayMenu->addAction("退出"), &QAction::triggered, this, &MainWindow::closeOptionSelection);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayIconActivated);
    // RSS
    connect(rss, &Rss::rssUpdated, this, [this] {mainPageManager->loadCollections(mainPageManager->getCurrentSubjectType(), mainPageManager->getCurrentStatusType(), false);});
    // 删除收藏
    connect(dbManager, &DatabaseManager::collectionDeleted, this, [this] {mainPageManager->loadCollections(mainPageManager->getCurrentSubjectType(), mainPageManager->getCurrentStatusType(), false);});
    // 标题栏
    connect(btnMinimize, &QPushButton::clicked, this, [this] {showMinimized();});
    connect(btnMaximize, &QPushButton::clicked, this, &MainWindow::toggleMaximizeWindow);
    connect(btnClose, &QPushButton::clicked, this, &MainWindow::onCloseButtonClicked);
    // 搜索
    connect(btnSearch, &QPushButton::clicked, this, &MainWindow::onSearchButtonClicked);
    // 设置
    connect(btnSettings, &QPushButton::clicked, this, &MainWindow::onSettingsButtonClicked);
    // 下载
    connect(btnDownload, &QPushButton::clicked, this, &MainWindow::onDownloadButtonClicked);
    connect(mainPageManager, &MainPageManager::showMainPageRequested, this, [this] {stackedMainContent->setCurrentIndex(0);});
    // 详情
    connect(mainPageManager, &MainPageManager::showDetailPageRequested, this, &MainWindow::onShowDetailPageRequested);
    // 选集
    connect(mainPageManager, &MainPageManager::showEpisodePageRequested, this, &MainWindow::onShowEpisodePageRequested);
    // 游戏启动/退出
    connect(gameMonitorUtil, &GameMonitorUtil::gameStarted, this, &MainWindow::onGameStarted);
    connect(gameMonitorUtil, &GameMonitorUtil::gameExited, this, &MainWindow::gameExited);
}

void MainWindow::checkCacheCleanup() const
{   // 检查缓存清理
    const qlonglong next = getConfig("Cache/next_cleanup_timestamp", 0).toLongLong();
    const qlonglong now = QDateTime::currentSecsSinceEpoch();
    if (now < next && next != 0) return;
    QDateTime dt = QDateTime::currentDateTime().toTimeZone(QTimeZone("Asia/Shanghai"));
    int days = (Qt::Wednesday - dt.date().dayOfWeek() + 7) % 7;
    if (days == 0) days = 7;
    dt = dt.addDays(days);
    dt.setTime(QTime(6, 0));
    const qlonglong nextCleanup = dt.toSecsSinceEpoch();
    if (now >= next) {
        QDir imageDir("data/images");
        imageDir.removeRecursively();
        (void)imageDir.mkpath(".");
        dbManager->clearEpisodeCollectionTable();
    }
    setConfig("Cache/next_cleanup_timestamp", nextCleanup);
    mainPageManager->loadCollections(mainPageManager->getCurrentSubjectType(), mainPageManager->getCurrentStatusType(), false);
}

void MainWindow::toggleMaximizeWindow()
{   // 切换最大化/还原窗口
    isMaximized() ? showNormal() : showMaximized();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{   // 事件过滤器
    if (event->type() == QEvent::MouseButtonPress) {
        if (gameFocusTimer && gameFocusTimer->isActive()) {
            gameFocusTimer->stop();
            gameFocusTimer->deleteLater();
            gameFocusTimer = nullptr;
        }
    }
    if (watched == titlebar_frame) {
        const auto *mouseEvent = dynamic_cast<QMouseEvent*>(event);
        if (event->type() == QEvent::MouseButtonDblClick) {
            toggleMaximizeWindow();
            return true;
        }
        if (isMaximized() && (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseMove || event->type() == QEvent::MouseButtonRelease)) return false;
        if (event->type() == QEvent::MouseButtonPress) {
            m_dragPosition = mouseEvent->globalPosition().toPoint() - frameGeometry().topLeft();
            m_isDragging = true;
            return true;
        }
        if (event->type() == QEvent::MouseMove && m_isDragging) {
            move(mouseEvent->globalPosition().toPoint() - m_dragPosition);
            return true;
        }
        if (event->type() == QEvent::MouseButtonRelease) m_isDragging = false;
    }
    if (watched == btnClose && event->type() == QEvent::MouseButtonPress) {
        if (dynamic_cast<QMouseEvent*>(event)->button() == Qt::RightButton) {
            showCloseOptionDialog();
            return true;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{   // 窗口大小改变事件
    QMainWindow::resizeEvent(event);
    if (episodeOverlay) episodeOverlay->setGeometry(0, titlebar_frame->height(), width(), height() - titlebar_frame->height());
}

void MainWindow::showCloseOptionDialog()
{   // 显示关闭选项对话框
    QDialog dialog(this);
    dialog.setWindowFlags(Qt::Popup);
    auto *layout = new QVBoxLayout(&dialog);
    auto *minimizeBtn = new QPushButton("最小化到托盘", &dialog);
    auto *quitBtn = new QPushButton("退出", &dialog);
    layout->addWidget(minimizeBtn);
    layout->addWidget(quitBtn);
    auto *rememberCheckBox = new QCheckBox("记住我的选择", &dialog);
    layout->addWidget(rememberCheckBox);
    auto *hintLabel = new QLabel("右键关闭更改", &dialog);
    hintLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(hintLabel);
    connect(minimizeBtn, &QPushButton::clicked, &dialog, [&] {dialog.done(1);});
    connect(quitBtn, &QPushButton::clicked, &dialog, [&] {dialog.done(2);});
    const QPoint pos = QCursor::pos();
    const QSize hint = dialog.sizeHint();
    const QScreen *screen = QGuiApplication::screenAt(pos);
    const QRect screenRect = screen->availableGeometry();
    int x = pos.x() - hint.width() / 2;
    int y = pos.y() - hint.height();
    if (x < screenRect.left()) x = screenRect.left();
    if (x + hint.width() > screenRect.right()) x = screenRect.right() - hint.width();
    if (y < screenRect.top()) y = pos.y();
    if (y + hint.height() > screenRect.bottom()) y = screenRect.bottom() - hint.height();
    dialog.move(x, y);
    const int result = dialog.exec();
    if (result != 0) {
        if (rememberCheckBox->isChecked()) {
            setConfig("CloseAction/remember", true);
            setConfig("CloseAction/action", result);
        }
        closeOptionSelection(result);
    }
}

void MainWindow::onCloseButtonClicked()
{   // 关闭按钮点击
    if (getConfig("CloseAction/remember", false).toBool()) {
        const int action = getConfig("CloseAction/action", 1).toInt();
        closeOptionSelection(action);
    } else showCloseOptionDialog();
}

void MainWindow::closeOptionSelection(const int result)
{   // 关闭选项选择
    m_wasMaximized = isMaximized();
    m_savedNormalGeometry = normalGeometry();
    hide();
    if (result == 1) return;
    gameMonitorUtil->resumeAllSuspendedProcess(false);
    QApplication::quit();
}

void MainWindow::onTrayIconActivated(const QSystemTrayIcon::ActivationReason reason)
{   // 托盘激活窗口
    if (reason != QSystemTrayIcon::Trigger) return;
    showNormal();
    if (m_savedNormalGeometry.isValid()) {
        setGeometry(m_savedNormalGeometry);
        m_savedNormalGeometry = QRect();
    }
    if (m_wasMaximized) toggleMaximizeWindow();
    raise();
    activateWindow();
    if (windowHandle()) windowHandle()->requestActivate();
}

void MainWindow::onGameStarted()
{   // 游戏启动时
    if (gameFocusTimer) {
        gameFocusTimer->stop();
        gameFocusTimer->deleteLater();
        gameFocusTimer = nullptr;
    }
    gameFocusTimer = new QTimer(this);
    gameFocusTimer->setSingleShot(true);
    connect(gameFocusTimer, &QTimer::timeout, this, &MainWindow::onGameFocusTimeout);
    gameFocusTimer->start(5000);
}

void MainWindow::onGameFocusTimeout()
{   // 游戏启动后 焦点超时
    hide();
    if (!gameFocusTimer) return;
    gameFocusTimer->deleteLater();
    gameFocusTimer = nullptr;
}

void MainWindow::gameExited()
{   // 游戏退出
    onTrayIconActivated(QSystemTrayIcon::Trigger);
    if (!gameFocusTimer) return;
    gameFocusTimer->stop();
    gameFocusTimer->deleteLater();
    gameFocusTimer = nullptr;
}

void MainWindow::ensureSearchPage()
{   // 确保搜索页面
    searchPage = new SearchPage();
    searchPage->setManagers(cacheImageUtil, bangumiAPI, dbManager, gameMonitorUtil);
    stackedMainWindow->addWidget(searchPage);
    connect(searchPage, &SearchPage::backButtonClicked, this, &MainWindow::onBackButtonClicked);
    connect(searchPage, &SearchPage::showEpisodePageRequested, this, &MainWindow::onShowEpisodePageRequested);
    if (settingsPage) connect(settingsPage, &SettingsPage::nsfwSettingChanged, searchPage, &SearchPage::updateNsfwCheckBox);
}

void MainWindow::onSearchButtonClicked()
{   // 切换到搜索页面
    if (!searchPage) ensureSearchPage();
    searchPage->updateComboBoxByType(mainPageManager->getCurrentSubjectType());
    stackedMainWindow->setCurrentWidget(searchPage);
}

void MainWindow::onTagClicked(const QString &tag, const int subjectType)
{   // tag搜索
    if (!searchPage) ensureSearchPage();
    pageHistory.append(stackedMainWindow->currentWidget());
    stackedMainWindow->setCurrentWidget(searchPage);
    searchPage->searchByTag(tag, subjectType);
}

void MainWindow::onSettingsButtonClicked()
{   // 切换到设置页面
    if (!settingsPage) {
        settingsPage = new SettingsPage(this);
        settingsPage->setManagers(bangumiAPI, pikpakApi, dbManager);
        stackedMainWindow->addWidget(settingsPage);
        connect(settingsPage, &SettingsPage::backButtonClicked, this, &MainWindow::onBackButtonClicked);
        if (searchPage) connect(settingsPage, &SettingsPage::nsfwSettingChanged, searchPage, &SearchPage::updateNsfwCheckBox);
    }
    stackedMainWindow->setCurrentWidget(settingsPage);
}

void MainWindow::onShowDetailPageRequested(const int subjectId, const QString &progressText)
{   // 切换到详情页面
    if (!detailPage) {
        detailPage = new DetailPage();
        detailPage->setManagers(cacheImageUtil, bangumiAPI, dbManager, gameMonitorUtil);
        stackedMainWindow->addWidget(detailPage);
        connect(detailPage, &DetailPage::backButtonClicked, this, &MainWindow::onBackButtonClicked);
        connect(detailPage, &DetailPage::showEpisodePageRequested, this, &MainWindow::onShowEpisodePageRequested);
        connect(detailPage, &DetailPage::tagClicked, this, &MainWindow::onTagClicked);
        connect(detailPage, &DetailPage::refresh, this, [this] {mainPageManager->loadCollections(mainPageManager->getCurrentSubjectType(), mainPageManager->getCurrentStatusType(), false);});
    }
    detailPage->clearHistory();
    detailPage->setCollectionData(subjectId, progressText);
    stackedMainWindow->setCurrentWidget(detailPage);
}

void MainWindow::onDownloadButtonClicked()
{   // 切换到下载页面
    if (!downloadPage) {
        downloadPage = new DownloadPage(this);
        downloadPage->setManagers(pikpakApi);
        stackedMainContent->addWidget(downloadPage);
    }
    stackedMainContent->setCurrentIndex(1);
    downloadPage->loadRecentFiles();
}

void MainWindow::onShowEpisodePageRequested(const SubjectsData &subjectsData)
{   // 创建选集遮罩层
    repaint();
    episodeOverlay = new EpisodeOverlay(this);
    episodeOverlay->setManagers(bangumiAPI, dbManager);
    connect(episodeOverlay, &EpisodeOverlay::episodeClicked, this, &MainWindow::onEpisodeClicked);
    connect(episodeOverlay, &EpisodeOverlay::overlayClosed, this, [this] {
        episodeOverlay->deleteLater();
        episodeOverlay = nullptr;
    });
    connect(episodeOverlay, &EpisodeOverlay::collectionDataChanged, this, [this] {mainPageManager->loadCollections(mainPageManager->getCurrentSubjectType(), mainPageManager->getCurrentStatusType(), false);});
    episodeOverlay->setGeometry(0, titlebar_frame->height(), width(), height() - titlebar_frame->height());
    episodeOverlay->showWithSubjectsData(subjectsData);
    if (playerPage) return;
    if (subjectsData.subjectType != 2) return;
    QTimer::singleShot(1, this, &MainWindow::precreatePlayerPage);
}

void MainWindow::precreatePlayerPage()
{   // 播放器页面预创建
    playerPage = new PlayerPage(this);
    playerPage->setManagers(cacheImageUtil, pikpakApi);
    stackedMainWindow->addWidget(playerPage);
    connect(playerPage, &PlayerPage::backButtonClicked, this, &MainWindow::onBackButtonClicked);
}

void MainWindow::onEpisodeClicked(const SubjectsData &subjectsData, const EpisodeData &episodeData)
{   // 切换播放器页面
    pageHistory.append(stackedMainWindow->currentWidget());
    playerPage->fetchRoutes(subjectsData, episodeData);
    stackedMainWindow->setCurrentWidget(playerPage);
}

void MainWindow::onBackButtonClicked()
{   // 返回历史页面
    if (!pageHistory.isEmpty()) {
        QWidget *prevPage = pageHistory.takeLast();
        stackedMainWindow->setCurrentWidget(prevPage);
    } else {
        stackedMainWindow->setCurrentIndex(0);
        QTimer::singleShot(100, this, [this]{mainPageManager->loadCollections(mainPageManager->getCurrentSubjectType(), mainPageManager->getCurrentStatusType(), false);});
    }
}