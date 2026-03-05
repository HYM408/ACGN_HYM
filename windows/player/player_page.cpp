#include "player_page.h"
#include <QLabel>
#include <QDialog>
#include <QPointer>
#include <QCheckBox>
#include <QJsonArray>
#include <QPushButton>
#include <QThreadPool>
#include "player.h"
#include "../config.h"
#include "player_core.h"
#include "../api/pikpak_api.h"
#include "../crawler/crawler.h"
#include "../utils/cache_image_util.h"

PlayerPage::PlayerPage(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    // 初始化播放器
    vlcPlayer = new VLCPlayer(ui.video_container);
    qobject_cast<QVBoxLayout*>(ui.video_container->layout())->addWidget(vlcPlayer);
    // 设置控制面板
    setupControlOverlay();
}

void PlayerPage::setManagers(CacheImageUtil *cacheImage, PikPakApi *pikpakapi)
{   // 初始化实例
    cacheImageUtil = cacheImage;
    pikpakApi = pikpakapi;
}

void PlayerPage::setupControlOverlay()
{   // 设置控制面板
    controlOverlay = new ControlOverlay(vlcPlayer);
    controlOverlay->setGeometry(vlcPlayer->rect());
    controlOverlay->raise();
    vlcPlayer->installEventFilter(this);
    connect(controlOverlay, &ControlOverlay::playPauseRequested, vlcPlayer, &VLCPlayer::togglePlayPause);
    connect(controlOverlay, &ControlOverlay::volumeChanged, vlcPlayer, &VLCPlayer::setVolume);
    connect(controlOverlay, &ControlOverlay::seekRequested, vlcPlayer, &VLCPlayer::setPosition);
    connect(controlOverlay, &ControlOverlay::playbackRateChanged, vlcPlayer, &VLCPlayer::setPlaybackRate);
    connect(controlOverlay, &ControlOverlay::fullscreenRequested, this, &PlayerPage::toggleFullscreen);
    connect(controlOverlay, &ControlOverlay::backRequested, this, &PlayerPage::onBackButtonClicked);
    connect(vlcPlayer, &VLCPlayer::playStateChanged, controlOverlay, &ControlOverlay::setPlayState);
    connect(vlcPlayer, &VLCPlayer::timeChanged, [this](int currentMs, int totalMs) {controlOverlay->setTime(currentMs / 1000, totalMs / 1000);});
    connect(vlcPlayer, &VLCPlayer::positionChanged, controlOverlay, &ControlOverlay::setProgress);
    connect(vlcPlayer, &VLCPlayer::exitFullscreenRequested, this, [this] {if (fullscreen_mode) toggleFullscreen();});
    connect(vlcPlayer, &VLCPlayer::volumeChanged, controlOverlay, &ControlOverlay::setVolume);
}

void PlayerPage::setSiteLoadingState(const QString &siteId) const
{   // 更新卡片状态
    if (auto *card = siteWidgets.value(siteId)) {
        if (auto *statusBtn = card->findChild<QPushButton*>("statusBtn")) statusBtn->setText("搜索中...");
        if (auto *routesContainer = card->findChild<QWidget*>("routesContainer")) routesContainer->setVisible(false);
    }
    if (auto *contentFrame = siteDetailFrames.value(siteId)) {
        auto *contentLayout = qobject_cast<QVBoxLayout*>(contentFrame->layout());
        clearLayout(contentLayout);
        auto *loadingBtn = new QPushButton("搜索中...");
        loadingBtn->setStyleSheet("color: #666; padding: 5px; border: none");
        contentLayout->addWidget(loadingBtn);
        contentLayout->addStretch();
    }
}

void PlayerPage::startSiteSearch(const QString &siteId)
{   // 搜索
    setSiteLoadingState(siteId);
    if (Crawler::getAllAPISiteIds().contains(siteId)) {QThreadPool::globalInstance()->start([=] {
        auto results = Crawler::searchAPI(siteId, m_keyword, m_abortFlag);
        QMetaObject::invokeMethod(this, "handleSearchResult", Qt::QueuedConnection, Q_ARG(QString, siteId), Q_ARG(QList<SearchResult>, results));});
    } else if (Crawler::getAllSiteIds().contains(siteId)) {QThreadPool::globalInstance()->start([=] {
        auto results = Crawler::searchSite(siteId, m_keyword, m_abortFlag);
        QMetaObject::invokeMethod(this, "handleSearchResult", Qt::QueuedConnection, Q_ARG(QString, siteId), Q_ARG(QList<SearchResult>, results));});
    } else if (Crawler::getAllBTSiteIds().contains(siteId)) {QThreadPool::globalInstance()->start([=] {
        auto results = Crawler::searchBT(siteId, m_keyword, m_abortFlag);
        QMetaObject::invokeMethod(this, "handleBTSearchResult", Qt::QueuedConnection, Q_ARG(QString, siteId), Q_ARG(QList<BTResult>, results));});
    }
}

void PlayerPage::fetchRoutes(const CollectionData &collectionData, const EpisodeData &episodeData)
{   // 创建组件
    show();
    m_abortFlag = std::make_shared<std::atomic<bool>>(false);
    m_episodeData = episodeData;
    m_keyword = collectionData.subject_name_cn;
    if (m_keyword.isEmpty()) m_keyword = collectionData.subject_name;
    *m_abortFlag = true;
    m_abortFlag->store(false);
    QStringList allApiIds = Crawler::getAllAPISiteIds();
    QStringList allSiteIds = Crawler::getAllSiteIds();
    QStringList allBtIds = Crawler::getAllBTSiteIds();
    QStringList allIds = allApiIds + allSiteIds + allBtIds;
    if (allIds.isEmpty()) return;
    QVariant sitesOrder = getConfig("EnabledSites/sites");
    QStringList sortedIds;
    if (sitesOrder.typeId() == QMetaType::QStringList)  sortedIds = sitesOrder.toStringList();
    else {
        QString str = sitesOrder.toString();
        if (str.isEmpty()) return;
        if (str == "*") {
            setConfig("EnabledSites/sites", allIds);
            sortedIds = allIds;
        } else sortedIds = QStringList(str);
    }
    if (!sortedIds.isEmpty()) {
        QSet<QString> allSet = QSet(allIds.begin(), allIds.end());
        QStringList filtered;
        for (const QString &id : sortedIds) filtered.append(id);
        allIds = filtered;
    }
    QWidget *container = ui.scrollAreaWidgetContents;
    container->setLayout(new QVBoxLayout());
    QWidget *detailTab = ui.tabWidget->widget(1);
    detailTabWidget = new QTabWidget();
    detailTabWidget->setTabPosition(QTabWidget::West);
    detailTabWidget->setIconSize(QSize(30, 30));
    detailTabWidget->setStyleSheet("QTabBar::tab {width: 50px; height: 50px}");
    detailTab->layout()->addWidget(detailTabWidget);
    for (const QString &siteId : std::as_const(allIds)) {
        QWidget *card = createSiteCard(siteId);
        siteWidgets[siteId] = card;
        container->layout()->addWidget(card);
        createSiteDetailTab(siteId);
    }
    qobject_cast<QVBoxLayout*>(container->layout())->addStretch();
    for (const QString &siteId : allIds) startSiteSearch(siteId);
}

void PlayerPage::reSearchSite(const QString &siteId)
{   // 再次搜索
    if (m_keyword.isEmpty()) return;
    startSiteSearch(siteId);
}

QString PlayerPage::getSiteIconUrl(const QString &siteId)
{   // 图标
    std::vector loaders = {Crawler::loadAPIConfig, Crawler::loadSiteConfig, Crawler::loadBTConfig};
    for (auto loader : loaders) {
        QJsonObject config = loader(siteId);
        if (!config.isEmpty()) return config["icon"].toString();
    }
    return {};
}

void PlayerPage::createSiteDetailTab(const QString &siteId)
{   // 详情标签
    auto *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setStyleSheet("QScrollBar:vertical {border: none; background: #f0f0f0; width: 11px}"
                              "QScrollBar::handle:vertical {background: #c0c0c0; border-radius: 5px; min-height: 20px}"
                              "QScrollBar::handle:vertical:hover {background: #a0a0a0;}"
                              "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {border: none; background: none; height: 0px}");
    auto *contentFrame = new QFrame();
    auto *contentLayout = new QVBoxLayout(contentFrame);
    contentLayout->setContentsMargins(5, 5, 5, 5);
    contentLayout->setSpacing(3);
    contentLayout->setAlignment(Qt::AlignTop);
    scrollArea->setWidget(contentFrame);
    int tabIndex = detailTabWidget->addTab(scrollArea, "");Q_UNUSED(tabIndex);
    siteDetailFrames[siteId] = contentFrame;
    QString iconUrl = getSiteIconUrl(siteId);
    cacheImageUtil->getImageAsync(iconUrl, [this, tabIndex](const QPixmap &pixmap) {
        if (!detailTabWidget) return;
        if (tabIndex < 0 || tabIndex >= detailTabWidget->count()) return;
        QTransform transform;
        detailTabWidget->setTabIcon(tabIndex, QIcon(pixmap.transformed(transform.rotate(90))));
    }, false);
}

QWidget* PlayerPage::createSiteCard(const QString &siteId)
{   // 基础标签
    auto *card = new QFrame();
    card->setProperty("siteId", siteId);
    card->setFixedHeight(80);
    auto *layout = new QVBoxLayout(card);
    layout->setSpacing(0);
    layout->setContentsMargins(5, 0, 5, 0);
    auto *topLayout = new QHBoxLayout();
    topLayout->setSpacing(10);
    topLayout->setAlignment(Qt::AlignTop);
    // 图标
    auto *iconBtn = new QPushButton();
    iconBtn->setFixedSize(20, 20);
    iconBtn->setFlat(true);
    topLayout->addWidget(iconBtn);
    QString iconUrl = getSiteIconUrl(siteId);
    QPointer btnPtr(iconBtn);
    cacheImageUtil->getImageAsync(iconUrl, [btnPtr](const QPixmap &pixmap) {if (btnPtr) btnPtr->setIcon(QIcon(pixmap));}, false);
    // 站点名称
    auto *siteBtn = new QPushButton(siteId);
    siteBtn->setFont(QFont("", 14, QFont::Bold));
    siteBtn->setStyleSheet(
        "QPushButton {color: #333; text-align: left; border: none; padding: 5px 5px}"
        "QPushButton:hover {background-color: #f0f0f0; border-radius: 5px}");
    siteBtn->setFlat(true);
    connect(siteBtn, &QPushButton::clicked, this, [this, siteId] {reSearchSite(siteId);});
    topLayout->addWidget(siteBtn);
    // 状态.标题
    auto *statusBtn = new QPushButton();
    statusBtn->setObjectName("statusBtn");
    statusBtn->setFlat(true);
    statusBtn->setStyleSheet("color: #666; font-size: 13px; text-align: left; border: none");
    topLayout->addWidget(statusBtn, 1);
    layout->addLayout(topLayout);
    // 线路
    auto *routesContainer = new QWidget();
    routesContainer->setObjectName("routesContainer");
    auto *routesLayout = new QHBoxLayout(routesContainer);
    routesLayout->setAlignment(Qt::AlignLeft);
    routesContainer->setVisible(false);
    layout->addWidget(routesContainer);
    statusBtn->setText("");
    return card;
}

void PlayerPage::updateCardContent(const QWidget *card, const QString &status, const QList<SearchResult> &results)
{   // 更新基础卡片
    if (auto *statusBtn = card->findChild<QPushButton*>("statusBtn")) statusBtn->setText(status == "loading" ? "搜索中..." : (status == "success" ? results.first().title : "✗"));
    auto *routesContainer = card->findChild<QWidget*>("routesContainer");
    auto *routesLayout = qobject_cast<QHBoxLayout*>(routesContainer->layout());
    clearLayout(routesLayout);
    if (status == "success" && !results.first().routes.isEmpty()) {
        QString siteId = card->property("siteId").toString();
        for (const QJsonObject &routeObj : results.first().routes) {
            QString routeName = routeObj["route"].toString();
            auto *button = new QPushButton(routeName);
            button->setMinimumHeight(35);
            button->setStyleSheet("QPushButton {padding: 5px 10px; font-size: 13px; border: 1px solid #ddd; border-radius: 5px; background-color: white}"
                                  "QPushButton:hover {background-color: #f5f5f5; border-color: #aaa}");
            connect(button, &QPushButton::clicked, [this, siteId, routeObj] {onRouteSelected(siteId, routeObj);});
            routesLayout->addWidget(button);
        }
        routesContainer->setVisible(true);
    } else routesContainer->setVisible(false);
}

void PlayerPage::updateBTCardContent(const QWidget *card, const QString &status, const QList<BTResult> &results)
{   // 更新BT基础卡片
    if (auto *statusBtn = card->findChild<QPushButton*>("statusBtn")) statusBtn->setText(status == "loading" ? "搜索中..." : (status == "success" ? QString::number(results.size()) + "个结果" : "✗"));
    card->findChild<QWidget*>("routesContainer")->setVisible(false);
}

void PlayerPage::handleSearchResult(const QString &siteId, const QList<SearchResult> &results)
{   // 处理搜索结果
    if (auto *card = siteWidgets.value(siteId)) updateCardContent(card, results.isEmpty() ? "error" : "success", results);
    auto *contentFrame = siteDetailFrames.value(siteId);
    if (!contentFrame) return;
    auto *contentLayout = qobject_cast<QVBoxLayout*>(contentFrame->layout());
    clearLayout(contentLayout);
    if (results.isEmpty()) {
        auto *errorBtn = new QPushButton("搜索失败或无结果");
        errorBtn->setFlat(true);
        errorBtn->setStyleSheet("color: #ff4444; padding: 10px; border: none");
        contentLayout->addWidget(errorBtn);
    } else for (const auto &searchResult : results) for (const auto &routeObj : searchResult.routes) contentLayout->addWidget(createRouteComponent(siteId, routeObj, searchResult.title));
    contentLayout->addStretch();
}

void PlayerPage::handleBTSearchResult(const QString &siteId, const QList<BTResult> &results)
{   // 处理BT搜索结果
    QList<BTResult> filtered, excluded;
    filterBTResults(results, filtered, excluded);
    if (auto *card = siteWidgets.value(siteId)) updateBTCardContent(card, results.isEmpty() ? "error" : "success", results);
    auto *contentFrame = siteDetailFrames.value(siteId);
    if (!contentFrame) return;
    auto *contentLayout = qobject_cast<QVBoxLayout*>(contentFrame->layout());
    clearLayout(contentLayout);
    if (results.isEmpty()) {
        auto *errorBtn = new QPushButton("搜索失败或无结果");
        errorBtn->setFlat(true);
        errorBtn->setStyleSheet("color: #ff4444; padding: 10px; border: none");
        contentLayout->addWidget(errorBtn);
    } else {
        for (const auto &result : filtered) contentLayout->addWidget(createBTResultComponent(result));
        if (!excluded.isEmpty()) {
            auto *toggleCheck = new QCheckBox("显示被过滤的结果 (" + QString::number(excluded.size()) + "个)");
            toggleCheck->setChecked(false);
            contentLayout->addWidget(toggleCheck);
            auto *excludedContainer = new QWidget();
            excludedContainer->setVisible(false);
            auto *excludedLayout = new QVBoxLayout(excludedContainer);
            excludedLayout->setContentsMargins(0, 0, 0, 0);
            for (const auto &result : excluded) {
                QFrame *comp = createBTResultComponent(result);
                for (QLabel *label : comp->findChildren<QLabel*>()) label->setStyleSheet(label->styleSheet() + " color: #666;");
                excludedLayout->addWidget(comp);
            }
            excludedLayout->addStretch();
            contentLayout->addWidget(excludedContainer);
            connect(toggleCheck, &QCheckBox::toggled, [excludedContainer](bool checked) {excludedContainer->setVisible(checked);});
        }
    }
    contentLayout->addStretch();
}

void PlayerPage::filterBTResults(const QList<BTResult> &results, QList<BTResult> &filtered, QList<BTResult> &excluded)
{   // 过滤BT搜索结果
    filtered.clear();
    excluded.clear();
    for (const BTResult &result : results) {
        QString name = result.name.toLower();
        if (name.contains("360") || name.contains("480") || name.contains("720")) excluded.append(result);
        else filtered.append(result);
    }
}

QFrame* PlayerPage::createRouteComponent(const QString &siteId, const QJsonObject &route, const QString &title)
{   // 详细组件
    auto *frame = new QFrame();
    frame->setAttribute(Qt::WA_Hover);
    frame->setStyleSheet("QFrame {border: 1px solid #e0e0e0; border-radius: 4px; margin: 2px; background-color: white}"
                         "QFrame:hover {border: 1px solid #4da6ff; background-color: #e8f4ff}");
    auto *layout = new QVBoxLayout(frame);
    auto makeLabel = [](const QString &text, int size, const QString &color, bool bold) -> QLabel* {
        auto *label = new QLabel(text);
        label->setFont(QFont("微软雅黑", size, bold ? QFont::Bold : QFont::Normal));
        label->setStyleSheet(QString("color: %1; background-color: transparent; border: none; text-align: left").arg(color));
        label->setWordWrap(true);
        return label;
    };
    layout->addWidget(makeLabel(title, 13, "black", true));
    layout->addWidget(makeLabel(route["route"].toString(), 10, "#333", true));
    frame->setProperty("siteId", siteId);
    frame->setProperty("route", route.toVariantMap());
    frame->installEventFilter(this);
    return frame;
}

QFrame* PlayerPage::createBTResultComponent(const BTResult &result)
{   // BT结果组件
    auto *frame = new QFrame();
    frame->setAttribute(Qt::WA_Hover);
    frame->setStyleSheet("QFrame {border: 1px solid #e0e0e0; border-radius: 4px; margin: 2px; background-color: white}"
                         "QFrame:hover {border: 1px solid #4caf50; background-color: #e8f5e9}");
    auto *layout = new QVBoxLayout(frame);
    layout->setSpacing(5);
    auto addLabel = [layout](const QString &text, int fontSize, const QString &color, bool bold = false, bool expand = false) {
        if (text.isEmpty()) return;
        auto *label = new QLabel(text);
        QFont font("微软雅黑", fontSize);
        font.setBold(bold);
        label->setFont(font);
        label->setStyleSheet(QString("color: %1; border: none; background: transparent;").arg(color));
        label->setWordWrap(true);
        if (expand) label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        layout->addWidget(label);
    };
    addLabel(result.name, 13, "black", true, true);
    if (!result.size.isEmpty())
        addLabel("大小: " + result.size, 10, "#666");
    frame->setProperty("magnet", result.magnet_link);
    frame->setProperty("play", result.play_link);
    frame->installEventFilter(this);
    return frame;
}

bool PlayerPage::eventFilter(QObject *watched, QEvent *event)
{   // 事件
    if (watched == vlcPlayer && event->type() == QEvent::Resize) {
        controlOverlay->setGeometry(vlcPlayer->rect());
        return true;
    }
    if (event->type() == QEvent::MouseButtonPress) {
        if (auto *frame = qobject_cast<QFrame*>(watched)) {
            if (frame->dynamicPropertyNames().contains("magnet")) {
                onBTResultClicked(frame->property("magnet").toString(), frame->property("play").toString());
                return true;
            }
            onRouteSelected(frame->property("siteId").toString(), QJsonObject::fromVariantMap(frame->property("route").toMap()));
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void PlayerPage::onRouteSelected(const QString &siteId, const QJsonObject &route) const
{   // 解析并播放
    vlcPlayer->stop();
    int episodeIndex = static_cast<int>(m_episodeData.sort) - 1;
    QJsonArray episodes = route["episodes"].toArray();
    if (episodes.isEmpty() || episodeIndex < 0 || episodeIndex >= episodes.size()) return;
    QString episodeUrl = episodes[episodeIndex].toObject()["link"].toString();
    if (episodeUrl.isEmpty()) return;
    Crawler::processVideoUrl(siteId, episodeUrl, [this](const QString &videoUrl) {
        qDebug() << videoUrl;
        vlcPlayer->playVideo(videoUrl);
    });
    vlcPlayer->setFocus();
}

void PlayerPage::onBTResultClicked(const QString &magnet, const QString &playLink)
{   // 点击BT结果
    auto *dialog = new QDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle("BT 结果");
    dialog->setFixedSize(600, 200);
    auto *layout = new QVBoxLayout(dialog);
    layout->addWidget(new QLabel(QString("磁力链接: %1").arg(magnet)));
    auto *playLabel = new QLabel(QString("<a href='%1'>播放链接: %1</a>").arg(playLink));
    playLabel->setOpenExternalLinks(true);
    playLabel->setTextFormat(Qt::RichText);
    layout->addWidget(playLabel);
    auto *redirectLabel = new QLabel("重定向中...");
    layout->addWidget(redirectLabel);
    // 按钮区域
    auto *buttonLayout = new QHBoxLayout();
    auto *downloadBtn = new QPushButton("BT下载");
    connect(downloadBtn, &QPushButton::clicked, [magnet] {qDebug() << magnet;});
    buttonLayout->addWidget(downloadBtn);
    auto *pikpakBtn = new QPushButton("保存到PikPak(ip不能在中国大陆)");
    pikpakBtn->setFixedWidth(250);
    pikpakBtn->setEnabled(false);
    buttonLayout->addWidget(pikpakBtn);
    auto *redetectBtn = new QPushButton("重定向");
    buttonLayout->addWidget(redetectBtn);
    layout->addLayout(buttonLayout);
    auto *manager = new QNetworkAccessManager(dialog);Q_UNUSED(manager);
    // 重定向检测
    auto detectRedirect = [=] {
        redirectLabel->setText("重定向中...");
        pikpakBtn->setEnabled(false);
        QNetworkRequest request(playLink);
        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
        QNetworkReply *reply = manager->get(request);
        connect(reply, &QNetworkReply::finished, [=] {
            if (reply->error() == QNetworkReply::NoError) {
                QUrl finalUrl = reply->url();
                QString finalUrlStr = finalUrl.toString();
                if (finalUrlStr.contains("?act=play")) finalUrlStr = finalUrlStr.left(finalUrlStr.indexOf("?act=play"));
                redirectLabel->setText(QString("PikPak链接: %1").arg(finalUrlStr));
                pikpakBtn->setProperty("finalUrl", finalUrlStr);
                pikpakBtn->setEnabled(true);
            } else redirectLabel->setText(QString("重定向失败: %1").arg(reply->errorString()));
            reply->deleteLater();
        });
    };
    connect(redetectBtn, &QPushButton::clicked, detectRedirect);
    detectRedirect();
    // 保存到PikPak
    connect(pikpakBtn, &QPushButton::clicked, [=] {
        QString finalUrl = pikpakBtn->property("finalUrl").toString();
        pikpakBtn->setText("转存中...");
        pikpakBtn->setEnabled(false);
        if (pikpakApi->transferShareLink(finalUrl, "")) pikpakBtn->setText("转存成功");
        else pikpakBtn->setText("转存失败");
        pikpakBtn->setEnabled(true);
    });
    dialog->exec();
}

void PlayerPage::toggleFullscreen()
{
    if (!fullscreen_mode) {
        window()->hide();
        fullscreen_mode = true;
        original_parent = vlcPlayer->parentWidget();
        original_layout = original_parent->layout();
        vlcPlayer->setParent(nullptr);
        vlcPlayer->showFullScreen();
    } else {
        window()->show();
        fullscreen_mode = false;
        vlcPlayer->showNormal();
        vlcPlayer->setParent(original_parent);
        original_layout->addWidget(vlcPlayer);
    }
    vlcPlayer->setFocus();
}

void PlayerPage::clearLayout(QLayout *layout)
{   // 清理布局
    if (!layout) return;
    while (QLayoutItem *item = layout->takeAt(0)) {
        if (QWidget *widget = item->widget()) widget->deleteLater();
        delete item;
    }
}

void PlayerPage::cleanupPage()
{   // 清理
    vlcPlayer->stop();
    QWidget *container = ui.scrollAreaWidgetContents;
    if (QLayout *oldLayout = container->layout()) {
        clearLayout(oldLayout);
        delete oldLayout;
    }
    if (detailTabWidget) {
        detailTabWidget->deleteLater();
        detailTabWidget = nullptr;
    }
    siteWidgets.clear();
    siteDetailFrames.clear();
}

void PlayerPage::cancelAllSearches() const
{   // 取消所有搜索
    if (m_abortFlag) *m_abortFlag = true;
}

void PlayerPage::onBackButtonClicked()
{   // 返回
    if (fullscreen_mode) toggleFullscreen();
    cancelAllSearches();
    cleanupPage();
    emit backButtonClicked();
}