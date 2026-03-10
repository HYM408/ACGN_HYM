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
    void closePublicDatabase();
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
    static bool updateAllEpisodesStatus(int subjectId, int collectionType);
    // 公共数据表
    [[nodiscard]] QJsonObject getEpisodeAirdates(const QList<int> &subjectIds) const;
    [[nodiscard]] bool insertOrUpdateSubject(const QJsonObject &apiData) const;
    [[nodiscard]] SubjectsData getSubjectById(int subjectId) const;

signals:
    void collectionDeleted();

private:
    QSqlDatabase database;
    QSqlDatabase episodePublicDate;
    static QString simplifyTags(const QJsonArray &tags);
    static int determineSubjectType(int originalType, const QJsonArray &tags);
    static QString processImageUrl(const QString &url);
    static bool executeQuery(QSqlQuery &query, const QString &errorMsg);
    static qint64 dateStringToTimestamp(const QString& dateStr);
    static qint64 dateTimeStringToTimestamp(const QString& dateTimeStr);
    static QString timestampToDateString(qint64 timestamp);
    static QByteArray compressString(const QString& str);
    static QString decompressString(const QByteArray& data);
};

#endif // SQL_H