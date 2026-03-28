#include "episode_overlay.h"
#include "episode_delegate.h"
#include <QPainter>
#include <QPointer>
#include <QLineEdit>
#include <QListView>
#include <QJsonArray>
#include <QResizeEvent>
#include <QStandardItemModel>
#include "../config.h"
#include "../sql/sql.h"
#include "../api/bangumi_api.h"
#include "../utils/context_menu_util.h"

static constexpr int BUTTON_SIZE = 40;
static constexpr int SPACING = 7;
static constexpr int COLUMNS = 12;
static constexpr int MAX_CONTAINER_HEIGHT = 900;
static constexpr int MIN_CONTAINER_HEIGHT = 200;

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
    episodeListView->setUniformItemSizes(true);
    episodeListView->setGridSize(QSize(BUTTON_SIZE + SPACING, BUTTON_SIZE + SPACING));
    episodeListView->setFlow(QListView::LeftToRight);
    episodeListView->setWrapping(true);
    episodeListView->setStyleSheet("QListView {border: none}");
    ui.gridLayoutEpisodes->addWidget(episodeListView, 0, 0, 1, 1);
    connect(ui.btnBack, &QPushButton::clicked, this, &EpisodeOverlay::closeOverlay);
    connect(ui.btnUpdate, &QPushButton::clicked, this, &EpisodeOverlay::onUpdateButtonClicked);
    connect(episodeDelegate, &EpisodeDelegate::episodeClicked, this, &EpisodeOverlay::onEpisodeItemClicked);
    applyTheme();
}

void EpisodeOverlay::setManagers(BangumiAPI *api, DatabaseManager *db)
{
    bangumiAPI = api;
    dbManager = db;
}

void EpisodeOverlay::applyTheme() const
{   // 主题
    const QColor color1 = getColor("color1", 0xfdf7ff);
    ui.mainFrame->setStyleSheet(QString("QFrame {background-color: %1}").arg(color1.name()));
}

void EpisodeOverlay::keyPressEvent(QKeyEvent *event)
{   // 键盘事件
    if (event->key() == Qt::Key_Escape) closeOverlay();
}

bool EpisodeOverlay::eventFilter(QObject *obj, QEvent *event)
{   // 事件过滤器
    if (event->type() == QEvent::MouseButtonPress) {
        if (auto *edit = qobject_cast<QLineEdit*>(obj)) {
            QMetaObject::invokeMethod(edit, &QLineEdit::selectAll, Qt::QueuedConnection);
            return false;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void EpisodeOverlay::onUpdateButtonClicked()
{   // 更新按钮点击
    if (subjectsData.subjectType == 2) onMarkAllWatchedClicked();
    else onUpdateClicked();
}

void EpisodeOverlay::showWithSubjectsData(const SubjectsData &sData)
{   // 显示叠加层
    show();
    setFocus();
    ui.labelSubjectName->setText(sData.nameCn.isEmpty() ? sData.name : sData.nameCn);
    this->subjectsData = sData;
    ui.btnUpdate->setEnabled(true);
    if (sData.subjectType != 2) {
        ui.btnUpdate->setText("更新");
        loadVolEpData();
    } else {
        ui.btnUpdate->setText("全部已看");
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
    const int subjectId = subjectsData.subjectId;
    episodes = DatabaseManager::getEpisodesBySubjectId(subjectId);
    if (!episodes.isEmpty()) {
        updateEpisodeView();
        return;
    }
    episodes = dbManager->getEpisodeData(subjectId);
    updateEpisodeView();
    QPointer guard(this);
    bangumiAPI->getSubjectEpisodes(subjectId, 3, [this, subjectId, guard](const QJsonArray &apiEpisodes, const QString &error) {
        if (!guard) return;
        if (!error.isEmpty() || apiEpisodes.isEmpty()) {
            episodeListView->hide();
            noEpisodesLabel = new QLabel("暂无剧集信息", episodeContainer);
            noEpisodesLabel->setAlignment(Qt::AlignCenter);
            noEpisodesLabel->setStyleSheet("color: #666");
            ui.gridLayoutEpisodes->addWidget(noEpisodesLabel, 0, 0);
            episodeContainer->setFixedHeight(MIN_CONTAINER_HEIGHT);
            resizeEvent(nullptr);
            return;
        }
        if (noEpisodesLabel) {
            delete noEpisodesLabel;
            noEpisodesLabel = nullptr;
        }
        episodeListView->show();
        DatabaseManager::insertManyEpisodes(subjectId, apiEpisodes);
        episodes = DatabaseManager::getEpisodesBySubjectId(subjectId);
        updateEpisodeView();
    });
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
    createField("Vol.", subjectsData.volStatus, subjectsData.subjectVolumes, volEdit);
    createField("Chap.", subjectsData.epStatus, subjectsData.subjectEps, epEdit);
    setupLineEditCustomContextMenu(volEdit, CMO_Cut | CMO_Copy | CMO_Paste);
    setupLineEditCustomContextMenu(epEdit, CMO_Cut | CMO_Copy | CMO_Paste);
    volEdit->installEventFilter(this);
    epEdit->installEventFilter(this);
    auto *hBox = new QHBoxLayout;
    hBox->addStretch();
    hBox->addWidget(container);
    hBox->addStretch();
    ui.gridLayoutEpisodes->addLayout(hBox, 0, 0);
    const int containerHeight = qMax(MIN_CONTAINER_HEIGHT, qMin(120 + vLayout->sizeHint().height() + 20, MAX_CONTAINER_HEIGHT));
    episodeContainer->setFixedHeight(containerHeight);
    resizeEvent(nullptr);
}

void EpisodeOverlay::updateEpisodeView()
{   // 更新ui
    episodeModel->clear();
    for (const auto &ep : episodes) {
        auto *item = new QStandardItem();
        item->setData(QVariant::fromValue(ep), EpisodeDataRole);
        QString name = ep.nameCn.isEmpty() ? ep.name : ep.nameCn;
        item->setToolTip(name);
        episodeModel->appendRow(item);
    }
    const int rows = (episodeModel->rowCount() + COLUMNS - 1) / COLUMNS;
    const int visibleRows = qMin(rows, 15);
    int containerHeight = 120 + visibleRows * (BUTTON_SIZE + SPACING) + 20;
    containerHeight = qMax(MIN_CONTAINER_HEIGHT, qMin(containerHeight, MAX_CONTAINER_HEIGHT));
    episodeContainer->setFixedHeight(containerHeight);
    ui.scrollAreaEpisodes->setVerticalScrollBarPolicy(rows > visibleRows ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
    resizeEvent(nullptr);
}

void EpisodeOverlay::onEpisodeItemClicked(const EpisodeData &episodeData)
{   // 点击剧集
    const int epType = episodeData.episodeType;
    if (epType != 0 && epType != 1) return;
    closeOverlay();
    emit episodeClicked(subjectsData, episodeData);
}

void EpisodeOverlay::onMarkAllWatchedClicked()
{   // 标记全部已看
    if (episodes.isEmpty()) return;
    ui.btnUpdate->setText("标记中...");
    ui.btnUpdate->setEnabled(false);
    const int subjectId = subjectsData.subjectId;
    QJsonArray episodeIds;
    for (const auto &ep : episodes) episodeIds.append(ep.episodeId);
    const QJsonObject apiRequestData{{"episode_id", episodeIds}, {"type", 2}};
    QPointer guard(this);
    bangumiAPI->updateSubjectEpisodes(subjectId, apiRequestData, 3, [this, subjectId, guard](const bool success, const QString &error) {
        DatabaseManager::updateAllEpisodesStatus(subjectId, 2);
        DatabaseManager::updateCollectionFields(subjectId, {{"ep_status", episodes.size()}}, true);
        if (!guard) return;
        if (!success || !error.isEmpty()) {
            ui.btnUpdate->setText("失败");
            ui.btnUpdate->setEnabled(true);
            return;
        }
        emit collectionDataChanged();
        loadEpisodesData();
        ui.btnUpdate->setText("全部已看");
        ui.btnUpdate->setEnabled(true);
    });
}

void EpisodeOverlay::onUpdateClicked()
{   // 更新进度
    ui.btnUpdate->setText("更新中...");
    ui.btnUpdate->setEnabled(false);
    const int subjectId = subjectsData.subjectId;
    int newEpStatus  = epEdit->text().toInt();
    int newVolStatus = volEdit->text().toInt();
    const QJsonObject apiRequestData{{"vol_status", newVolStatus}, {"ep_status", newEpStatus}};
    QPointer guard(this);
    bangumiAPI->updateCollection(subjectId, apiRequestData, 3, [this, subjectId, newVolStatus, newEpStatus, guard](const bool success, const QString &error) {
        DatabaseManager::updateCollectionFields(subjectId, {{"vol_status", newVolStatus}, {"ep_status", newEpStatus}}, true);
        if (!guard) return;
        if (!success || !error.isEmpty()) {
            ui.btnUpdate->setText("失败");
            ui.btnUpdate->setEnabled(true);
            return;
        }
        emit collectionDataChanged();
        ui.btnUpdate->setText("更新");
        ui.btnUpdate->setEnabled(true);
    });
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