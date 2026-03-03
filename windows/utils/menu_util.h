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
    explicit StatusSelector(const QPushButton *parentButton = nullptr, int subjectType = 2, int collectionType = 0, int subjectId = -1, BangumiAPI *bangumiAPI = nullptr, DatabaseManager *dbManager = nullptr, std::function<void(int)> callback = nullptr, int xOffset = -40);
    static void showStatusSelector(const QPushButton *parentButton, int subjectType = 2, int collectionType = 0, int subjectId = -1, BangumiAPI *bangumiAPI = nullptr, DatabaseManager *dbManager = nullptr, std::function<void(int)> callback = nullptr, int xOffset = -40);

private:
    void updateStatus(int statusValue);
    int subjectId;
    int collectionType;
    QVBoxLayout *layout;
    BangumiAPI *bangumiAPI;
    DatabaseManager *dbManager;
    std::function<void(int)> callback;
};

#endif // MENU_UTIL_H