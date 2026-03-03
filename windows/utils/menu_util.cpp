#include "menu_util.h"
#include <QLabel>
#include <QDialog>
#include <QJsonArray>
#include <QVBoxLayout>
#include <QPushButton>
#include <QApplication>
#include <QDialogButtonBox>
#include <QDesktopServices>
#include "../sql.h"
#include "../config.h"
#include "../api/bangumi_api.h"

static const QMap<int, QMap<int, QString>> statusNamesMap = {
    {1, {{0, "取消收藏"}, {1, "想读"}, {2, "读过"}, {3, "在读"}, {4, "搁置"}, {5, "抛弃"}}},
    {2, {{0, "取消追番"}, {1, "想看"}, {2, "看过"}, {3, "在看"}, {4, "搁置"}, {5, "抛弃"}}},
    {4, {{0, "取消收藏"}, {1, "想玩"}, {2, "玩过"}, {3, "在玩"}, {4, "搁置"}, {5, "抛弃"}}},
    {7, {{0, "取消收藏"}, {1, "想读"}, {2, "读过"}, {3, "在读"}, {4, "搁置"}, {5, "抛弃"}}},
    {8, {{0, "取消收藏"}, {1, "想读"}, {2, "读过"}, {3, "在读"}, {4, "搁置"}, {5, "抛弃"}}}};

StatusSelector::StatusSelector(const QPushButton *parentButton, int subjectType, int collectionType, int subjectId, BangumiAPI *bangumiAPI, DatabaseManager *dbManager, std::function<void(int)> callback, int xOffset): QWidget(nullptr), subjectId(subjectId), collectionType(collectionType), bangumiAPI(bangumiAPI), dbManager(dbManager), callback(std::move(callback))
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
        if (statusValue == 0 && collectionType == 0) continue;
        auto *btn = new QPushButton(statusMap.value(statusValue));
        connect(btn, &QPushButton::clicked, this, [this, statusValue] {updateStatus(statusValue);});
        layout->addWidget(btn);
    }
    move(parentButton->mapToGlobal(QPoint(xOffset, parentButton->height())));
}

void StatusSelector::showStatusSelector(const QPushButton *parentButton, int subjectType, int collectionType, int subjectId, BangumiAPI *bangumiAPI, DatabaseManager *dbManager, std::function<void(int)> callback, int xOffset)
{   // 显示下拉菜单
    auto selector = new StatusSelector(parentButton, subjectType, collectionType, subjectId, bangumiAPI, dbManager, std::move(callback), xOffset);
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
        else{
            for (QWidget *widget : qApp->topLevelWidgets()) {
                if (!widget->inherits("QDialog")) {
                    widget->raise();
                    widget->activateWindow();
                    break;
                }
            }
        }
        deleteLater();
        return;
    }
    QJsonObject collectionData{{"type", statusValue}};
    bool success = false;
    if (collectionType >= 1 && collectionType <= 5) success = bangumiAPI->updateCollection(subjectId, collectionData) && (DatabaseManager::updateCollectionFields(subjectId, {{"type", statusValue}}, true), true);
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