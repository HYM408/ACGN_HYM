#ifndef MENU_UTIL_H
#define MENU_UTIL_H

#include <QWidget>
#include <QPointer>
#include "../sql/data_structs.h"

class BangumiAPI;
class QVBoxLayout;
class QPushButton;
class DatabaseManager;

class StatusSelector : public QWidget
{
    Q_OBJECT

public:
    explicit StatusSelector(const QPushButton *parentButton, int subjectType, int collectionType, int subjectId, BangumiAPI *bangumiAPI, DatabaseManager *dbManager, std::function<void(int)> callback);
    static void showStatusSelector(const QPushButton *parentButton, int subjectType, int collectionType, int subjectId, BangumiAPI *bangumiAPI, DatabaseManager *dbManager, std::function<void(int)> callback);
    void deleteCollect();
    void updateEpisodesBatch(const QVector<EpisodeData>& episodes, int statusValue) const;
    void updateEpisodesIfNeeded(int statusValue) const;
    void onCollectionUpdateSuccess(int statusValue) const;
    void performUpdate(int statusValue) const;

private:
    void updateStatus(int statusValue);
    int subjectId = 0;
    int collectionType = 0;
    QVBoxLayout *layout = nullptr;
    BangumiAPI *bangumiAPI = nullptr;
    DatabaseManager *dbManager = nullptr;
    std::function<void(int)> callback = nullptr;
    QPointer<QWidget> mainWindow;
};

#endif // MENU_UTIL_H