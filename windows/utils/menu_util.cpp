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
    const QColor color2 = getConfig("Theme/color2", 0xf2ecf4).toString();
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
        const QColor color3 = getConfig("Theme/color3", 0xe1dbe4).toString();
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
    auto *selector = new StatusSelector(parentButton, subjectType, collectionType, subjectId, bangumiAPI, dbManager, std::move(callback));
    selector->setAttribute(Qt::WA_DeleteOnClose);
    selector->show();
}

void StatusSelector::updateStatus(const int statusValue)
{   // 更新状态
    hide();
    if (statusValue == 0) deleteCollect();
    else performUpdate(statusValue);
}

void StatusSelector::deleteCollect()
{   // 删除收藏
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
}

void StatusSelector::performUpdate(int statusValue) const
{   // 更新/创建收藏
    QPointer guard(this);
    const QJsonObject collectionData{{"type", statusValue}};
    if (collectionType >= 1 && collectionType <= 5) {
        bangumiAPI->updateCollection(subjectId, collectionData, 3, [this, guard, statusValue](const bool success, const QString &error) {
            if (!guard) return;
            if (!success) {
                qDebug() << subjectId << "状态更新失败:" << error;
                const_cast<StatusSelector*>(this)->deleteLater();
                return;
            }
            DatabaseManager::updateCollectionFields(subjectId, {{"type", statusValue}}, true);
            onCollectionUpdateSuccess(statusValue);
            }
        );
    } else {
        bangumiAPI->createOrUpdateCollection(subjectId, collectionData, 3, [this, guard, statusValue](const bool success, const QString &error) {
            if (!guard) return;
            if (!success) {
                qDebug() << subjectId << "状态更新失败:" << error;
                const_cast<StatusSelector*>(this)->deleteLater();
                return;
            }
            bangumiAPI->getUserCollection(subjectId, 3, [this, guard, statusValue](const QJsonObject &collection, const QString &err) {
                if (!guard) return;
                if (!err.isEmpty()) {
                    qDebug() << "获取收藏详情失败:" << err;
                    const_cast<StatusSelector*>(this)->deleteLater();
                    return;
                }
                DatabaseManager::insertManyCollectionData(QJsonArray{collection});
                onCollectionUpdateSuccess(statusValue);
            });
        });
    }
}

void StatusSelector::onCollectionUpdateSuccess(const int statusValue) const
{   // 收藏成功处理
    if (callback) callback(statusValue);
    if (statusValue == 2) updateEpisodesIfNeeded(statusValue);
    else const_cast<StatusSelector*>(this)->deleteLater();
}

void StatusSelector::updateEpisodesIfNeeded(int statusValue) const
{   // 根据需要标注全部已看(to 看过)
    QPointer guard(this);
    const auto episodes = DatabaseManager::getEpisodesBySubjectId(subjectId);
    if (episodes.isEmpty()) {
        bangumiAPI->getSubjectEpisodes(subjectId, 3, [this, guard, statusValue](const QJsonArray &apiEpisodes, const QString &error) {
            if (!guard) return;
            if (!error.isEmpty()) {
                qDebug() << "获取章节失败:" << error;
                const_cast<StatusSelector*>(this)->deleteLater();
                return;
            }
            DatabaseManager::insertManyEpisodes(subjectId, apiEpisodes);
            const auto newEpisodes = DatabaseManager::getEpisodesBySubjectId(subjectId);
            updateEpisodesBatch(newEpisodes, statusValue);
        });
    } else updateEpisodesBatch(episodes, statusValue);
}

void StatusSelector::updateEpisodesBatch(const QVector<EpisodeData> &episodes, int statusValue) const
{   // 全部已看
    QPointer guard(this);
    QJsonArray episodeIds;
    for (const auto &ep : episodes) episodeIds.append(ep.episodeId);
    if (episodeIds.isEmpty()) {
        const_cast<StatusSelector*>(this)->deleteLater();
        return;
    }
    const QJsonObject apiRequestData{{"episode_id", episodeIds}, {"type", statusValue}};
    bangumiAPI->updateSubjectEpisodes(subjectId, apiRequestData, 3, [this, guard, episodes](const bool success, const QString &error) {
        if (!guard) return;
        if (success) {
            DatabaseManager::updateAllEpisodesStatus(subjectId, 2);
            DatabaseManager::updateCollectionFields(subjectId, {{"ep_status", episodes.size()}}, true);
        } else qDebug() << "更新章节状态失败:" << error;
        const_cast<StatusSelector*>(this)->deleteLater();
    });
}