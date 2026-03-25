#include "progress_util.h"
#include <QJsonArray>
#include "../sql/sql.h"

QString computeProgressText(CollectionData &collection, const QJsonObject &airdatesJson, const DatabaseManager *db)
{   // 计算进度
    if (collection.subject_type == 2) {
        const QDate currentDate = QDate::currentDate();
        bool hasAirdate = false;
        QDate earliest, latest;
        int airedCount = 0;
        QJsonArray episodes = airdatesJson.value(QString::number(collection.subject_id)).toArray();
        const int totalEpisodes = collection.subject_eps > 0 ? collection.subject_eps : static_cast<int>(episodes.size());
        QString totalEpsStr = totalEpisodes > 0 ? QString::number(totalEpisodes) : "--";
        for (const auto &epVal : episodes) {
            QJsonObject epObj = epVal.toObject();
            QString dateStr = epObj["airdate"].toString();
            QDate airDate = QDate::fromString(dateStr, Qt::ISODate);
            if (!airDate.isValid()) {
                airDate = QDate::fromString(collection.subject_date, Qt::ISODate);
                if (!airDate.isValid()) continue;
            }
            hasAirdate = true;
            if (!earliest.isValid() || airDate < earliest) earliest = airDate;
            if (!latest.isValid() || airDate > latest) latest = airDate;
            if (airDate <= currentDate) ++airedCount;
        }
        QString progress;
        if (totalEpisodes > 0 && collection.ep_status >= totalEpisodes) progress = "已看完";
        else progress = QString("看到 %1").arg(collection.ep_status);
        if (!hasAirdate) {
            const QDate subjectDate = QDate::fromString(collection.subject_date, Qt::ISODate);
            if (!subjectDate.isValid()) return QString("未开播 · 预定全 %1 话").arg(totalEpsStr);
            if (subjectDate <= currentDate) return QString("已完结 · 全 %1 话 · %2").arg(totalEpsStr, progress);
            return QString("%1 开播 · 预定全 %2 话").arg(collection.subject_date, totalEpsStr);
        }
        if (earliest > currentDate) return QString("%1 开播 · 预定全 %2 话").arg(earliest.toString(Qt::ISODate), totalEpsStr);
        if (latest < currentDate) return QString("已完结 · 全 %1 话 · %2").arg(totalEpsStr, progress);
        return QString("连载至第 %1 话 · 全 %2 话 · %3").arg(airedCount).arg(totalEpsStr, progress);
    }
    if (collection.subject_type == 7 || collection.subject_type == 8 || collection.subject_type == 1) {
        collection.subject_volumes = db->countSubjectRelations(collection.subject_id);
        QString totalVols = collection.subject_volumes > 0 ? QString::number(collection.subject_volumes) : "--";
        QString totalEps = collection.subject_eps > 0 ? QString::number(collection.subject_eps) : "--";
        return QString("全 %1 卷 · 全 %2 话").arg(totalVols, totalEps);
    }
    if (collection.subject_type == 4) {
        const QString date = collection.subject_date;
        return QString("%1 发售").arg(date.isEmpty() || date == "0" ? "--" : date);
    }
    return {};
}