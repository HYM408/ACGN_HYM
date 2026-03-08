#ifndef MENU_UTIL_H
#define MENU_UTIL_H

#include <QWidget>

class BangumiAPI;
class QVBoxLayout;
class QPushButton;
class DatabaseManager;

class StatusSelector : public QWidget
{
    Q_OBJECT
public:
    explicit StatusSelector(const QPushButton *parentButton, int subjectType, int collectionType, int subjectId, BangumiAPI *bangumiAPI, DatabaseManager *dbManager, std::function<void(int)> callback, int xOffset);
    static void showStatusSelector(const QPushButton *parentButton, int subjectType, int collectionType, int subjectId, BangumiAPI *bangumiAPI, DatabaseManager *dbManager, std::function<void(int)> callback, int xOffset);

private:
    void updateStatus(int statusValue);
    int subjectId = 0;
    int collectionType = 0;
    QVBoxLayout *layout = nullptr;
    BangumiAPI *bangumiAPI = nullptr;
    DatabaseManager *dbManager = nullptr;
    std::function<void(int)> callback = nullptr;
};

#endif // MENU_UTIL_H