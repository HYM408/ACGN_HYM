#include "sql.h"
#include <QDir>
#include <QSqlError>
#include <QJsonArray>

DatabaseManager::DatabaseManager(QObject* parent) : QObject(parent)
{   // 初始化
    database = QSqlDatabase::addDatabase("QSQLITE");
    episodePublicDate = QSqlDatabase::addDatabase("QSQLITE", "public_date_connection");
    database.setDatabaseName("data/data.db");
    episodePublicDate.setDatabaseName("data/public_date.db");
}

DatabaseManager::~DatabaseManager()
{
    database.close();
    episodePublicDate.close();
}

void DatabaseManager::openDatabase()
{   // 打开数据库
    const QDir dir("data");
    if (!dir.exists()) dir.mkpath(".");
    database.open();
    episodePublicDate.open();
}

void DatabaseManager::closePublicDatabase()
{   // 关闭public_date数据库
    episodePublicDate.close();
}

void DatabaseManager::initTables()
{   // 初始化表
    QSqlQuery query;
    const QStringList tables = {
        // collection表
        "CREATE TABLE IF NOT EXISTS collection (subject_id INTEGER PRIMARY KEY, vol_status INTEGER, ep_status INTEGER, subject_type INTEGER, type INTEGER, rate INTEGER, subject_date INTEGER, subject_name TEXT, subject_name_cn TEXT, subject_eps INTEGER, subject_volumes INTEGER, updated_at INTEGER)",
        // episode表
        "CREATE TABLE IF NOT EXISTS episode_collection (subject_id INTEGER, episode_id INTEGER, ep INTEGER, sort INTEGER, name TEXT, name_cn TEXT, episode_type INTEGER, collection_type INTEGER, created_at INTEGER DEFAULT (strftime('%s','now')), PRIMARY KEY(subject_id, episode_id))",
        // game_data表
        "CREATE TABLE IF NOT EXISTS game_data (subject_id INTEGER PRIMARY KEY, launch_path TEXT, play_duration INTEGER)"
    };
    for (const QString &sql : tables) query.exec(sql);
    const QStringList publicTables = {
        // episode公共数据表
        "CREATE TABLE IF NOT EXISTS episode_public_date (subject_id INTEGER, episode_id INTEGER, airdate INTEGER, sort INTEGER, PRIMARY KEY (subject_id, episode_id))",
        // subject公共数据表
        "CREATE TABLE IF NOT EXISTS subject_public_date (subject_id INTEGER PRIMARY KEY, name TEXT, name_cn TEXT, summary BLOB, tags TEXT, meta_tags TEXT, score INTEGER, rank INTEGER, date INTEGER, score_details TEXT, doing INTEGER, done INTEGER, dropped INTEGER, on_hold INTEGER, wish INTEGER)",
        // character公共数据表
        "CREATE TABLE IF NOT EXISTS character_public_date (character_id INTEGER PRIMARY KEY, name TEXT, name_cn TEXT)",
        // subject character对应表
        "CREATE TABLE IF NOT EXISTS subject_character (subject_id INTEGER, character_id INTEGER, type INTEGER, PRIMARY KEY (subject_id, character_id))",
        // person公共数据表
        "CREATE TABLE IF NOT EXISTS person (person_id INTEGER PRIMARY KEY, name TEXT, name_cn TEXT)",
        // person character对应表
        "CREATE TABLE IF NOT EXISTS person_character (person_id INTEGER, subject_id INTEGER, character_id INTEGER, PRIMARY KEY (person_id, subject_id))"
    };
    QSqlQuery publicQuery(QSqlDatabase::database("public_date_connection"));
    for (const auto &sql : publicTables) publicQuery.exec(sql);
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

int DatabaseManager::determineSubjectType(const int originalType, const QJsonArray &tags)
{   // 类型处理
    if (originalType != 1) return originalType;
    for (const auto &tag : tags) if (tag.toObject()["name"].toString().trimmed() == "漫画") return 8;
    return 7;
}

QString DatabaseManager::compactScoreDetails(const QJsonObject &scoreDetails)
{   // 评分详情处理
    int scores[10] = {};
    for (auto it = scoreDetails.begin(); it != scoreDetails.end(); ++it) scores[it.key().toInt() - 1] = it.value().toInt();
    QStringList parts;
    parts.reserve(10);
    for (const int score : scores) parts << QString::number(score);
    return parts.join(',');
}

bool DatabaseManager::executeQuery(QSqlQuery &query, const QString &errorMsg)
{   // 查询操作
    if (!query.exec()) {
        if (!errorMsg.isEmpty()) qDebug() << errorMsg << ":" << query.lastError().text();
        return false;
    }
    return true;
}

qint64 DatabaseManager::dateStringToTimestamp(const QString& dateStr)
{   // yyyy-MM-dd -> Unix时间戳
    if (dateStr.isEmpty()) return 0;
    const QDate date = QDate::fromString(dateStr, "yyyy-MM-dd");
    const QDateTime dt(date.startOfDay(QTimeZone::utc()));
    return dt.toSecsSinceEpoch();
}

qint64 DatabaseManager::dateTimeStringToTimestamp(const QString& dateTimeStr)
{   // ISO 8601 日期 -> Unix时间戳
    if (dateTimeStr.isEmpty()) return 0;
    const QDateTime dt = QDateTime::fromString(dateTimeStr, Qt::ISODate);
    return dt.toSecsSinceEpoch();
}

QString DatabaseManager::timestampToDateString(const qint64 timestamp)
{   // Unix时间戳 -> yyyy-MM-dd
    if (timestamp == 0) return {};
    const QDateTime dt = QDateTime::fromSecsSinceEpoch(timestamp, QTimeZone::utc());
    return dt.toString("yyyy-MM-dd");
}

QByteArray DatabaseManager::compressString(const QString &str)
{   // 压缩字符串
    const QByteArray original = str.toUtf8();
    return qCompress(original, 9);
}

QString DatabaseManager::decompressString(const QByteArray &data)
{   // 解压缩字符串
    const QByteArray uncompressed = qUncompress(data);
    return QString::fromUtf8(uncompressed);
}

QString DatabaseManager::sanitizeString(const QString &str)
{   // 清理字符
    QString result = str;
    result.replace(QStringLiteral("♭"), QStringLiteral("b"));
    return result;
}

// =============== collection表 ===============
bool DatabaseManager::insertManyCollectionData(const QJsonArray &jsonArray)
{   // 批量插入多条数据
    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO collection VALUES (?,?,?,?,?,?,?,?,?,?,?,?)");
    for (const auto &value : jsonArray) {
        QJsonObject data = value.toObject();
        QJsonObject subject = data["subject"].toObject();
        QJsonArray tags = subject["tags"].toArray();
        const int finalType = determineSubjectType(data["subject_type"].toInt(), tags);
        const qint64 dateTimestamp = dateStringToTimestamp(subject["date"].toString());
        const qint64 updatedTimestamp = dateTimeStringToTimestamp(data["updated_at"].toString());
        query.addBindValue(data["subject_id"].toInt());
        query.addBindValue(data["vol_status"].toInt());
        query.addBindValue(data["ep_status"].toInt());
        query.addBindValue(finalType);
        query.addBindValue(data["type"].toInt());
        query.addBindValue(data["rate"].toInt());
        query.addBindValue(dateTimestamp);
        query.addBindValue(sanitizeString(subject["name"].toString()));
        query.addBindValue(sanitizeString(subject["name_cn"].toString()));
        query.addBindValue(subject["eps"].toInt());
        query.addBindValue(subject["volumes"].toInt());
        query.addBindValue(updatedTimestamp);
        if (!executeQuery(query, "插入收藏失败")) return false;
        query.finish();
    }
    return true;
}

CollectionData DatabaseManager::collectionFromQuery(const QSqlQuery &query)
{   // collection表解析
    CollectionData data;
    data.subject_id = query.value("subject_id").toInt();
    data.vol_status = query.value("vol_status").toInt();
    data.ep_status = query.value("ep_status").toInt();
    data.subject_type = query.value("subject_type").toInt();
    data.type = query.value("type").toInt();
    data.rate = query.value("rate").toInt();
    const int dateTimestamp = query.value("subject_date").toInt();
    data.subject_date = timestampToDateString(dateTimestamp);
    data.subject_name = query.value("subject_name").toString();
    data.subject_name_cn = query.value("subject_name_cn").toString();
    data.subject_eps = query.value("subject_eps").toInt();
    data.subject_volumes = query.value("subject_volumes").toInt();
    return data;
}

QVector<CollectionData> DatabaseManager::getCollectionBySubjectTypeAndType(const int subjectType, const int typeValue)
{   // 获取所有收藏
    QVector<CollectionData> results;
    QSqlQuery query;
    query.prepare("SELECT * FROM collection WHERE subject_type = ? AND type = ? ORDER BY updated_at DESC");
    query.addBindValue(subjectType);
    query.addBindValue(typeValue);
    if (!executeQuery(query, "获取收藏失败")) return results;
    while (query.next()) results.append(collectionFromQuery(query));
    return results;
}

QJsonObject DatabaseManager::getStatusCountsBySubjectType(const int subjectType)
{   // 统状态计藏数量
    QJsonObject statusCounts;
    QSqlQuery query;
    query.prepare("SELECT type, COUNT(*) as count FROM collection WHERE subject_type = ? GROUP BY type");
    query.addBindValue(subjectType);
    if (!executeQuery(query, "状态统计失败")) return statusCounts;
    while (query.next()) statusCounts[query.value("type").toString()] = query.value("count").toInt();
    return statusCounts;
}

CollectionData DatabaseManager::getCollectionBySubjectId(const int subjectId)
{   // 根据subject_id获取单条记录
    QSqlQuery query;
    query.prepare("SELECT * FROM collection WHERE subject_id = ?");
    query.addBindValue(subjectId);
    if (!executeQuery(query, "获取单条收藏失败") || !query.next()) return {};
    return collectionFromQuery(query);
}

bool DatabaseManager::updateCollectionFields(const int subjectId, const QJsonObject &fields, const bool updateTimestamp)
{   // 更新指定subject_id的某个字段值
    QStringList setParts;
    QVariantList values;
    for (auto it = fields.begin(); it != fields.end(); ++it) {
        setParts << it.key() + " = ?";
        values << it.value().toVariant();
    }
    if (updateTimestamp) {
        setParts << "updated_at = ?";
        values << QDateTime::currentDateTime().toSecsSinceEpoch();
    }
    QSqlQuery query;
    query.prepare(QString("UPDATE collection SET %1 WHERE subject_id = ?").arg(setParts.join(", ")));
    for (const auto &v : values) query.addBindValue(v);
    query.addBindValue(subjectId);
    return executeQuery(query, "更新字段失败");
}

bool DatabaseManager::deleteCollectionBySubjectId(const int subjectId)
{   // 删除指定subject_id
    QSqlQuery query;
    query.prepare("DELETE FROM collection WHERE subject_id = ?");
    query.addBindValue(subjectId);
    const bool success = executeQuery(query, "删除收藏失败");
    if (success) emit collectionDeleted();
    return success;
}

void DatabaseManager::clearCollectionTable() const
{   // 清空collection表
    QSqlQuery query(database);
    query.exec("DELETE FROM collection");
    query.exec("VACUUM");
}

// =============== episode_collection表 ===============
bool DatabaseManager::insertManyEpisodes(const int subjectId, const QJsonArray &episodesArray)
{   // 插入剧集数据
    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO episode_collection VALUES (?,?,?,?,?,?,?,?,strftime('%s','now'))");
    for (const auto &v : episodesArray) {
        QJsonObject item = v.toObject();
        QJsonObject ep = item["episode"].toObject();
        query.addBindValue(subjectId);
        query.addBindValue(ep["id"].toInt());
        query.addBindValue(ep["ep"].toDouble() * 10.0);
        query.addBindValue(ep["sort"].toDouble() * 10.0);
        query.addBindValue(ep["name"].toString());
        query.addBindValue(ep["name_cn"].toString());
        query.addBindValue(ep["type"].toInt());
        query.addBindValue(item["type"].toInt());
        if (!executeQuery(query, "插入剧集失败")) return false;
        query.finish();
    }
    return true;
}

QVector<EpisodeData> DatabaseManager::getEpisodesBySubjectId(const int subjectId)
{   // 根据subject_id获取所有剧集
    QVector<EpisodeData> results;
    QSqlQuery query;
    query.prepare("SELECT * FROM episode_collection WHERE subject_id = ? ORDER BY episode_type ASC");
    query.addBindValue(subjectId);
    if (!executeQuery(query, "获取剧集失败")) return results;
    while (query.next()) {
        EpisodeData ep;
        ep.subject_id = query.value("subject_id").toInt();
        ep.episode_id = query.value("episode_id").toInt();
        ep.ep = query.value("ep").toInt() / 10.0;
        ep.sort = query.value("sort").toInt() / 10.0;
        ep.name = query.value("name").toString();
        ep.name_cn = query.value("name_cn").toString();
        ep.episode_type = query.value("episode_type").toInt();
        ep.collection_type = query.value("collection_type").toInt();
        results.append(ep);
    }
    return results;
}

bool DatabaseManager::deleteEpisodesBySubjectId(const int subjectId)
{   // 根据subject_id删除所有剧集
    QSqlQuery query;
    query.prepare("DELETE FROM episode_collection WHERE subject_id = ?");
    query.addBindValue(subjectId);
    return executeQuery(query, "删除剧集失败");
}

bool DatabaseManager::updateAllEpisodesStatus(const int subjectId, const int collectionType)
{   // 根据subject_id更新剧集状态
    QSqlQuery query;
    query.prepare("UPDATE episode_collection SET collection_type = ? WHERE subject_id = ?");
    query.addBindValue(collectionType);
    query.addBindValue(subjectId);
    return executeQuery(query, "更新剧集状态失败");
}

void DatabaseManager::clearEpisodeCollectionTable() const
{   // 清空episode_collection表
    QSqlQuery query(database);
    query.exec("DELETE FROM episode_collection");
    query.exec("VACUUM");
}

// =============== game_data表 ===============
bool DatabaseManager::updateGameData(const int subjectId, const QJsonObject &fields)
{   // 更新game数据
    QStringList setParts;
    QVariantList values;
    for (QJsonObject::const_iterator it = fields.begin(); it != fields.end(); ++it) {
        setParts << it.key() + " = ?";
        values << it.value().toVariant();
    }
    QSqlQuery query;
    query.prepare(QString("UPDATE game_data SET %1 WHERE subject_id = ?").arg(setParts.join(", ")));
    for (const QVariant &v : values) query.addBindValue(v);
    query.addBindValue(subjectId);
    if (!executeQuery(query, "更新game数据失败")) return false;
    if (query.numRowsAffected() == 0) {
        QStringList insertFields = {"subject_id"};
        QVariantList insertValues = {subjectId};
        for (QJsonObject::const_iterator it = fields.begin(); it != fields.end(); ++it) {
            insertFields << it.key();
            insertValues << it.value().toVariant();
        }
        QStringList placeholders;
        placeholders.fill("?", insertFields.size());
        QSqlQuery insertQuery;
        insertQuery.prepare(QString("INSERT INTO game_data (%1) VALUES (%2)").arg(insertFields.join(", ")).arg(placeholders.join(", ")));
        for (const QVariant &v : insertValues) insertQuery.addBindValue(v);
        return executeQuery(insertQuery, "插入game data失败");
    }
    return true;
}

QVector<GameData> DatabaseManager::getGameData(const QList<int> &subjectIds)
{   // 根据subject_id获取game数据
    QVector<GameData> results;
    QStringList placeholders;
    placeholders.fill("?", subjectIds.size());
    QSqlQuery query;
    query.prepare(QString("SELECT subject_id, launch_path, play_duration FROM game_data WHERE subject_id IN (%1)").arg(placeholders.join(", ")));
    for (const int id : subjectIds) query.addBindValue(id);
    if (!executeQuery(query, "获取game data失败")) return results;
    while (query.next()) {
        GameData data;
        data.subject_id = query.value("subject_id").toInt();
        data.launch_path = query.value("launch_path").toString();
        data.play_duration = static_cast<double>(query.value("play_duration").toLongLong()) / 3600.0;
        results.append(data);
    }
    return results;
}

// =============== episode公共数据表 ===============
QJsonObject DatabaseManager::getEpisodeAirdates(const QList<int> &subjectIds) const
{   // 获取播出日期
    QSqlQuery query(episodePublicDate);
    QStringList placeholders;
    placeholders.fill(QString('?'), subjectIds.size());
    query.prepare(QString("SELECT * FROM episode_public_date WHERE subject_id IN (%1)").arg(placeholders.join(", ")));
    for (const int episode_id : subjectIds) query.addBindValue(episode_id);
    query.exec();
    QJsonObject result;
    while (query.next()) {
        QString key = QString::number(query.value("subject_id").toInt());
        QString airdateStr = timestampToDateString(query.value("airdate").toLongLong());
        QJsonArray arr = result[key].toArray();
        double sort = query.value("sort").toInt();
        arr.append(QJsonObject{{"episode_id", query.value("episode_id").toInt()}, {"airdate", airdateStr}, {"sort", sort}});
        result[key] = arr;
    }
    return result;
}

// =============== subject公共数据表 ===============
bool DatabaseManager::insertOrUpdateSubject(const QJsonObject &apiData) const
{   // 插入subjects信息(API)
    QSqlQuery query(episodePublicDate);
    query.prepare("INSERT OR REPLACE INTO subject_public_date VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
    query.addBindValue(apiData["id"].toInt());
    query.addBindValue(apiData["name"].toString());
    query.addBindValue(apiData["name_cn"].toString());
    query.addBindValue(compressString(apiData["summary"].toString()));
    query.addBindValue(simplifyTags(apiData["tags"].toArray()));
    query.addBindValue(QJsonDocument(apiData["meta_tags"].toArray()).toJson(QJsonDocument::Compact));
    QJsonObject rating = apiData["rating"].toObject();
    query.addBindValue(rating["score"].toDouble() * 10.0);
    query.addBindValue(rating["rank"].toInt());
    query.addBindValue(dateStringToTimestamp(apiData["date"].toString()));
    query.addBindValue(compactScoreDetails(rating["count"].toObject()));
    QJsonObject collection = apiData["collection"].toObject();
    query.addBindValue(collection["doing"].toInt());
    query.addBindValue(collection["collect"].toInt());
    query.addBindValue(collection["dropped"].toInt());
    query.addBindValue(collection["on_hold"].toInt());
    query.addBindValue(collection["wish"].toInt());
    return executeQuery(query, "插入subject失败");
}

SubjectsData DatabaseManager::getSubjectById(const int subjectId) const
{   // // 根据ID获取subject信息
    QSqlQuery query(episodePublicDate);
    query.prepare("SELECT * FROM subject_public_date WHERE subject_id = ?");
    query.addBindValue(subjectId);
    if (!query.exec() || !query.next()) return {};
    SubjectsData data;
    data.subject_id = query.value("subject_id").toInt();
    data.name = query.value("name").toString();
    data.name_cn = query.value("name_cn").toString();
    const QByteArray compressedSummary = query.value("summary").toByteArray();
    if (!compressedSummary.isEmpty()) data.summary = decompressString(compressedSummary);
    const QString tagsJson = query.value("tags").toString();
    if (!tagsJson.isEmpty()) data.tags = QJsonDocument::fromJson(tagsJson.toUtf8()).object();
    data.meta_tags = query.value("meta_tags").toString();
    data.rating_score = query.value("score").toInt() / 10.0;
    data.rating_rank = query.value("rank").toInt();
    data.date = timestampToDateString(query.value("date").toLongLong());
    for (const QString &part : query.value("score_details").toString().split(',')) data.score_details.append(part.toInt());
    data.doing = query.value("doing").toInt();
    data.collect = query.value("done").toInt();;
    data.dropped = query.value("dropped").toInt();
    data.on_hold = query.value("on_hold").toInt();
    data.wish = query.value("wish").toInt();
    return data;
}

// =============== character相关 ===============
QVector<CharacterData> DatabaseManager::getCharactersWithPersonsBySubjectId(const int subjectId) const
{   // 获取角色信息
    QSqlQuery query(episodePublicDate);
    query.prepare(
        "SELECT c.character_id, c.name AS char_name, c.name_cn AS char_name_cn, sc.type, json_group_array(json_object('person_id', p.person_id, 'person_name', p.name, 'person_name_cn', p.name_cn)) AS persons_json "
        "FROM subject_character sc "
        "JOIN character_public_date c ON sc.character_id = c.character_id "
        "LEFT JOIN person_character pc ON pc.character_id = c.character_id AND pc.subject_id = sc.subject_id "
        "LEFT JOIN person p ON pc.person_id = p.person_id "
        "WHERE sc.subject_id = ? "
        "GROUP BY c.character_id, c.name, c.name_cn, sc.type "
        "ORDER BY sc.type"
    );
    query.addBindValue(subjectId);
    if (!query.exec()) return {};
    QVector<CharacterData> results;
    while (query.next()) {
        CharacterData data;
        data.character_id = query.value("character_id").toInt();
        data.character_name = query.value("char_name").toString();
        data.character_name_cn = query.value("char_name_cn").toString();
        data.type = query.value("type").toInt();
        QString personsJsonStr = query.value("persons_json").toString();
        QJsonArray personsArray = QJsonDocument::fromJson(personsJsonStr.toUtf8()).array();
        for (const auto &val : personsArray) {
            QJsonObject obj = val.toObject();
            PersonInfo info;
            info.person_id = obj["person_id"].toInt();
            info.person_name = obj["person_name"].toString();
            info.person_name_cn = obj["person_name_cn"].toString();
            data.persons.append(info);
        }
        results.append(data);
    }
    return results;
}