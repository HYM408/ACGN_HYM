#include "menu_util.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QPushButton>
#include "../sql.h"
#include "../api/bangumi_api.h"

static const QMap<int, QMap<int, QString>> statusNamesMap = {
    {1, {{0, "取消收藏"}, {1, "想读"}, {2, "读过"}, {3, "在读"}, {4, "搁置"}, {5, "抛弃"}}},
    {2, {{0, "取消追番"}, {1, "想看"}, {2, "看过"}, {3, "在看"}, {4, "搁置"}, {5, "抛弃"}}},
    {4, {{0, "取消收藏"}, {1, "想玩"}, {2, "玩过"}, {3, "在玩"}, {4, "搁置"}, {5, "抛弃"}}},
    {7, {{0, "取消收藏"}, {1, "想读"}, {2, "读过"}, {3, "在读"}, {4, "搁置"}, {5, "抛弃"}}},
    {8, {{0, "取消收藏"}, {1, "想读"}, {2, "读过"}, {3, "在读"}, {4, "搁置"}, {5, "抛弃"}}}};

StatusSelector::StatusSelector(const QPushButton *parentButton, int subjectType, int collectionType, int subjectId, BangumiAPI* bangumiAPI, std::function<void(int)> callback, int xOffset): QWidget(nullptr), subjectId(subjectId), collectionType(collectionType), bangumiAPI(bangumiAPI), callback(std::move(callback))
{   // 下拉菜单
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setFixedWidth(120);
    layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    setStyleSheet("QPushButton {background-color: white; border: none; padding: 10px 12px; font-size: 13px; color: #333}"
                  "QPushButton:hover {background-color: #f5f5f5}");
    auto statusMap = statusNamesMap.value(subjectType, statusNamesMap[2]);
    for (auto statusValue : {1, 3, 2, 4, 5, 0}) {
        auto *btn = new QPushButton(statusMap.value(statusValue));
        connect(btn, &QPushButton::clicked, this, [this, statusValue] {updateStatus(statusValue);});
        layout->addWidget(btn);
    }
    move(parentButton->mapToGlobal(QPoint(xOffset, parentButton->height())));
}

void StatusSelector::showStatusSelector(const QPushButton *parentButton, int subjectType, int collectionType, int subjectId, BangumiAPI* bangumiAPI, std::function<void(int)> callback, int xOffset)
{   // 显示下拉菜单
    auto selector = new StatusSelector(parentButton, subjectType, collectionType, subjectId, bangumiAPI, std::move(callback), xOffset);
    selector->setAttribute(Qt::WA_DeleteOnClose);
    selector->show();
}

void StatusSelector::updateStatus(int statusValue)
{   // 状态更新
    hide();
    if (statusValue == 0) return deleteLater();
    QJsonObject collectionData{{"type", statusValue}};
    bool success = false;
    if (collectionType >= 1 && collectionType <= 5) success = bangumiAPI->updateCollection(subjectId, collectionData) && (DatabaseManager::updateCollectionField(subjectId, "type", statusValue), true);
    else  success = bangumiAPI->createOrUpdateCollection(subjectId, collectionData) && (DatabaseManager::insertManyCollectionData(QJsonArray{bangumiAPI->getUserCollection(subjectId)}), true);
    qDebug() << subjectId << "状态更新" << (success ? "成功" : "失败");
    if (success) {
        callback(statusValue);
        if (statusValue == 2) {
            QJsonArray episodes = DatabaseManager::getEpisodesBySubjectId(subjectId);
            QJsonArray apiEpisodes;
            if (episodes.isEmpty()) apiEpisodes = bangumiAPI->getSubjectEpisodes(subjectId);
            if (!apiEpisodes.isEmpty()) {
                DatabaseManager::insertManyEpisodes(subjectId, apiEpisodes);
                episodes = DatabaseManager::getEpisodesBySubjectId(subjectId);
            }
            QJsonArray episodeIds;
            for (const QJsonValueRef &value : episodes) episodeIds.append(value.toObject().value("id").toInt());
            if (!episodeIds.isEmpty()) {
                QJsonObject apiRequestData{{"episode_id", episodeIds}, {"type", 2}};
                if (bangumiAPI->updateSubjectEpisodes(subjectId, apiRequestData)) DatabaseManager::updateAllEpisodesStatus(subjectId, 2);
            }
        }
    }
    deleteLater();
}