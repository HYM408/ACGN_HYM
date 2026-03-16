#include "menu_util.h"
#include <QLabel>
#include <QDialog>
#include <QJsonArray>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QDesktopServices>
#include <QGraphicsDropShadowEffect>
#include "../config.h"
#include "../sql/sql.h"
#include "../api/bangumi_api.h"

static const QMap<int, QMap<int, QString>> statusNamesMap = {
    {1, {{0, "取消收藏"}, {1, "想读"}, {2, "读过"}, {3, "在读"}, {4, "搁置"}, {5, "抛弃"}}},
    {2, {{0, "取消追番"}, {1, "想看"}, {2, "看过"}, {3, "在看"}, {4, "搁置"}, {5, "抛弃"}}},
    {4, {{0, "取消收藏"}, {1, "想玩"}, {2, "玩过"}, {3, "在玩"}, {4, "搁置"}, {5, "抛弃"}}},
    {7, {{0, "取消收藏"}, {1, "想读"}, {2, "读过"}, {3, "在读"}, {4, "搁置"}, {5, "抛弃"}}},
    {8, {{0, "取消收藏"}, {1, "想读"}, {2, "读过"}, {3, "在读"}, {4, "搁置"}, {5, "抛弃"}}}};

StatusSelector::StatusSelector(const QPushButton *parentButton, const int subjectType, const int collectionType, const int subjectId, BangumiAPI *bangumiAPI, DatabaseManager *dbManager, std::function<void(int)> callback) : QWidget(nullptr), subjectId(subjectId), collectionType(collectionType), bangumiAPI(bangumiAPI), dbManager(dbManager), callback(std::move(callback)), mainWindow(parentButton->window())
{   // 下拉菜单
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedWidth(160);
    auto *container = new QFrame(this);
    const QColor color2 = getColor("color2", QColor("#f2ecf4"));
    container->setStyleSheet(QString("QFrame {background-color: %1; border: 1px solid #ccc; border-radius: 4px}").arg(color2.name()));
    auto *shadowEffect = new QGraphicsDropShadowEffect(container);
    shadowEffect->setBlurRadius(15);
    shadowEffect->setColor(QColor(0, 0, 0, 80));
    shadowEffect->setOffset(0, 2);
    container->setGraphicsEffect(shadowEffect);
    auto *containerLayout = new QVBoxLayout(container);
    containerLayout->setSpacing(0);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    const auto statusMap = statusNamesMap.value(subjectType, statusNamesMap[2]);
    for (auto statusValue : {1, 3, 2, 4, 5, 0}) {
        if (statusValue == 0 && collectionType == 0) continue;
        auto *btn = new QPushButton(statusMap.value(statusValue));
        const QColor color3 = getColor("color3", QColor("#e1dbe4"));
        btn->setStyleSheet(QString("QPushButton {border: none; padding: 10px 12px; font-size: 13px}"
                                   "QPushButton:hover {background-color: %1}").arg(color3.name()));
        connect(btn, &QPushButton::clicked, this, [this, statusValue] {updateStatus(statusValue);});
        containerLayout->addWidget(btn);
    }
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->addWidget(container);
    move(parentButton->mapToGlobal(QPoint(parentButton->width() / 2, parentButton->height())) - QPoint(width() / 2, 15));
}

void StatusSelector::showStatusSelector(const QPushButton *parentButton, const int subjectType, const int collectionType, const int subjectId, BangumiAPI *bangumiAPI, DatabaseManager *dbManager, std::function<void(int)> callback)
{   // 显示下拉菜单
    const auto selector = new StatusSelector(parentButton, subjectType, collectionType, subjectId, bangumiAPI, dbManager, std::move(callback));
    selector->setAttribute(Qt::WA_DeleteOnClose);
    selector->show();
}

void StatusSelector::updateStatus(int statusValue)
{   // 状态更新
    hide();
    if (statusValue == 0) {
        QDesktopServices::openUrl(QString("%1subject/%2").arg(getConfig("Bangumi/bangumi_base_url").toString()).arg(subjectId));
        QDialog confirmDialog;
        confirmDialog.setWindowFlags(confirmDialog.windowFlags() | Qt::WindowStaysOnTopHint);
        confirmDialog.setWindowTitle("删除收藏");
        confirmDialog.setFixedSize(300, 100);
        QVBoxLayout dialogLayout(&confirmDialog);
        QLabel label("请从浏览器中执行删除收藏操作");
        dialogLayout.addWidget(&label);
        QDialogButtonBox buttonBox;
        buttonBox.addButton("确认删除", QDialogButtonBox::AcceptRole);
        buttonBox.addButton("取消", QDialogButtonBox::RejectRole);
        dialogLayout.addWidget(&buttonBox);
        connect(&buttonBox, &QDialogButtonBox::accepted, &confirmDialog, &QDialog::accept);
        connect(&buttonBox, &QDialogButtonBox::rejected, &confirmDialog, &QDialog::reject);
        if (confirmDialog.exec() == QDialog::Accepted) dbManager->deleteCollectionBySubjectId(subjectId);
        else {
            mainWindow->showNormal();
            mainWindow->raise();
            mainWindow->activateWindow();
        }
        deleteLater();
        return;
    }
    const QJsonObject collectionData{{"type", statusValue}};
    bool success = false;
    if (collectionType >= 1 && collectionType <= 5) success = bangumiAPI->updateCollection(subjectId, collectionData, 3) && (DatabaseManager::updateCollectionFields(subjectId, {{"type", statusValue}}, true), true);
    else success = bangumiAPI->createOrUpdateCollection(subjectId, collectionData, 3) && (DatabaseManager::insertManyCollectionData(QJsonArray{bangumiAPI->getUserCollection(subjectId, 3)}), true);
    qDebug() << subjectId << "状态更新" << (success ? "成功" : "失败");
    if (success) {
        callback(statusValue);
        if (statusValue == 2) {
            QVector<EpisodeData> episodes = DatabaseManager::getEpisodesBySubjectId(subjectId);
            QJsonArray apiEpisodes;
            if (episodes.isEmpty()) apiEpisodes = bangumiAPI->getSubjectEpisodes(subjectId, 3);
            if (!apiEpisodes.isEmpty()) {
                DatabaseManager::insertManyEpisodes(subjectId, apiEpisodes);
                episodes = DatabaseManager::getEpisodesBySubjectId(subjectId);
            }
            QJsonArray episodeIds;
            for (const auto &ep : episodes) episodeIds.append(ep.episode_id);
            if (!episodeIds.isEmpty()) {
                const QJsonObject apiRequestData{{"episode_id", episodeIds}, {"type", 2}};
                if (bangumiAPI->updateSubjectEpisodes(subjectId, apiRequestData, 3)) DatabaseManager::updateAllEpisodesStatus(subjectId, 2);
            }
        }
    }
    deleteLater();
}