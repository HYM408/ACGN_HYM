#include "sql.h"
#include <QDir>
#include <QSqlError>
#include <QSqlRecord>
#include <QJsonArray>
#include <QJsonObject>

DatabaseManager::DatabaseManager(QObject* parent) : QObject(parent)
{   // 初始化
    database = QSqlDatabase::addDatabase("QSQLITE");
    episodePublicDate = QSqlDatabase::addDatabase("QSQLITE", "episode_public_date_connection");
    database.setDatabaseName("data/data.db");
    episodePublicDate.setDatabaseName("data/episode_public_date.db");
}

DatabaseManager::~DatabaseManager()
{
    database.close();
    episodePublicDate.close();
}

void DatabaseManager::openDatabase()
{   // 打开数据库
    QDir dir("data");
    if (!dir.exists()) dir.mkpath(".");
    database.open();
    episodePublicDate.open();
}

void DatabaseManager::initTables()
{   // 初始化表
    QSqlQuery query;
    const QStringList tables = {
        "CREATE TABLE IF NOT EXISTS collection ("
        // collection表
        "subject_id INTEGER PRIMARY KEY, vol_status INTEGER, ep_status INTEGER, subject_type INTEGER, type INTEGER, rate INTEGER, subject_date TEXT, subject_name TEXT, subject_name_cn TEXT, subject_eps INTEGER, subject_volumes INTEGER, subject_images_common TEXT, updated_at TEXT)",
        // episode表
        "CREATE TABLE IF NOT EXISTS episode_collection ("
        "subject_id INTEGER, id INTEGER, ep REAL, sort INTEGER, name TEXT, name_cn TEXT, episode_type INTEGER, collection_type INTEGER, created_at TEXT DEFAULT (datetime('now')), PRIMARY KEY(subject_id, id))",
        // subjects表
        "CREATE TABLE IF NOT EXISTS subjects ("
        "id INTEGER PRIMARY KEY, name TEXT NOT NULL, name_cn TEXT, date TEXT, total_episodes INTEGER, volumes INTEGER, summary TEXT, rating_rank INTEGER, rating_score REAL, rating_total INTEGER, collect INTEGER, on_hold INTEGER, dropped INTEGER, wish INTEGER, doing INTEGER, tags TEXT, created_at TEXT DEFAULT (datetime('now')))"
    };
    for (const auto &sql : tables) query.exec(sql);
    // episode公共数据表
    QSqlQuery queryEpisode(QSqlDatabase::database("episode_public_date_connection"));
    queryEpisode.exec(
        "CREATE TABLE IF NOT EXISTS episode_public_date ("
        "subject_id INTEGER NOT NULL, id INTEGER NOT NULL, airdate TEXT NOT NULL, sort INTEGER, PRIMARY KEY (subject_id, id))"
    );
}

QString DatabaseManager::simplifyTags(const QJsonArray &tags)
{   // tag处理
    if (tags.isEmpty()) return {};
    QJsonObject tagObject;
    for (const auto &tag : tags) {
        QJsonObject obj = tag.toObject();
        QString name = obj["name"].toString().trimmed();
        if (!name.isEmpty()) tagObject[name] = obj["count"].toInt();
    }
    return QJsonDocument(tagObject).toJson(QJsonDocument::Compact);
}

int DatabaseManager::determineSubjectType(int originalType, const QJsonArray &tags)
{   // 类型处理
    if (originalType != 1) return originalType;
    for (const auto &tag : tags) if (tag.toObject()["name"].toString().trimmed() == "漫画") return 8;
    return 7;
}

QString DatabaseManager::processImageUrl(const QString &url)
{   // 图片url处理
    const QString prefix = "https://lain.bgm.tv/r/400/pic/cover/l/";
    return url.startsWith(prefix) ? url.mid(prefix.length()) : url;
}

QJsonObject DatabaseManager::rowToJsonObject(const QSqlRecord &record)
{   // 构建JSON
    QJsonObject obj;
    for (int i = 0; i < record.count(); ++i) obj[record.fieldName(i)] = QJsonValue::fromVariant(record.value(i));
    return obj;
}

bool DatabaseManager::executeQuery(QSqlQuery &query, const QString &errorMsg)
{   // 查询操作
    if (!query.exec()) {
        if (!errorMsg.isEmpty()) qDebug() << errorMsg << ":" << query.lastError().text();
        return false;
    }
    return true;
}

// =============== collection表 ===============
bool DatabaseManager::insertManyCollectionData(const QJsonArray &jsonArray)
{   // 批量插入多条数据
    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO collection VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?)");
    for (const auto &value : jsonArray) {
        QJsonObject data = value.toObject();
        QJsonObject subject = data["subject"].toObject();
        QJsonArray tags = subject["tags"].toArray();
        int finalType = determineSubjectType(data["subject_type"].toInt(), tags);
        query.addBindValue(data["subject_id"].toInt());
        query.addBindValue(data["vol_status"].toInt());
        query.addBindValue(data["ep_status"].toInt());
        query.addBindValue(finalType);
        query.addBindValue(data["type"].toInt());
        query.addBindValue(data["rate"].toInt());
        query.addBindValue(subject["date"].toString());
        query.addBindValue(subject["name"].toString());
        query.addBindValue(subject["name_cn"].toString());
        query.addBindValue(subject["eps"].toInt());
        query.addBindValue(subject["volumes"].toInt());
        query.addBindValue(processImageUrl(subject["images"].toObject()["common"].toString()));
        query.addBindValue(data["updated_at"].toString());
        if (!executeQuery(query, "插入收藏失败")) return false;
        query.finish();
    }
    return true;
}

QVector<CollectionData> DatabaseManager::getCollectionBySubjectTypeAndType(int subjectType, int typeValue)
{   // 获取所有收藏
    QVector<CollectionData> results;
    QSqlQuery query;
    query.prepare("SELECT * FROM collection WHERE subject_type = ? AND type = ? ORDER BY updated_at DESC");
    query.addBindValue(subjectType);
    query.addBindValue(typeValue);
    if (!executeQuery(query, "获取收藏失败")) return results;
    while (query.next()) {
        CollectionData collectiondata;
        collectiondata.subject_id = query.value("subject_id").toInt();
        collectiondata.vol_status = query.value("vol_status").toInt();
        collectiondata.ep_status = query.value("ep_status").toInt();
        collectiondata.subject_type = query.value("subject_type").toInt();
        collectiondata.type = query.value("type").toInt();
        collectiondata.rate = query.value("rate").toInt();
        collectiondata.subject_date = query.value("subject_date").toString();
        collectiondata.subject_name = query.value("subject_name").toString();
        collectiondata.subject_name_cn = query.value("subject_name_cn").toString();
        collectiondata.subject_eps = query.value("subject_eps").toInt();
        collectiondata.subject_volumes = query.value("subject_volumes").toInt();
        collectiondata.subject_images_common = query.value("subject_images_common").toString();
        if (!collectiondata.subject_images_common.isEmpty()) collectiondata.subject_images_common = "https://lain.bgm.tv/r/400/pic/cover/l/" + collectiondata.subject_images_common;
        results.append(collectiondata);
    }
    return results;
}

QJsonObject DatabaseManager::getStatusCountsBySubjectType(int subjectType)
{   // 统状态计藏数量
    QJsonObject statusCounts;
    QSqlQuery query;
    query.prepare("SELECT type, COUNT(*) as count FROM collection WHERE subject_type = ? GROUP BY type");
    query.addBindValue(subjectType);
    if (!executeQuery(query, "状态统计失败")) return statusCounts;
    while (query.next()) statusCounts[query.value("type").toString()] = query.value("count").toInt();
    return statusCounts;
}

QJsonObject DatabaseManager::getCollectionBySubjectId(int subjectId)
{   // 根据subject_id获取单条记录
    QJsonObject result;
    QSqlQuery query;
    query.prepare("SELECT * FROM collection WHERE subject_id = ?");
    query.addBindValue(subjectId);
    if (!executeQuery(query, "获取单条收藏失败")) return result;
    if (query.next()) {
        result = rowToJsonObject(query.record());
        QString imageUrl = result["subject_images_common"].toString();
        if (!imageUrl.isEmpty()) result["subject_images_common"] = "https://lain.bgm.tv/r/400/pic/cover/l/" + imageUrl;
    }
    return result;
}

bool DatabaseManager::updateCollectionFields(int subjectId, const QJsonObject &fields, bool updateTimestamp)
{   // 更新指定subject_id的某个字段值
    QStringList setParts;
    QVariantList values;
    for (auto it = fields.begin(); it != fields.end(); ++it) {
        setParts << it.key() + " = ?";
        values << it.value().toVariant();
    }
    if (updateTimestamp) {
        setParts << "updated_at = ?";
        values << QDateTime::currentDateTime().toString(Qt::ISODate);
    }
    QSqlQuery query;
    query.prepare(QString("UPDATE collection SET %1 WHERE subject_id = ?").arg(setParts.join(", ")));
    for (const auto &v : values) query.addBindValue(v);
    query.addBindValue(subjectId);
    return executeQuery(query, "更新字段失败");
}

void DatabaseManager::clearCollectionTable() const
{   // 清空collection表
    QSqlQuery query(database);
    query.exec("DELETE FROM collection");
}

// =============== episode_collection表操作 ===============
bool DatabaseManager::insertManyEpisodes(int subjectId, const QJsonArray &episodesArray)
{   // 插入剧集数据
    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO episode_collection VALUES (?,?,?,?,?,?,?,?,datetime('now'))");
    for (const auto &v : episodesArray) {
        QJsonObject item = v.toObject();
        QJsonObject ep = item["episode"].toObject();
        query.addBindValue(subjectId);
        query.addBindValue(ep["id"].toInt());
        query.addBindValue(ep["ep"].toDouble());
        query.addBindValue(ep["sort"].toDouble());
        query.addBindValue(ep["name"].toString());
        query.addBindValue(ep["name_cn"].toString());
        query.addBindValue(ep["type"].toInt());
        query.addBindValue(item["type"].toInt());
        if (!executeQuery(query, "插入剧集失败")) return false;
        query.finish();
    }
    return true;
}

QJsonArray DatabaseManager::getEpisodesBySubjectId(int subjectId)
{   // 根据subject_id获取所有剧集
    QJsonArray results;
    QSqlQuery query;
    query.prepare("SELECT * FROM episode_collection WHERE subject_id = ? ORDER BY episode_type ASC");
    query.addBindValue(subjectId);
    if (!executeQuery(query, "获取剧集失败")) return results;
    while (query.next()) results.append(rowToJsonObject(query.record()));
    return results;
}

bool DatabaseManager::deleteEpisodesBySubjectId(int subjectId)
{   // 根据subject_id删除所有剧集
    QSqlQuery query;
    query.prepare("DELETE FROM episode_collection WHERE subject_id = ?");
    query.addBindValue(subjectId);
    return executeQuery(query, "删除剧集失败");
}

bool DatabaseManager::updateAllEpisodesStatus(int subjectId, int collectionType)
{   // 根据subject_id更新剧集状态
    QSqlQuery query;
    query.prepare("UPDATE episode_collection SET collection_type = ? WHERE subject_id = ?");
    query.addBindValue(collectionType);
    query.addBindValue(subjectId);
    return executeQuery(query, "更新剧集状态失败");
}

// =============== subjects表操作 ===============
bool DatabaseManager::insertOrUpdateSubject(const QJsonObject &apiData)
{   // 插入subjects信息
    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO subjects VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,datetime('now'))");
    QJsonObject rating = apiData["rating"].toObject();
    QJsonObject collection = apiData["collection"].toObject();
    query.addBindValue(apiData["id"].toInt());
    query.addBindValue(apiData["name"].toString());
    query.addBindValue(apiData["name_cn"].toString());
    query.addBindValue(apiData["date"].toString());
    query.addBindValue(apiData["total_episodes"].toInt());
    query.addBindValue(apiData["volumes"].toInt());
    query.addBindValue(apiData["summary"].toString());
    query.addBindValue(rating["rank"].toInt());
    query.addBindValue(rating["score"].toDouble());
    query.addBindValue(rating["total"].toInt());
    query.addBindValue(collection["collect"].toInt());
    query.addBindValue(collection["on_hold"].toInt());
    query.addBindValue(collection["dropped"].toInt());
    query.addBindValue(collection["wish"].toInt());
    query.addBindValue(collection["doing"].toInt());
    query.addBindValue(simplifyTags(apiData["tags"].toArray()));
    return executeQuery(query, "插入subject失败");
}

QJsonObject DatabaseManager::getSubjectById(int subjectId)
{   // 根据ID获取subject信息
    QJsonObject result;
    QSqlQuery query;
    query.prepare("SELECT * FROM subjects WHERE id = ?");
    query.addBindValue(subjectId);
    if (!executeQuery(query)) return result;
    if (query.next()) {
        result = rowToJsonObject(query.record());
        QString tagsJson = result["tags"].toString();
        if (!tagsJson.isEmpty()) {
            QJsonDocument doc = QJsonDocument::fromJson(tagsJson.toUtf8());
            if (doc.isObject()) result["tags"] = doc.object();
        }
    }
    return result;
}

// =============== episode公共数据表 ===============
bool DatabaseManager::insertEpisodeAirdateFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    QSqlQuery query(episodePublicDate);
    query.prepare("INSERT OR REPLACE INTO episode_public_date (subject_id, id, airdate, sort) VALUES (?, ?, ?, ?)");
    episodePublicDate.transaction();
    int count = 0;
    constexpr int batchSize = 10000;
    QTextStream stream(&file);
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        if (line.isEmpty()) continue;
        QJsonDocument doc = QJsonDocument::fromJson(line.toUtf8());
        if (!doc.isObject()) continue;
        QJsonObject obj = doc.object();
        QString airdate = obj["airdate"].toString();
        if (airdate.isEmpty()) continue;
        int subjectId = obj["subject_id"].toInt();
        int id = obj["id"].toInt();
        int sort = obj["sort"].toInt();
        query.addBindValue(subjectId);
        query.addBindValue(id);
        query.addBindValue(airdate);
        query.addBindValue(sort);
        query.exec();
        ++count;
        if (count % batchSize == 0) {
            episodePublicDate.commit();
            episodePublicDate.transaction();
        }
    }
    episodePublicDate.commit();
    return true;
}

QJsonObject DatabaseManager::getEpisodeAirdates(const QList<int> &subjectIds) const
{   // 获取播出日期
    QSqlQuery query(episodePublicDate);
    QStringList placeholders;
    placeholders.fill(QString('?'), subjectIds.size());
    query.prepare(QString("SELECT * FROM episode_public_date WHERE subject_id IN (%1)").arg(placeholders.join(", ")));
    for (int id : subjectIds) query.addBindValue(id);
    query.exec();
    QJsonObject result;
    while (query.next()) {
        QString key = QString::number(query.value("subject_id").toInt());
        QJsonArray arr = result[key].toArray();
        arr.append(QJsonObject{{"id", query.value("id").toInt()}, {"airdate", query.value("airdate").toString()}, {"sort", query.value("sort").toInt()}});
        result[key] = arr;
    }
    return result;
}