#include "episode_page.h"
#include <QTimer>
#include <QPainter>
#include <QPointer>
#include <QLineEdit>
#include <QListView>
#include <QJsonArray>
#include <QResizeEvent>
#include <QStandardItemModel>
#include "config.h"
#include "sql/sql.h"
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
    connect(ui.pushButton_15, &QPushButton::clicked, this, &EpisodeOverlay::onUpdateButtonClicked);
    connect(episodeDelegate, &EpisodeDelegate::episodeClicked, this, &EpisodeOverlay::onEpisodeItemClicked);
    applyTheme();
}

EpisodeDelegate::EpisodeDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

void EpisodeOverlay::setManagers(BangumiAPI *api)
{   // 初始化实例
    bangumiAPI = api;
}

void EpisodeOverlay::applyTheme() const
{   // 主题
    const QColor color1 = getColor("color1", QColor("#fdf7ff"));
    ui.frame_2->setStyleSheet(QString("QFrame {background-color: %1}").arg(color1.name()));
}

void EpisodeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{   // 绘制
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    QRect rect = option.rect.adjusted(2, 2, -2, -2);
    auto ep = qvariant_cast<EpisodeData>(index.data(EpisodeOverlay::EpisodeDataRole));
    int epType = ep.episode_type;
    bool isHovered = option.state & QStyle::State_MouseOver;
    int colVal = ep.collection_type;
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
        double epValue = ep.ep;
        if (qAbs(epValue - qRound(epValue)) < 0.001) epText = QString::number(qRound(epValue));
        else epText = QString::number(epValue, 'f', 1);
    } else epText = QString::number(ep.sort);
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
        emit episodeClicked(qvariant_cast<EpisodeData>(index.data(EpisodeOverlay::EpisodeDataRole)));
        return true;
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

void EpisodeOverlay::keyPressEvent(QKeyEvent *event)
{   // 键盘事件
    if (event->key() == Qt::Key_Escape) closeOverlay();
}

bool EpisodeOverlay::eventFilter(QObject *obj, QEvent *event)
{   // 事件过滤器
    if (event->type() == QEvent::MouseButtonPress) {
        if (const auto *edit = qobject_cast<QLineEdit*>(obj)) {
            QTimer::singleShot(0, edit, &QLineEdit::selectAll);
            return false;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void EpisodeOverlay::onUpdateButtonClicked()
{   // 更新按钮点击
    if (collectionData.subject_type == 2) onMarkAllWatchedClicked();
    else onUpdateClicked();
}

void EpisodeOverlay::showWithCollectionData(const CollectionData &collData)
{   // 显示叠加层
    show();
    setFocus();
    ui.pushButton_13->setText(collData.subject_name_cn.isEmpty() ? collData.subject_name : collData.subject_name_cn);
    this->collectionData = collData;
    ui.pushButton_15->setEnabled(true);
    if (collData.subject_type != 2) {
        ui.pushButton_15->setText("更新");
        loadVolEpData();
    } else {
        ui.pushButton_15->setText("全部已看");
        loadEpisodesData();
    }
}

void EpisodeOverlay::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0, 0, 0, 150));
}

void EpisodeOverlay::mousePressEvent(QMouseEvent *event)
{   // 关闭事件
    if (!episodeContainer->geometry().contains(event->pos())) {
        closeOverlay();
        event->accept();
    } else QWidget::mousePressEvent(event);
}

void EpisodeOverlay::resizeEvent(QResizeEvent *event)
{   // 窗口大小改变
    QWidget::resizeEvent(event);
    const int x = (width() - episodeContainer->width()) / 2;
    const int y = (height() - episodeContainer->height()) / 2;
    episodeContainer->move(x, y);
}

void EpisodeOverlay::loadEpisodesData()
{   // 加载剧集数据
    const int subjectId = collectionData.subject_id;
    episodeModel->clear();
    episodes = DatabaseManager::getEpisodesBySubjectId(subjectId);
    if (!episodes.isEmpty()) {
        updateEpisodeView();
        return;
    }
    const QPointer guard(this);
    const QJsonArray apiEpisodes = bangumiAPI->getSubjectEpisodes(subjectId, 3);
    if (!apiEpisodes.isEmpty()) DatabaseManager::insertManyEpisodes(subjectId, apiEpisodes);
    if (!guard) return;
    if (!apiEpisodes.isEmpty()) {
        episodes = DatabaseManager::getEpisodesBySubjectId(subjectId);
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

void EpisodeOverlay::loadVolEpData()
{   // 加载卷话数据
    auto *container = new QWidget(episodeContainer);
    volContainer = container;
    auto *vLayout = new QVBoxLayout(container);
    auto createField = [&](const QString &label, const int value, const int total, QLineEdit* &editPtr) {
        auto *hLayout = new QHBoxLayout;
        hLayout->setSpacing(28);
        auto *labelWidget = new QLabel(label);
        labelWidget->setStyleSheet("font-weight: bold");
        editPtr = new QLineEdit;
        editPtr->setText(QString::number(value));
        editPtr->setFixedWidth(50);
        auto *totalLabel = new QLabel("/ " + QString::number(total));
        hLayout->addWidget(labelWidget);
        hLayout->addWidget(editPtr);
        hLayout->addWidget(totalLabel);
        vLayout->addLayout(hLayout);
    };
    createField("Vol.", collectionData.vol_status, collectionData.subject_volumes, volEdit);
    createField("Chap.", collectionData.ep_status, collectionData.subject_eps, epEdit);
    volEdit->installEventFilter(this);
    epEdit->installEventFilter(this);
    auto *hBox = new QHBoxLayout;
    hBox->addStretch();
    hBox->addWidget(container);
    hBox->addStretch();
    ui.gridLayout_3->addLayout(hBox, 0, 0);
    const int contentHeight = vLayout->sizeHint().height();
    const int containerHeight = qMax(MIN_CONTAINER_HEIGHT, qMin(120 + contentHeight + 20, MAX_CONTAINER_HEIGHT));
    episodeContainer->setFixedHeight(containerHeight);
    resizeEvent(nullptr);
}

void EpisodeOverlay::updateEpisodeView()
{   // 更新ui
    for (const auto &ep : episodes) {
        auto *item = new QStandardItem();
        item->setData(QVariant::fromValue(ep), EpisodeDataRole);
        QString name = ep.name_cn.isEmpty() ? ep.name : ep.name_cn;
        item->setToolTip(name);
        episodeModel->appendRow(item);
    }
    const int itemHeight = BUTTON_SIZE + SPACING;
    const int totalItems = episodeModel->rowCount();
    const int rows = (totalItems + COLUMNS - 1) / COLUMNS;
    const int visibleRows = qMin(rows, 15);
    int containerHeight = 120 + visibleRows * itemHeight + 20;
    containerHeight = qMax(MIN_CONTAINER_HEIGHT, qMin(containerHeight, MAX_CONTAINER_HEIGHT));
    episodeContainer->setFixedHeight(containerHeight);
    ui.scrollArea->setVerticalScrollBarPolicy(rows > visibleRows ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
    resizeEvent(nullptr);
}

void EpisodeOverlay::onEpisodeItemClicked(const EpisodeData &episodeData)
{   // 点击剧集
    const int epType = episodeData.episode_type;
    if (epType != 0 && epType != 1) return;
    closeOverlay();
    emit episodeClicked(collectionData, episodeData);
}

void EpisodeOverlay::onMarkAllWatchedClicked()
{   // 标记全部已看
    if (episodes.isEmpty()) return;
    ui.pushButton_15->setText(tr("标记中..."));
    ui.pushButton_15->setEnabled(false);
    const int subjectId = collectionData.subject_id;
    QJsonArray episodeIds;
    for (const auto &ep : episodes) episodeIds.append(ep.episode_id);
    const QJsonObject apiRequestData{{"episode_id", episodeIds}, {"type", 2}};
    const QPointer guard(this);
    const bool success = bangumiAPI->updateSubjectEpisodes(subjectId, apiRequestData, 3);
    DatabaseManager::updateAllEpisodesStatus(subjectId, 2);
    DatabaseManager::updateCollectionFields(subjectId, {{"ep_status", episodes.size()}}, true);
    if (!guard) return;
    if (!success) {
        ui.pushButton_15->setText(tr("失败"));
        ui.pushButton_15->setEnabled(true);
        return;
    }
    emit collectionDataChanged();
    loadEpisodesData();
    ui.pushButton_15->setText("全部已看");
    ui.pushButton_15->setEnabled(true);
}

void EpisodeOverlay::onUpdateClicked()
{   // 更新进度
    ui.pushButton_15->setText(tr("更新中..."));
    ui.pushButton_15->setEnabled(false);
    const int subjectId = collectionData.subject_id;
    int newEpStatus  = epEdit->text().toInt();
    int newVolStatus = volEdit->text().toInt();
    const QJsonObject apiRequestData{{"vol_status", newVolStatus}, {"ep_status", newEpStatus}};
    const QPointer guard(this);
    const bool success = bangumiAPI->updateCollection(subjectId, apiRequestData, 3);
    DatabaseManager::updateCollectionFields(subjectId, {{"vol_status", newVolStatus}, {"ep_status", newEpStatus}}, true);
    if (!guard) return;
    if (!success) {
        ui.pushButton_15->setText("失败");
        ui.pushButton_15->setEnabled(true);
        return;
    }
    emit collectionDataChanged();
    ui.pushButton_15->setText("更新");
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
    if (volContainer) {
        delete volContainer;
        volContainer = nullptr;
        volEdit = nullptr;
        epEdit = nullptr;
    }
    emit overlayClosed();
}