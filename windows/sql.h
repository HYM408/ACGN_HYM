#ifndef SQL_H
#define SQL_H

#include <QSqlQuery>
#include <QJsonObject>

struct CollectionData {
    int subject_id;
    int vol_status;
    int ep_status;
    int subject_type;
    int type;
    int rate;
    QString subject_date;
    QString subject_name;
    QString subject_name_cn;
    int subject_eps;
    int subject_volumes;
    QString subject_images_common;
};

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager() override;
    void openDatabase();
    static void initTables();
    // collection表
    static bool insertManyCollectionData(const QJsonArray &jsonArray);
    static QVector<CollectionData> getCollectionBySubjectTypeAndType(int subjectType, int typeValue);
    static QJsonObject getStatusCountsBySubjectType(int subjectType);
    static QJsonObject getCollectionBySubjectId(int subjectId);
    static bool updateCollectionFields(int subjectId, const QJsonObject &fields, bool updateTimestamp);
    void clearCollectionTable() const;
    // episode表
    static bool insertManyEpisodes(int subjectId, const QJsonArray &episodesArray);
    static QJsonArray getEpisodesBySubjectId(int subjectId);
    static bool deleteEpisodesBySubjectId(int subjectId);
    static bool updateAllEpisodesStatus(int subjectId, int collectionType = 2);
    // subjects表
    static bool insertOrUpdateSubject(const QJsonObject &apiData);
    static QJsonObject getSubjectById(int subjectId);

private:
    QSqlDatabase database;
    static QString simplifyTags(const QJsonArray &tags);
    static int determineSubjectType(int originalType, const QJsonArray &tags);
    static QString processImageUrl(const QString &url);
    static QJsonObject rowToJsonObject(const QSqlRecord &record);
    static bool executeQuery(QSqlQuery &query, const QString &errorMsg = "");
};

#endif // SQL_H