#include "main_page.h"
#include <QTimer>
#include <QLabel>
#include <QJsonArray>
#include <QJsonObject>
#include <QMouseEvent>
#include "sql.h"
#include "utils/menu_util.h"
#include "utils/image_util.h"
#include "utils/cache_image_util.h"

MainPageManager::MainPageManager(Ui::MainWindow *mainWindow, DatabaseManager *dbManager, CacheImageUtil *cacheImageUtil, BangumiAPI *bangumiAPI): QObject(nullptr), mainWindow(mainWindow), dbManager(dbManager), cacheImageUtil(cacheImageUtil), bangumiAPI(bangumiAPI)
{
    // 字体预热
    QTimer::singleShot(0, [] {warmupFonts();});
    // 初始化状态映射
    statusNamesMap = {
        {2, {{1, "想看"}, {2, "看过"}, {3, "在看"}}},
        {4, {{1, "想玩"}, {2, "玩过"}, {3, "在玩"}}},
        {7, {{1, "想读"}, {2, "读过"}, {3, "在读"}}},
        {8, {{1, "想读"}, {2, "读过"}, {3, "在读"}}}};
    // 初始化卡片池
    initCardPool();
    // 初始化连接
    setupConnections();
}

void MainPageManager::warmupFonts()
{   // 字体预热
    QLabel label;
    label.hide();
    label.setText("♭");
    label.grab();
}

void MainPageManager::initCardPool()
{   // 初始化卡片池
    cardPool.resize(itemsPerPage);
    for (int i = 0; i < itemsPerPage; ++i) {
        CardComponents &components = cardPool[i];
        components.inUse = false;
        // 主框架
        components.card = new QFrame(mainWindow->centralwidget);
        components.card->setFixedSize(420, 170);
        components.card->setStyleSheet("QFrame{background-color: rgb(242, 236, 244);border-radius: 15px}");
        components.card->setCursor(Qt::PointingHandCursor);
        components.card->hide();
        components.card->installEventFilter(this);
        auto *layout = new QHBoxLayout(components.card);
        layout->setContentsMargins(0, 0, 0, 0);
        // 图片
        components.coverLabel = new QLabel();
        components.coverLabel->setFixedSize(125, 170);
        components.coverLabel->setStyleSheet("background-color: rgb(242, 236, 244); border-top-left-radius: 15px; border-bottom-left-radius: 15px");
        components.coverLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(components.coverLabel);
        // 信息框架
        auto *infoFrame = new QFrame();
        infoFrame->setStyleSheet("background-color: transparent");
        auto *infoLayout = new QVBoxLayout(infoFrame);
        infoLayout->setSpacing(8);
        infoLayout->setContentsMargins(5, 5, 0, 0);
        // 标题
        components.titleLabel = new QLabel();
        components.titleLabel->setMaximumHeight(30);
        components.titleLabel->setFont(QFont("Microsoft YaHei", 13));
        infoLayout->addWidget(components.titleLabel);
        // 进度
        components.progressLabel = new QLabel();
        components.progressLabel->setFont(QFont("Microsoft YaHei", 10));
        infoLayout->addWidget(components.progressLabel);
        infoLayout->addStretch();
        // 按钮
        auto *buttonLayout = new QHBoxLayout();
        buttonLayout->setContentsMargins(0, 0, 15, 15);
        buttonLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
        // 状态
        components.moreButton = new QPushButton();
        components.moreButton->setFixedSize(40, 40);
        components.moreButton->setStyleSheet("QPushButton {background-color: rgb(242, 236, 244); border-radius: 20px}"
                                             "QPushButton:hover {background-color: rgb(216, 207, 232)}");
        components.moreButton->setIcon(QIcon("icons/more.png"));
        buttonLayout->addWidget(components.moreButton);
        // 选集
        components.episodeButton = new QPushButton("选集");
        components.episodeButton->setFixedSize(60, 40);
        components.episodeButton->setFont(QFont("Microsoft YaHei", 10));
        components.episodeButton->setStyleSheet("QPushButton {background-color: rgb(242, 236, 244); border-radius: 20px}"
                                                "QPushButton:hover {background-color: rgb(216, 207, 232)}");
        buttonLayout->addWidget(components.episodeButton);
        infoLayout->addLayout(buttonLayout);
        layout->addWidget(infoFrame);
        // 连接信号
        connect(components.moreButton, &QPushButton::clicked, this, [this, i] {
            auto data = cardPool[i].card->property("collectionData").value<CollectionData>();
            StatusSelector::showStatusSelector(cardPool[i].moreButton, currentSubjectType, data.type, data.subject_id, bangumiAPI, [this](int) {loadCollections(currentSubjectType, currentStatusType, false);}, -37);});
        connect(components.episodeButton, &QPushButton::clicked, this, [this, i] {showEpisodePage(cardPool[i].card->property("collectionData").value<CollectionData>());});
    }
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
    connect(mainWindow->animation_Button, &QPushButton::clicked, this, [this]() {switchCategory(2, "动画");});
    connect(mainWindow->novel_Button, &QPushButton::clicked, this, [this]() {switchCategory(7, "小说");});
    connect(mainWindow->game_Button, &QPushButton::clicked, this, [this]() {switchCategory(4, "游戏");});
    connect(mainWindow->comic_Button, &QPushButton::clicked, this, [this]() {switchCategory(8, "漫画");});
    connect(mainWindow->refresh_Button, &QPushButton::clicked, this, [this]() {loadCollections(currentSubjectType, currentStatusType, true);});
    connect(mainWindow->previous_Button, &QPushButton::clicked, this, &MainPageManager::previousPage);
    connect(mainWindow->next_Button, &QPushButton::clicked, this, &MainPageManager::nextPage);
    connect(mainWindow->searchlist_lineEdit, &QLineEdit::textChanged, this, &MainPageManager::onSearchTextChanged);
    mainWindow->animation_Button->setChecked(true);
    loadCollections(2, 3, true);
}

bool MainPageManager::eventFilter(QObject *obj, QEvent *event)
{   // 鼠标事件
    for (auto &card : cardPool) {
        if (card.inUse && card.card == obj && event->type() == QEvent::MouseButtonPress) {
            if (dynamic_cast<QMouseEvent*>(event)->button() == Qt::LeftButton) {
                showDetailPage(card.card->property("collectionData").value<CollectionData>());
                return true;
            }
        }
    }
    for (auto &[statusType, info] : statusFrames.toStdMap()) {
        if (info.frame == obj) {
            bool isSelected = statusType == currentStatusType;
            QString baseStyle = isSelected ? ";border-bottom: 3px solid rgb(103, 79, 165)" : "";
            if (event->type() == QEvent::Enter) {
                info.frame->setStyleSheet(QString("QFrame{background-color: rgba(103, 79, 165, 30)%1}").arg(baseStyle));
                return true;
            }
            if (event->type() == QEvent::Leave) {
                info.frame->setStyleSheet(QString("QFrame{background-color: transparent%1}").arg(baseStyle));
                return true;
            }
            if (event->type() == QEvent::MouseButtonPress) {
                if (dynamic_cast<QMouseEvent*>(event)->button() == Qt::LeftButton) {
                    loadCollections(currentSubjectType, statusType, true);
                    return true;
                }
            }
        }
    }
    return QObject::eventFilter(obj, event);
}

void MainPageManager::switchCategory(int subjectType, const QString &title)
{   // 类别切换
    emit showMainPageRequested();
    currentSubjectType = subjectType;
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

void MainPageManager::loadCollections(int subjectType, int statusType, bool resetToFirstPage)
{   // 加载数据
    currentSubjectType = subjectType;
    currentStatusType = statusType;
    const auto &statusMap = statusNamesMap[subjectType];
    QJsonObject statusCounts = DatabaseManager::getStatusCountsBySubjectType(subjectType);
    for (int i = 1; i <= 3; ++i) statusFrames[i].statusButton->setText(statusMap[i]);
    for (auto &[key, info] : statusFrames.toStdMap()) {
        int count = statusCounts.value(QString::number(key)).toInt();
        info.countButton->setText(QString::number(count));
        bool isHovered = info.frame->underMouse();
        bool selected = key == statusType;
        QString backgroundColor = isHovered ? "rgba(103, 79, 165, 30)" : "transparent";
        QString borderStyle = selected ? ";border-bottom: 3px solid rgb(103, 79, 165)" : "";
        info.frame->setStyleSheet(QString("QFrame{background-color: %1%2}").arg(backgroundColor, borderStyle));
        info.statusButton->setChecked(selected);
    }
    allCollections.clear();
    QJsonArray jsonArray = DatabaseManager::getCollectionBySubjectTypeAndType(subjectType, statusType);
    for (const auto &value : jsonArray) {
        QJsonObject row = value.toObject();
        QVariantList tagsList;
        for (const auto &tagValue : row["subject_tags"].toArray()) tagsList.append(tagValue.toVariant());
        allCollections.append({
            row["subject_id"].toInt(),
            row["subject_name"].toString(),
            row["subject_name_cn"].toString(),
            row["subject_eps"].toInt(),
            row["subject_images_common"].toString(),
            row["vol_status"].toInt(),
            row["type"].toInt(),
            row["subject_type"].toInt(),
            tagsList
        });
    }
    filteredCollections = allCollections;
    if (resetToFirstPage) currentPage = 1;
    mainWindow->searchlist_lineEdit->clear();
    allCollections.isEmpty() ? clearDisplayArea() : displayCurrentPage();
    updatePageInfo();
}

void MainPageManager::displayCurrentPage()
{   // 显示标签
    clearDisplayArea();
    auto layout = mainWindow->gridLayout_2;
    layout->setColumnMinimumWidth(0, 420);
    layout->setColumnMinimumWidth(1, 420);
    layout->setColumnMinimumWidth(2, 420);
    layout->setVerticalSpacing(20);
    layout->setHorizontalSpacing(40);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    int start = (currentPage - 1) * itemsPerPage;
    int dataCount = static_cast<int>(qMin(itemsPerPage, filteredCollections.size() - start));
    for (int i = 0; i < dataCount; ++i) {
        auto &card = cardPool[i];
        card.inUse = true;
        setupCardComponents(i, filteredCollections[start + i]);
        layout->addWidget(card.card, i / 3, i % 3, Qt::AlignCenter);
        card.card->show();
    }
    for (int i = 0; i < 3 * ((dataCount + 2) / 3) - dataCount; ++i) {
        auto placeholder = new QWidget();
        placeholder->setFixedSize(420, 170);
        placeholder->setStyleSheet("background-color: transparent");
        placeholder->setProperty("isPlaceholder", true);
        layout->addWidget(placeholder, (dataCount + i) / 3, (dataCount + i) % 3, Qt::AlignCenter);
        placeholderWidgets.append(placeholder);
    }
    updatePageInfo();
}

void MainPageManager::setupCardComponents(int cardIndex, const CollectionData &collection)
{   // 设置卡片
    auto &card = cardPool[cardIndex];
    card.card->setProperty("collectionData", QVariant::fromValue(collection));
    ImageUtil::loadImageWithCache(cacheImageUtil, collection.subject_images_common, card.coverLabel, 40, false, true);
    card.titleLabel->setText(collection.subject_name_cn.isEmpty() ? collection.subject_name : collection.subject_name_cn);
    card.progressLabel->setText(collection.subject_eps > 0 ? QString("全%1话").arg(collection.subject_eps) : "全--话");
}

void MainPageManager::clearDisplayArea()
{   // 清理布局
    auto layout = mainWindow->gridLayout_2;
    while (QLayoutItem *item = layout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            if (widget->property("isPlaceholder").toBool()) widget->deleteLater();
            else widget->hide();
        }
        delete item;
    }
    placeholderWidgets.clear();
    for (auto &card : cardPool) {
        if (!card.inUse) continue;
        card.coverLabel->clear();
        card.titleLabel->clear();
        card.progressLabel->clear();
        card.card->setProperty("collectionData", QVariant());
        card.card->hide();
        card.inUse = false;
    }
}

void MainPageManager::updatePageInfo() const
{   // 分页
    int totalPages = static_cast<int>(qMax(1LL, (filteredCollections.size() + itemsPerPage - 1) / itemsPerPage));
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
    int totalPages = static_cast<int>(qMax(1, (filteredCollections.size() + itemsPerPage - 1) / itemsPerPage));
    if (currentPage < totalPages) {
        cacheImageUtil->clearPendingDownloads();
        ++currentPage;
        displayCurrentPage();
    }
}

void MainPageManager::showDetailPage(const CollectionData &collectionData)
{   // 跳转详情页面
    emit showDetailPageRequested(collectionData);
}

void MainPageManager::showEpisodePage(const CollectionData &collectionData)
{   // 跳转选集页面
    emit showEpisodePageRequested(collectionData);
}