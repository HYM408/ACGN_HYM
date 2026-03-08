#ifndef BANGUMI_API_H
#define BANGUMI_API_H

#include <QNetworkReply>

class BangumiAPI : public QObject {
    Q_OBJECT

public:
    explicit BangumiAPI(QObject *parent = nullptr);
    QJsonArray getUserCollections(bool getAll, int maxRetries, const std::function<void(int offset, int total)>& progressCallback);
    QJsonObject getUserCollection(int subjectId, int maxRetries);
    QJsonObject getSubjectInfo(int subjectId, int maxRetries);
    QJsonArray searchSubjects(const QString &keyword, const QString &tag, int subjectType, int maxRetries);
    QJsonArray getSubjectEpisodes(int subjectId, int maxRetries);
    bool createOrUpdateCollection(int subjectId, const QJsonObject &collectionData, int maxRetries);
    bool updateCollection(int subjectId, const QJsonObject &collectionData, int maxRetries);
    bool updateSubjectEpisodes(int subjectId, const QJsonObject &episodesData, int maxRetries);

private:
    void refreshAndReload();
    [[nodiscard]] QNetworkRequest createRequest(const QString &url) const;
    QJsonObject sendRequest(QNetworkAccessManager &manager, QNetworkRequest &request, const QString &method, const QByteArray &data, int maxRetries, int *statusCode);
    QString userId;
    QString accessToken;
    QString refreshToken;
    QString baseUrl = "https://api.bgm.tv";
};

#endif // BANGUMI_API_H