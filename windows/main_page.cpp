#include "main_page.h"
#include "sql/sql.h"
#include "utils/menu_util.h"
#include "utils/image_util.h"
#include "utils/progress_util.h"
#include "utils/cache_image_util.h"

MainPageManager::MainPageManager(Ui::MainWindow *mainWindow, CacheImageUtil *cacheImageUtil, BangumiAPI *bangumiAPI, DatabaseManager *dbManager): QObject(nullptr), mainWindow(mainWindow), dbManager(dbManager), cacheImageUtil(cacheImageUtil), bangumiAPI(bangumiAPI)
{
    // 初始化状态映射
    statusNamesMap = {
        {2, {{1, "想看"}, {2, "看过"}, {3, "在看"}}},
        {4, {{1, "想玩"}, {2, "玩过"}, {3, "在玩"}}},
        {7, {{1, "想读"}, {2, "读过"}, {3, "在读"}}},
        {8, {{1, "想读"}, {2, "读过"}, {3, "在读"}}}};
    // 初始化连接
    setupConnections();
}

void MainPageManager::initStatusFrames()
{   // 状态
    const auto frames = {
        std::make_pair(1, StatusFrameInfo{mainWindow->frame_3, mainWindow->pushButton_3, mainWindow->pushButton_10}),
        std::make_pair(2, StatusFrameInfo{mainWindow->frame_7, mainWindow->pushButton_5, mainWindow->pushButton_12}),
        std::make_pair(3, StatusFrameInfo{mainWindow->frame_5, mainWindow->pushButton_4, mainWindow->pushButton_11}),
        std::make_pair(4, StatusFrameInfo{mainWindow->frame_9, mainWindow->pushButton_6, mainWindow->pushButton_13}),
        std::make_pair(5, StatusFrameInfo{mainWindow->frame_11, mainWindow->pushButton_7, mainWindow->pushButton_14})
    };
    for (const auto &[statusType, info] : frames) {
        statusFrames[statusType] = info;
        info.frame->setProperty("status_type", statusType);
        info.frame->setMouseTracking(true);
        info.frame->installEventFilter(this);
        info.statusButton->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        info.countButton->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    }
}

void MainPageManager::setupConnections()
{   // 连接
    initStatusFrames();
    connect(mainWindow->animation_Button, &QPushButton::clicked, this, [this] {switchCategory(2, "动画");});
    connect(mainWindow->novel_Button, &QPushButton::clicked, this, [this] {switchCategory(7, "小说");});
    connect(mainWindow->game_Button, &QPushButton::clicked, this, [this] {switchCategory(4, "游戏");});
    connect(mainWindow->comic_Button, &QPushButton::clicked, this, [this] {switchCategory(8, "漫画");});
    connect(mainWindow->refresh_Button, &QPushButton::clicked, this, [this] {loadCollections(currentSubjectType, currentStatusType, true);});
    connect(mainWindow->previous_Button, &QPushButton::clicked, this, &MainPageManager::previousPage);
    connect(mainWindow->next_Button, &QPushButton::clicked, this, &MainPageManager::nextPage);
    connect(mainWindow->searchlist_lineEdit, &QLineEdit::textChanged, this, &MainPageManager::onSearchTextChanged);
    mainWindow->animation_Button->setChecked(true);
    loadCollections(2, 3, true);
}

bool MainPageManager::eventFilter(QObject *obj, QEvent *event)
{   // 鼠标事件
    if (obj->property("isCard").toBool() && event->type() == QEvent::MouseButtonPress) {
        const auto *frame = qobject_cast<QFrame*>(obj);
        const auto *progressLabel = frame->property("progressLabel").value<QLabel*>();
        showDetailPage(frame->property("collectionData").value<CollectionData>(), progressLabel ? progressLabel->text() : QString());
        return true;
    }
    for (auto &[statusType, info] : statusFrames.toStdMap()) {
        if (info.frame == obj) {
            const QString baseStyle = statusType == currentStatusType ? ";border-bottom: 3px solid rgb(103, 79, 165)" : "";
            if (event->type() == QEvent::Enter) {
                info.frame->setStyleSheet(QString("QFrame{background-color: rgba(103, 79, 165, 30)%1}").arg(baseStyle));
                return true;
            }
            if (event->type() == QEvent::Leave) {
                info.frame->setStyleSheet(QString("QFrame{background-color: transparent%1}").arg(baseStyle));
                return true;
            }
            if (event->type() == QEvent::MouseButtonPress) {
                loadCollections(currentSubjectType, statusType, true);
                return true;
            }
        }
    }
    return QObject::eventFilter(obj, event);
}

void MainPageManager::switchCategory(const int subjectType, const QString &title)
{   // 类别切换
    emit showMainPageRequested();
    mainWindow->project_Button->setText(title);
    loadCollections(subjectType, currentStatusType, true);
}

void MainPageManager::onSearchTextChanged(const QString &text)
{   //当前类别搜索
    currentPage = 1;
    if (text.trimmed().isEmpty()) filteredCollections = allCollections;
    else {
        QString searchLower = text.toLower();
        filteredCollections.clear();
        std::copy_if(allCollections.begin(), allCollections.end(), std::back_inserter(filteredCollections), [&searchLower](const CollectionData &collection) {
            return collection.subject_name_cn.toLower().contains(searchLower) || collection.subject_name.toLower().contains(searchLower);
        });
    }
    displayCurrentPage();
}

void MainPageManager::loadCollections(const int subjectType, const int statusType, const bool resetToFirstPage)
{   // 加载数据
    const bool typeChanged = subjectType != currentSubjectType || statusType != currentStatusType;
    QString oldSearchText;
    if (!typeChanged) oldSearchText = mainWindow->searchlist_lineEdit->text();
    currentSubjectType = subjectType;
    currentStatusType = statusType;
    const auto &statusMap = statusNamesMap[subjectType];
    const QJsonObject statusCounts = DatabaseManager::getStatusCountsBySubjectType(subjectType);
    for (int i = 1; i <= 3; ++i) statusFrames[i].statusButton->setText(statusMap[i]);
    for (auto &[key, info] : statusFrames.toStdMap()) {
        const int count = statusCounts.value(QString::number(key)).toInt();
        info.countButton->setText(QString::number(count));
        const bool isHovered = info.frame->underMouse();
        const bool selected = key == statusType;
        QString backgroundColor = isHovered ? "rgba(103, 79, 165, 30)" : "transparent";
        QString borderStyle = selected ? ";border-bottom: 3px solid rgb(103, 79, 165)" : "";
        info.frame->setStyleSheet(QString("QFrame{background-color: %1%2}").arg(backgroundColor, borderStyle));
    }
    allCollections = DatabaseManager::getCollectionBySubjectTypeAndType(subjectType, statusType);
    if (!allCollections.isEmpty()) {
        QList<int> subjectIds;
        for (const auto &col : allCollections) subjectIds.append(col.subject_id);
        airdatesJson = dbManager->getEpisodeAirdates(subjectIds);
    } else airdatesJson = QJsonObject();
    if (!typeChanged) {
        if (oldSearchText.isEmpty()) filteredCollections = allCollections;
        else {
            QString searchLower = oldSearchText.toLower();
            filteredCollections.clear();
            std::copy_if(allCollections.begin(), allCollections.end(), std::back_inserter(filteredCollections), [&searchLower](const CollectionData &collection) {return collection.subject_name_cn.toLower().contains(searchLower) || collection.subject_name.toLower().contains(searchLower);});
        }
    } else {
        filteredCollections = allCollections;
        if (resetToFirstPage) currentPage = 1;
        mainWindow->searchlist_lineEdit->clear();
    }
    allCollections.isEmpty() ? clearDisplayArea() : displayCurrentPage();
    updatePageInfo();
}

void MainPageManager::displayCurrentPage()
{   // 显示标签
    clearDisplayArea();
    const auto layout = mainWindow->gridLayout_2;
    layout->setColumnMinimumWidth(0, 420);
    layout->setColumnMinimumWidth(1, 420);
    layout->setColumnMinimumWidth(2, 420);
    layout->setVerticalSpacing(20);
    layout->setHorizontalSpacing(40);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    const int start = (currentPage - 1) * itemsPerPage;
    const int dataCount = static_cast<int>(qMin(itemsPerPage, filteredCollections.size() - start));
    for (int i = 0; i < dataCount; ++i) {
        CardComponents card;
        createCardComponents(card, filteredCollections[start + i]);
        layout->addWidget(card.card, i / 3, i % 3, Qt::AlignCenter);
        card.card->show();
    }
    for (int i = 0; i < 3 * ((dataCount + 2) / 3) - dataCount; ++i) {
        const auto placeholder = new QWidget();
        placeholder->setFixedSize(420, 170);
        placeholder->setStyleSheet("background-color: transparent");
        placeholder->setProperty("isPlaceholder", true);
        layout->addWidget(placeholder, (dataCount + i) / 3, (dataCount + i) % 3, Qt::AlignCenter);
        placeholderWidgets.append(placeholder);
    }
    updatePageInfo();
}

void MainPageManager::createCardComponents(CardComponents &card, const CollectionData &collection)
{   // 创建卡片
    // 主框架
    card.card = new QFrame(mainWindow->centralwidget);
    card.card->setFixedSize(420, 170);
    card.card->setStyleSheet("QFrame{background-color: rgb(242, 236, 244);border-radius: 15px}");
    card.card->setCursor(Qt::PointingHandCursor);
    card.card->installEventFilter(this);
    card.card->setProperty("isCard", true);
    card.card->setProperty("collectionData", QVariant::fromValue(collection));
    auto *layout = new QHBoxLayout(card.card);
    layout->setContentsMargins(0, 0, 0, 0);
    // 图片
    card.coverLabel = new QLabel();
    card.coverLabel->setFixedSize(125, 170);
    card.coverLabel->setStyleSheet("background-color: rgb(242, 236, 244); border-top-left-radius: 15px; border-bottom-left-radius: 15px");
    card.coverLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(card.coverLabel);
    // 信息框架
    auto *infoFrame = new QFrame();
    infoFrame->setStyleSheet("background-color: transparent");
    auto *infoLayout = new QVBoxLayout(infoFrame);
    infoLayout->setSpacing(15);
    infoLayout->setContentsMargins(5, 5, 0, 0);
    // 标题
    card.titleLabel = new QLabel();
    card.titleLabel->setMaximumHeight(30);
    card.titleLabel->setFont(QFont("Microsoft YaHei", 13));
    infoLayout->addWidget(card.titleLabel);
    // 进度
    card.progressLabel = new QLabel();
    card.progressLabel->setFont(QFont("Microsoft YaHei", 10));
    infoLayout->addWidget(card.progressLabel);
    infoLayout->addStretch();
    // 按钮
    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 0, 15, 15);
    buttonLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    // 状态
    card.moreButton = new QPushButton();
    card.moreButton->setFixedSize(40, 40);
    card.moreButton->setStyleSheet("QPushButton {background-color: rgb(242, 236, 244); border-radius: 20px}"
                                   "QPushButton:hover {background-color: rgb(216, 207, 232)}");
    card.moreButton->setIcon(QIcon("icons/more.png"));
    buttonLayout->addWidget(card.moreButton);
    // 选集.进度
    card.episodeButton = new QPushButton();
    card.episodeButton->setFixedSize(60, 40);
    card.episodeButton->setFont(QFont("Microsoft YaHei", 10));
    buttonLayout->addWidget(card.episodeButton);
    infoLayout->addLayout(buttonLayout);
    layout->addWidget(infoFrame);
    // 填充内容
    if (collection.subject_images_common.isEmpty()) {
        card.coverLabel->setText("暂无图片");
        card.coverLabel->setStyleSheet("QLabel {color: gray}");
    } else ImageUtil::loadImageWithCache(cacheImageUtil, collection.subject_images_common, card.coverLabel, 40, false, true);
    card.titleLabel->setText(collection.subject_name_cn.isEmpty() ? collection.subject_name : collection.subject_name_cn);
    card.progressLabel->setText(computeProgressText(collection, airdatesJson));
    card.card->setProperty("progressLabel", QVariant::fromValue(card.progressLabel));
    if (collection.subject_type == 2) card.episodeButton->setText("选集");
    else if (collection.subject_type == 4) card.episodeButton->setText("");
    else card.episodeButton->setText("进度");
    if (collection.subject_type == 4) card.episodeButton->setStyleSheet("QPushButton {background-color: rgb(242, 236, 244); border-radius: 20px}");
    else card.episodeButton->setStyleSheet("QPushButton {background-color: rgb(242, 236, 244); border-radius: 20px}"
                                          "QPushButton:hover {background-color: rgb(216, 207, 232)}");
    // 连接信号
    connect(card.moreButton, &QPushButton::clicked, this, [this, card]() mutable {
        const auto data = card.card->property("collectionData").value<CollectionData>();
        StatusSelector::showStatusSelector(card.moreButton, currentSubjectType, data.type, data.subject_id, bangumiAPI, dbManager, [this](int) {loadCollections(currentSubjectType, currentStatusType, false);}, -37);});
    connect(card.episodeButton, &QPushButton::clicked, this, [this, card]() mutable {
        const auto data = card.card->property("collectionData").value<CollectionData>();
        if (data.subject_type == 4) showDetailPage(data, card.progressLabel->text());
        else showEpisodePage(data);
    });
}

void MainPageManager::clearDisplayArea()
{   // 清理布局
    const auto layout = mainWindow->gridLayout_2;
    while (const QLayoutItem *item = layout->takeAt(0)) {
        item->widget()->deleteLater();
        delete item;
    }
    placeholderWidgets.clear();
}

void MainPageManager::updatePageInfo() const
{   // 分页
    const int totalPages = static_cast<int>(qMax(1LL, (filteredCollections.size() + itemsPerPage - 1) / itemsPerPage));
    mainWindow->pages_Button->setText(QString("%1/%2").arg(currentPage).arg(totalPages));
    mainWindow->previous_Button->setEnabled(currentPage > 1);
    mainWindow->next_Button->setEnabled(currentPage < totalPages);
}

void MainPageManager::previousPage()
{   // 上一页
    if (currentPage > 1) {
        cacheImageUtil->clearPendingDownloads();
        --currentPage;
        displayCurrentPage();
    }
}

void MainPageManager::nextPage()
{   // 下一页
    const int totalPages = static_cast<int>(qMax(1, (filteredCollections.size() + itemsPerPage - 1) / itemsPerPage));
    if (currentPage < totalPages) {
        cacheImageUtil->clearPendingDownloads();
        ++currentPage;
        displayCurrentPage();
    }
}

void MainPageManager::showDetailPage(const CollectionData &collectionData, const QString &progressText)
{   // 跳转详情页面
    emit showDetailPageRequested(collectionData, progressText);
}

void MainPageManager::showEpisodePage(const CollectionData &collectionData)
{   // 跳转选集页面
    emit showEpisodePageRequested(collectionData);
}