#ifndef BANGUMI_API_H
#define BANGUMI_API_H

#include <QNetworkAccessManager>

class BangumiOAuth;

class BangumiAPI : public QObject
{
    Q_OBJECT

public:
    explicit BangumiAPI(BangumiOAuth *oauth, QObject *parent = nullptr);
    using BoolCallback = std::function<void(bool success, const QString &error)>;
    using CollectionsCallback = std::function<void(const QJsonArray&, const QString &error)>;
    using SubjectCallback = std::function<void(const QJsonObject&, const QString &error)>;
    void requestWithAuth(const QString &url, const QString &method, const QByteArray &data, int maxRetries, const std::function<void(const QByteArray&, int, const QString&)> &callback);
    void refreshAndReload(const std::function<void(bool)> &callback);
    void getUser(int maxRetries, const BoolCallback &callback);
    void getUserCollections(int offset, QJsonArray allItems, bool getAll, int maxRetries, const std::function<void(int current, int total)> &progress, const CollectionsCallback &completion);
    void getUserCollection(int subjectId, int maxRetries, const SubjectCallback &callback);
    void getSubjectInfo(int subjectId, int maxRetries, const SubjectCallback &callback);
    void searchSubjectsWithPost(const QString &keyword, const QString &tag, int subjectType, bool containsNsfw, int maxRetries, const CollectionsCallback &callback);
    void getSubjectEpisodes(int subjectId, int maxRetries, const CollectionsCallback &callback);
    void createOrUpdateCollection(int subjectId, const QJsonObject &collectionData, int maxRetries, const BoolCallback &callback);
    void updateCollection(int subjectId, const QJsonObject &collectionData, int maxRetries, const BoolCallback &callback);
    void updateSubjectEpisodes(int subjectId, const QJsonObject &episodesData, int maxRetries, const BoolCallback &callback);

private:
    BangumiOAuth *bangumiOAuth = nullptr;
    bool m_isRefreshing = false;
    std::vector<std::function<void(bool)>> m_pendingCallbacks;
    QNetworkAccessManager m_manager;
    QString m_userId;
    QString m_accessToken;
    QString m_refreshToken;
};

#endif // BANGUMI_API_H