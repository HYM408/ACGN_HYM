#ifndef SQL_H
#define SQL_H

#include <QSqlQuery>
#include "data_structs.h"

class DatabaseManager: public QObject
{
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager() override;
    void openDatabase();
    static void initTables();
    // collection表
    static bool insertManyCollectionData(const QJsonArray &jsonArray);
    static CollectionData collectionFromQuery(const QSqlQuery& query);
    static QVector<CollectionData> getCollectionBySubjectTypeAndType(int subjectType, int typeValue);
    static QJsonObject getStatusCountsBySubjectType(int subjectType);
    static CollectionData getCollectionBySubjectId(int subjectId);
    static bool updateCollectionFields(int subjectId, const QJsonObject &fields, bool updateTimestamp);
    bool deleteCollectionBySubjectId(int subjectId);
    void clearCollectionTable() const;
    // episode表
    static bool insertManyEpisodes(int subjectId, const QJsonArray &episodesArray);
    static QVector<EpisodeData> getEpisodesBySubjectId(int subjectId);
    static bool deleteEpisodesBySubjectId(int subjectId);
    static bool updateAllEpisodesStatus(int subjectId, int collectionType = 2);
    // subjects表
    static bool insertOrUpdateSubject(const QJsonObject &apiData);
    static SubjectsData getSubjectById(int subjectId);
    // episode公共数据表
    bool insertEpisodeAirdateFromFile(const QString& filePath);
    [[nodiscard]] QJsonObject getEpisodeAirdates(const QList<int> &subjectIds) const;

signals:
    void collectionDeleted();

private:
    QSqlDatabase database;
    QSqlDatabase episodePublicDate;
    static QString simplifyTags(const QJsonArray &tags);
    static int determineSubjectType(int originalType, const QJsonArray &tags);
    static QString processImageUrl(const QString &url);
    static bool executeQuery(QSqlQuery &query, const QString &errorMsg = "");
};

#endif // SQL_H