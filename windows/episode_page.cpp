#include "episode_page.h"
#include <QLabel>
#include <QPainter>
#include <qpointer.h>
#include <QResizeEvent>
#include "sql.h"
#include "api/bangumi_api.h"

EpisodeOverlay::EpisodeOverlay(QWidget *parent) : QWidget(parent)
{
    episodeContainer = new QWidget(this);
    ui.setupUi(episodeContainer);
    episodeContainer->setFixedSize(600, 200);
    episodeListView = new QListView();
    episodeModel = new QStandardItemModel(this);
    episodeDelegate = new EpisodeDelegate(this);
    episodeListView->setModel(episodeModel);
    episodeListView->setItemDelegate(episodeDelegate);
    episodeListView->setViewMode(QListView::IconMode);
    episodeListView->setMovement(QListView::Static);
    episodeListView->setResizeMode(QListView::Adjust);
    episodeListView->setSpacing(SPACING);
    episodeListView->setUniformItemSizes(true);
    episodeListView->setSelectionMode(QListView::SingleSelection);
    episodeListView->setGridSize(QSize(BUTTON_SIZE + SPACING, BUTTON_SIZE + SPACING));
    episodeListView->setFlow(QListView::LeftToRight);
    episodeListView->setWrapping(true);
    episodeListView->setStyleSheet("QListView {border: none; background-color: transparent; outline: none}"
                                   "QListView::item {border: none}"
                                   "QListView::item:selected {background-color: transparent}");
    ui.gridLayout_3->addWidget(episodeListView, 0, 0, 1, 1);
    ui.gridLayout_3->setContentsMargins(10, 10, 10, 10);
    connect(ui.pushButton_14, &QPushButton::clicked, this, &EpisodeOverlay::closeOverlay);
    connect(ui.pushButton_15, &QPushButton::clicked, this, &EpisodeOverlay::onMarkAllWatchedClicked);
    connect(episodeDelegate, &EpisodeDelegate::episodeClicked, this, &EpisodeOverlay::onEpisodeItemClicked);
    installEventFilter(this);
}

EpisodeDelegate::EpisodeDelegate(QObject *parent): QStyledItemDelegate(parent) {}

void EpisodeOverlay::setManagers(DatabaseManager *db, BangumiAPI *api)
{   // 初始化实例
    dbManager = db;
    bangumiAPI = api;
}

void EpisodeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{   // 绘制
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    QRect rect = option.rect.adjusted(2, 2, -2, -2);
    QByteArray jsonData = index.data(EpisodeOverlay::EpisodeDataRole).toByteArray();
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    QJsonObject episode = doc.object();
    int epType = episode.value("episode_type").toInt();
    bool isHovered = option.state & QStyle::State_MouseOver;
    int colVal = episode.value("collection_type").toInt();
    QColor bgColor, textColor;
    if (colVal == 2) {
        bgColor = isHovered ? QColor(66, 155, 70) : QColor(76, 175, 80);
        textColor = Qt::white;
    } else {
        bgColor = isHovered ? QColor(240, 240, 240) : Qt::white;
        textColor = Qt::black;
    }
    painter->setBrush(bgColor);
    painter->setPen(QPen(QColor(221, 221, 221), 1));
    painter->drawRoundedRect(rect, 5, 5);
    painter->setPen(textColor);
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(9);
    painter->setFont(font);
    QString epText;
    if (epType == 0) {
        double epValue = episode.value("ep").toDouble();
        if (qAbs(epValue - qRound(epValue)) < 0.001) epText = QString::number(qRound(epValue));
        else epText = QString::number(epValue, 'f', 1);
    } else {
        double sortValue = episode.value("sort").toDouble();
        epText = QString::number(sortValue);
    }
    painter->drawText(rect, Qt::AlignCenter, epText);
    if (epType != 0) {
        QFont smallFont = painter->font();
        smallFont.setPointSize(6);
        painter->setFont(smallFont);
        QColor typeColor;
        QString typeText;
        switch (epType) {
            case 1: typeColor = QColor(255, 152, 0); typeText = "SP"; break;
            case 2: typeColor = QColor(33, 150, 243); typeText = "OP"; break;
            case 3: typeColor = QColor(156, 39, 176); typeText = "ED"; break;
            default: typeColor = QColor(255, 87, 34); typeText = "?"; break;
        }
        painter->setPen(typeColor);
        painter->drawText(rect.adjusted(-2, 2, -2, 2), Qt::AlignTop | Qt::AlignRight, typeText);
    }
    painter->restore();
}

QSize EpisodeDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {return {40, 40};}

bool EpisodeDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{   // 鼠标事件
    if (event->type() == QEvent::MouseButtonRelease) {
        auto *mouseEvent = dynamic_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            QByteArray jsonData = index.data(EpisodeOverlay::EpisodeDataRole).toByteArray();
            QJsonDocument doc = QJsonDocument::fromJson(jsonData);
            emit episodeClicked(doc.object());
            return true;
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

void EpisodeOverlay::showWithCollectionData(const CollectionData &collData)
{   // 显示叠加层
    show();
    ui.pushButton_13->setText(collData.subject_name_cn.isEmpty() ? collData.subject_name : collData.subject_name_cn);
    QJsonObject jsonData;
    jsonData["subject_id"] = collData.subject_id;
    jsonData["subject_name"] = collData.subject_name;
    jsonData["subject_name_cn"] = collData.subject_name_cn;
    this->collectionData = jsonData;
    loadEpisodesData();
}

void EpisodeOverlay::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0, 0, 0, 150));
}

bool EpisodeOverlay::eventFilter(QObject *obj, QEvent *event)
{   // 关闭事件
    if (event->type() == QEvent::MouseButtonPress) {
        auto *mouseEvent = dynamic_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton && !episodeContainer->geometry().contains(mouseEvent->pos())) {
            closeOverlay();
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void EpisodeOverlay::resizeEvent(QResizeEvent *event)
{   // 窗口大小改变
    QWidget::resizeEvent(event);
    int x = (width() - episodeContainer->width()) / 2;
    int y = (height() - episodeContainer->height()) / 2;
    episodeContainer->move(x, y);
}

void EpisodeOverlay::loadEpisodesData()
{   // 加载剧集数据
    int subjectId = collectionData.value("subject_id").toInt();
    episodeModel->clear();
    episodes = DatabaseManager::getEpisodesBySubjectId(subjectId);
    if (!episodes.isEmpty()) {
        updateEpisodeView();
        return;
    }
    int localSubjectId = subjectId;
    BangumiAPI* localBangumiAPI = bangumiAPI;
    QPointer guard(this);
    QJsonArray apiEpisodes = localBangumiAPI->getSubjectEpisodes(localSubjectId);
    if (!apiEpisodes.isEmpty()) DatabaseManager::insertManyEpisodes(localSubjectId, apiEpisodes);
    if (!guard) return;
    if (!apiEpisodes.isEmpty()) {
        episodes = DatabaseManager::getEpisodesBySubjectId(localSubjectId);
        updateEpisodeView();
    } else {
        episodeListView->hide();
        if (!noEpisodesLabel) {
            noEpisodesLabel = new QLabel("暂无剧集信息", episodeContainer);
            noEpisodesLabel->setAlignment(Qt::AlignCenter);
            noEpisodesLabel->setStyleSheet("color: #666");
            ui.gridLayout_3->addWidget(noEpisodesLabel, 0, 0);
        }
        episodeContainer->setFixedHeight(MIN_CONTAINER_HEIGHT);
        resizeEvent(nullptr);
    }
}

void EpisodeOverlay::updateEpisodeView()
{   // 更新ui
    for (const QJsonValueRef &value : episodes) {
        auto *item = new QStandardItem();
        QJsonObject episodeObj = value.toObject();
        item->setData(QJsonDocument(episodeObj).toJson(), EpisodeDataRole);
        QString name = episodeObj.value("name_cn").toString();
        if (name.isEmpty()) name = episodeObj.value("name").toString();
        item->setToolTip(name);
        episodeModel->appendRow(item);
    }
    int itemHeight = BUTTON_SIZE + SPACING;
    int headerHeight = 120;
    int totalItems = episodeModel->rowCount();
    int rows = (totalItems + COLUMNS - 1) / COLUMNS;
    int visibleRows = qMin(rows, 15);
    int containerHeight = headerHeight + (visibleRows * itemHeight) + 20;
    containerHeight = qMax(MIN_CONTAINER_HEIGHT, qMin(containerHeight, MAX_CONTAINER_HEIGHT));
    episodeContainer->setFixedHeight(containerHeight);
    ui.scrollArea->setVerticalScrollBarPolicy(rows > visibleRows ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
    resizeEvent(nullptr);
}

void EpisodeOverlay::onEpisodeItemClicked(const QJsonObject &episodeData)
{   // 点击剧集
    int epType = episodeData.value("episode_type").toInt();
    if (epType != 0 && epType != 1) return ;
    closeOverlay();
    emit episodeClicked(collectionData, episodeData);
}

void EpisodeOverlay::onMarkAllWatchedClicked()
{   // 标记全部已看
    if (episodes.isEmpty()) return;
    ui.pushButton_15->setText(tr("标记中..."));
    ui.pushButton_15->setEnabled(false);
    int subjectId = collectionData.value("subject_id").toInt();
    QJsonArray episodeIds;
    for (const QJsonValueRef &value : episodes) episodeIds.append(value.toObject().value("id").toInt());
    QJsonObject apiRequestData{{"episode_id", episodeIds}, {"type", 2}};
    BangumiAPI* localBangumiAPI = bangumiAPI;
    QPointer guard(this);
    bool success = localBangumiAPI->updateSubjectEpisodes(subjectId, apiRequestData);
    if (!success) return;
    DatabaseManager::updateAllEpisodesStatus(subjectId, 2);
    if (!guard) return;
    loadEpisodesData();
    ui.pushButton_15->setText("全部已看");
    ui.pushButton_15->setEnabled(true);
}

void EpisodeOverlay::closeOverlay()
{   // 关闭
    hide();
    if (episodeModel) episodeModel->clear();
    if (noEpisodesLabel) {
        delete noEpisodesLabel;
        noEpisodesLabel = nullptr;
    }
    emit overlayClosed();
}