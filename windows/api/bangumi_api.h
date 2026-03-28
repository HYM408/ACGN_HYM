#ifndef BANGUMI_API_H
#define BANGUMI_API_H

#include <QNetworkReply>

class BangumiAPI : public QObject {
    Q_OBJECT

public:
    explicit BangumiAPI(QObject *parent = nullptr);
    using CollectionsCallback = std::function<void(const QJsonArray&, const QString &error)>;
    using CollectionCallback = std::function<void(const QJsonObject&, const QString &error)>;
    using SubjectCallback = std::function<void(const QJsonObject&, const QString &error)>;
    using EpisodesCallback = std::function<void(const QJsonArray&, const QString &error)>;
    using BoolCallback = std::function<void(bool success, const QString &error)>;
    using ProgressCallback = std::function<void(int current, int total)>;
    void getUserCollections(bool getAll, int maxRetries, const ProgressCallback &progressCallback, const CollectionsCallback &completionCallback);
    void getUserCollection(int subjectId, int maxRetries, const CollectionCallback &callback);
    void getSubjectInfo(int subjectId, int maxRetries, const SubjectCallback &callback);
    void searchSubjectsWithPost(const QString &keyword, const QString &tag, int subjectType, bool containsNsfw, int maxRetries, const CollectionsCallback &callback);
    void getSubjectEpisodes(int subjectId, int maxRetries, const EpisodesCallback& callback);
    void createOrUpdateCollection(int subjectId, const QJsonObject &collectionData, int maxRetries, const BoolCallback &callback);
    void updateCollection(int subjectId, const QJsonObject &collectionData, int maxRetries, const BoolCallback &callback);
    void updateSubjectEpisodes(int subjectId, const QJsonObject &episodesData, int maxRetries, const BoolCallback &callback);

private:
    void refreshAndReload();
    [[nodiscard]] QNetworkRequest createRequest(const QString &url) const;
    void fetchCollectionsPage(int offset, QJsonArray allItems, bool getAll, int maxRetries, const ProgressCallback &progress, const CollectionsCallback &completion);
    QNetworkAccessManager manager;
    QString userId;
    QString accessToken;
    QString refreshToken;
};

#endif // BANGUMI_API_H