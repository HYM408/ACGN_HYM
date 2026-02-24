#ifndef BANGUMI_API_H
#define BANGUMI_API_H

#include <QNetworkReply>

class BangumiAPI : public QObject {
    Q_OBJECT

public:
    explicit BangumiAPI(QObject *parent = nullptr);
    QJsonArray getUserCollections(bool getAll = true, int maxRetries = 3);
    QJsonObject getUserCollection(int subjectId, int maxRetries = 3);
    QJsonObject getSubjectInfo(int subjectId, int maxRetries = 3);
    QJsonArray searchSubjects(const QString &keyword = "", const QString &tag = "", int subjectType = 2, int maxRetries = 3);
    QJsonArray getSubjectEpisodes(int subjectId, int maxRetries = 3);
    bool createOrUpdateCollection(int subjectId, const QJsonObject &collectionData, int maxRetries = 3);
    bool updateCollection(int subjectId, const QJsonObject &collectionData, int maxRetries = 3);
    bool updateSubjectEpisodes(int subjectId, const QJsonObject &episodesData, int maxRetries = 3);

private:
    void refreshAndReload();
    [[nodiscard]] QNetworkRequest createRequest(const QString &url) const;
    QJsonObject sendRequest(QNetworkAccessManager &manager, QNetworkRequest &request, const QString &method = "GET", const QByteArray &data = QByteArray(), int maxRetries = 3, int *statusCode = nullptr);
    QString userId;
    QString accessToken;
    QString refreshToken;
    QString baseUrl = "https://api.bgm.tv";
};

#endif // BANGUMI_API_H