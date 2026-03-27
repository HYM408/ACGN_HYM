#include "main_page.h"
#include "config.h"
#include "sql/sql.h"
#include "utils/menu_util.h"
#include "utils/image_util.h"
#include "utils/progress_util.h"
#include "utils/cache_image_util.h"
#include "utils/game_monitor_util.h"
#include "utils/context_menu_util.h"

static constexpr int itemsPerPage = 12;
static const QVector<int> posToType = {1, 3, 2, 4, 5};

MainPageManager::MainPageManager(Ui::MainWindow *mainWindow, CacheImageUtil *cacheImageUtil, BangumiAPI *bangumiAPI, DatabaseManager *dbManager, GameMonitorUtil *gameMonitor) : QObject(nullptr), mainWindow(mainWindow), dbManager(dbManager), cacheImageUtil(cacheImageUtil), bangumiAPI(bangumiAPI), gameMonitor(gameMonitor)
{
    m_statusButtons = {mainWindow->statusBtnWish, mainWindow->statusBtnWatched, mainWindow->statusBtnWatching, mainWindow->statusBtnOnHold, mainWindow->statusBtnDropped};
    m_statusLabels = {mainWindow->statusLabelWish, mainWindow->statusLabelWatched, mainWindow->statusLabelWatching, mainWindow->statusLabelOnHold, mainWindow->statusLabelDropped};
    m_statusCountLabels = {mainWindow->statusCountWish, mainWindow->statusCountWatched, mainWindow->statusCountWatching, mainWindow->statusCountOnHold, mainWindow->statusCountDropped};
    applyTheme();
    initStatus();
    setupConnections();
    loadCollections(2, 3, true);
}

void MainPageManager::applyTheme()
{   // 主题
    m_color2 = getColor("color2", QColor("#f2ecf4"));
    m_color3 = getColor("color3", QColor("#e1dbe4"));
    const QColor color4 = getColor("color4", QColor("#e9ddff"));
    const QColor color5 = getColor("color5", QColor("#ddd0f6"));
    const QColor color6 = getColor("color6", QColor("#f2ecf4"));
    const QString color6A = QString("rgba(%1, %2, %3, 30)").arg(color6.red()).arg(color6.green()).arg(color6.blue());
    mainWindow->toolbar_frame->setStyleSheet(QString("QFrame {background-color: %1}").arg(m_color2.name()));
    mainWindow->btnSearch->setStyleSheet(QString("QPushButton {background-color: %1; border: none; border-radius:15px}"
                                                     "QPushButton:hover {background-color: %2}").arg(color4.name(), color5.name()));
    mainWindow->btnSettings->setStyleSheet(QString("QPushButton {border: none; border-radius:15px}"
                                                     "QPushButton:hover {background-color: %1}").arg(m_color3.name()));
    const QString buttonStyle = QString("QPushButton {background-color: %1; border: none; border-radius:15px}"
                                        "QPushButton:checked {background-color: %2}"
                                        "QPushButton:hover {background-color: %3}"
                                        "QPushButton:checked:hover {background-color: %4}").arg(m_color2.name(), color4.name(), m_color3.name(), color5.name());
    for (auto button : {mainWindow->btnAnime, mainWindow->btnNovel, mainWindow->btnGame, mainWindow->btnComic, mainWindow->btnDownload}) button->setStyleSheet(buttonStyle);
    const QString StatusCountsQLabelStyle = QString("QLabel {border:none; border-radius:15px; background-color: %1}").arg(m_color2.name());
    for (auto QLabel : m_statusCountLabels) QLabel->setStyleSheet(StatusCountsQLabelStyle);
    const QString StatusCountsQButtonStyle = QString("QPushButton {border: none}"
                                                     "QPushButton:hover {background-color: %1}"
                                                     "QPushButton:checked {border-bottom: 3px solid %2}").arg(color6A, color6.name());
    for (auto button : m_statusButtons) button->setStyleSheet(StatusCountsQButtonStyle);
}

void MainPageManager::initStatus()
{   // 初始化状态
    for (int i = 0; i < m_statusButtons.size(); ++i) {
        int statusType = posToType[i];
        connect(m_statusButtons[i], &QPushButton::clicked, this, [this, statusType] {loadCollections(m_currentSubjectType, statusType, true);});
    }
    for (const auto label : m_statusLabels) label->setAttribute(Qt::WA_TransparentForMouseEvents);
}

void MainPageManager::setupConnections()
{   // 连接
    connect(mainWindow->btnAnime, &QPushButton::clicked, this, [this] {switchCategory(2, "动画");});
    connect(mainWindow->btnNovel, &QPushButton::clicked, this, [this] {switchCategory(7, "小说");});
    connect(mainWindow->btnGame, &QPushButton::clicked, this, [this] {switchCategory(4, "游戏");});
    connect(mainWindow->btnComic, &QPushButton::clicked, this, [this] {switchCategory(8, "漫画");});
    connect(mainWindow->btnRefresh, &QPushButton::clicked, this, [this] {loadCollections(m_currentSubjectType, m_currentStatusType, true);});
    connect(mainWindow->btnPrev, &QPushButton::clicked, this, &MainPageManager::previousPage);
    connect(mainWindow->btnNext, &QPushButton::clicked, this, &MainPageManager::nextPage);
    connect(mainWindow->lineEditSearch, &QLineEdit::textChanged, this, [this](const QString &text) {onSearchTextChanged(text, true, true);});
    setupLineEditCustomContextMenu(mainWindow->lineEditSearch, CMO_Default);
}

bool MainPageManager::eventFilter(QObject *obj, QEvent *event)
{   // 鼠标事件
    if (obj->property("isCard").toBool() && event->type() == QEvent::MouseButtonPress) {
        const auto *frame = qobject_cast<QFrame*>(obj);
        emit showDetailPageRequested(frame->property("collectionData").value<CollectionData>().subject_id, frame->property("progressLabel").value<QLabel*>()->text());
        return true;
    }
    return QObject::eventFilter(obj, event);
}

void MainPageManager::switchCategory(const int subjectType, const QString &title)
{   // 类别切换
    emit showMainPageRequested();
    mainWindow->labelCategoryTitle->setText(title);
    loadCollections(subjectType, m_currentStatusType, true);
}

void MainPageManager::onSearchTextChanged(const QString &text, const bool resetToFirstPage, const bool displayPage)
{   // 搜索当前状态
    if (resetToFirstPage) m_currentPage = 1;
    if (text.trimmed().isEmpty()) m_filteredCollections = m_allCollections;
    else {
        QString searchLower = text.toLower();
        m_filteredCollections.clear();
        std::copy_if(m_allCollections.begin(), m_allCollections.end(), std::back_inserter(m_filteredCollections), [&searchLower](const CollectionData &collection) {return collection.subject_name_cn.toLower().contains(searchLower) || collection.subject_name.toLower().contains(searchLower);});
    }
    if (displayPage) displayCurrentPage();
}

void MainPageManager::loadCollections(const int subjectType, const int statusType, const bool resetToFirstPage)
{   // 加载数据
    static const QMap<int, QMap<int, QString>> statusNamesMap = {
        {2, {{1, "想看"}, {2, "看过"}, {3, "在看"}, {4, "搁置"}, {5, "抛弃"}}},
        {4, {{1, "想玩"}, {2, "玩过"}, {3, "在玩"}, {4, "搁置"}, {5, "抛弃"}}},
        {7, {{1, "想读"}, {2, "读过"}, {3, "在读"}, {4, "搁置"}, {5, "抛弃"}}},
        {8, {{1, "想读"}, {2, "读过"}, {3, "在读"}, {4, "搁置"}, {5, "抛弃"}}}};
    const bool typeChanged = subjectType != m_currentSubjectType || statusType != m_currentStatusType;
    m_currentSubjectType = subjectType;
    m_currentStatusType = statusType;
    const QJsonObject statusCounts = DatabaseManager::getStatusCountsBySubjectType(subjectType);
    for (int i = 0; i < m_statusButtons.size(); ++i) {
        int mappedType = posToType[i];
        m_statusLabels[i]->setText(statusNamesMap[subjectType][mappedType]);
        m_statusCountLabels[i]->setText(QString::number(statusCounts.value(QString::number(mappedType)).toInt()));
    }
    m_allCollections = DatabaseManager::getCollectionBySubjectTypeAndType(subjectType, statusType);
    if (!m_allCollections.isEmpty()) {
        QList<int> subjectIds;
        for (const auto &col : m_allCollections) subjectIds.append(col.subject_id);
        m_airdatesJson = dbManager->getEpisodeAirdates(subjectIds);
    } else m_airdatesJson = QJsonObject();
    if (!typeChanged) onSearchTextChanged(mainWindow->lineEditSearch->text(), false, false);
    else {
        m_filteredCollections = m_allCollections;
        if (resetToFirstPage) m_currentPage = 1;
        mainWindow->lineEditSearch->clear();
    }
    displayCurrentPage();
    updatePageInfo();
}

void MainPageManager::displayCurrentPage()
{   // 显示标签
    const auto layout = mainWindow->gridLayout_2;
    while (const QLayoutItem *item = layout->takeAt(0)) {
        item->widget()->deleteLater();
        delete item;
    }
    m_placeholderWidgets.clear();
    layout->setColumnMinimumWidth(0, 420);
    layout->setColumnMinimumWidth(1, 420);
    layout->setColumnMinimumWidth(2, 420);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    const int start = (m_currentPage - 1) * itemsPerPage;
    const int dataCount = static_cast<int>(qMin(itemsPerPage, m_filteredCollections.size() - start));
    for (int i = 0; i < dataCount; ++i) {
        auto *card = createCardComponents(m_filteredCollections[start + i]);
        layout->addWidget(card, i / 3, i % 3, Qt::AlignCenter);
        card->show();
    }
    for (int i = 0; i + dataCount < 3; ++i) {
        const auto placeholder = new QWidget();
        placeholder->setFixedWidth(420);
        placeholder->setProperty("isPlaceholder", true);
        layout->addWidget(placeholder, (dataCount + i) / 3, (dataCount + i) % 3, Qt::AlignCenter);
        m_placeholderWidgets.append(placeholder);
    }
    updatePageInfo();
}

QFrame* MainPageManager::createCardComponents(CollectionData &collection)
{   // 创建卡片
    // 主框架
    auto *card = new QFrame(mainWindow->centralwidget);
    card->setFixedSize(420, 170);
    card->setStyleSheet(QString("background-color: %1;border-radius: 15px").arg(m_color2.name()));
    card->setCursor(Qt::PointingHandCursor);
    card->installEventFilter(this);
    card->setProperty("isCard", true);
    card->setProperty("collectionData", QVariant::fromValue(collection));
    auto *layout = new QHBoxLayout(card);
    layout->setContentsMargins(0, 0, 0, 0);
    // 图片
    auto *coverLabel = new QLabel();
    coverLabel->setFixedSize(125, 170);
    layout->addWidget(coverLabel);
    // 信息框架
    auto *infoFrame = new QFrame();
    infoFrame->setStyleSheet("background-color: transparent");
    auto *infoLayout = new QVBoxLayout(infoFrame);
    infoLayout->setSpacing(15);
    infoLayout->setContentsMargins(5, 5, 0, 0);
    // 标题
    auto *titleLabel = new QLabel();
    titleLabel->setFont(QFont("Microsoft YaHei", 13));
    infoLayout->addWidget(titleLabel);
    // 进度
    auto *progressLabel = new QLabel();
    progressLabel->setFont(QFont("Microsoft YaHei", 10));
    infoLayout->addWidget(progressLabel);
    infoLayout->addStretch();
    // 按钮
    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 0, 15, 15);
    buttonLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    // 状态
    auto *statusButton = new QPushButton();
    statusButton->setFixedSize(40, 40);
    statusButton->setStyleSheet(QString("QPushButton {border-radius: 20px}"
                                        "QPushButton:hover {background-color: %1}").arg(m_color3.name()));
    statusButton->setIcon(QIcon("icons/more.png"));
    buttonLayout->addWidget(statusButton);
    // 选集/进度/启动
    auto *episodeButton = new QPushButton();
    episodeButton->setFixedSize(60, 40);
    episodeButton->setFont(QFont("Microsoft YaHei", 10));
    buttonLayout->addWidget(episodeButton);
    infoLayout->addLayout(buttonLayout);
    layout->addWidget(infoFrame);
    // 填充内容
    const QString imageUrl = QString("https://api.bgm.tv/v0/subjects/%1/image?type=common").arg(collection.subject_id);
    ImageUtil::loadImageWithCache(cacheImageUtil, imageUrl, coverLabel, 40, false, true, QString("s%1.jpg").arg(collection.subject_id));
    titleLabel->setText(collection.subject_name_cn.isEmpty() ? collection.subject_name : collection.subject_name_cn);
    if (collection.subject_type == 7 || collection.subject_type == 8) collection.subject_volumes = dbManager->countSubjectRelations(collection.subject_id);
    progressLabel->setText(computeProgressText(collection, m_airdatesJson));
    card->setProperty("progressLabel", QVariant::fromValue(progressLabel));
    if (collection.subject_type == 2) episodeButton->setText("选集");
    else if (collection.subject_type == 4) episodeButton->setText("启动");
    else episodeButton->setText("进度");
    episodeButton->setStyleSheet(QString("QPushButton {border-radius: 20px}"
                                         "QPushButton:hover {background-color: %1}").arg(m_color3.name()));
    // 连接信号
    connect(statusButton, &QPushButton::clicked, this, [this, card, statusButton] {
        const auto data = card->property("collectionData").value<CollectionData>();
        StatusSelector::showStatusSelector(statusButton, m_currentSubjectType, data.type, data.subject_id, bangumiAPI, dbManager, [this](int) {loadCollections(m_currentSubjectType, m_currentStatusType, false);});
    });
    if (collection.subject_type == 4) connect(episodeButton, &QPushButton::clicked, this, [this, subjectId = collection.subject_id] {gameMonitor->startGame(subjectId);});
    else connect(episodeButton, &QPushButton::clicked, this, [this, data = collection] {emit showEpisodePageRequested(data);});
    return card;
}

void MainPageManager::updatePageInfo() const
{   // 分页
    const int totalPages = static_cast<int>(qMax(1LL, (m_filteredCollections.size() + itemsPerPage - 1) / itemsPerPage));
    mainWindow->labelPage->setText(QString("%1/%2").arg(m_currentPage).arg(totalPages));
    mainWindow->btnPrev->setEnabled(m_currentPage > 1);
    mainWindow->btnNext->setEnabled(m_currentPage < totalPages);
}

void MainPageManager::previousPage()
{   // 上一页
    if (m_currentPage < 1) return;
    --m_currentPage;
    displayCurrentPage();
}

void MainPageManager::nextPage()
{   // 下一页
    if (m_currentPage > static_cast<int>(qMax(1, (m_filteredCollections.size() + itemsPerPage - 1) / itemsPerPage))) return;
    ++m_currentPage;
    displayCurrentPage();
}