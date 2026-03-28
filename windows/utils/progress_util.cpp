#include "progress_util.h"
#include <QJsonArray>
#include "../sql/sql.h"

QString computeProgressText(const SubjectsData &subjectsData, const QJsonObject &airdatesJson)
{   // 计算进度
    if (subjectsData.subjectType == 2) {
        const QDate currentDate = QDate::currentDate();
        const QJsonArray episodes = airdatesJson.value(QString::number(subjectsData.subjectId)).toArray();
        const QDate subjectDate = QDate::fromString(subjectsData.date, Qt::ISODate);
        const int totalEpisodes = subjectsData.subjectEps > 0 ? subjectsData.subjectEps : static_cast<int>(episodes.size());
        QString totalEpsStr = totalEpisodes > 0 ? QString::number(totalEpisodes) : "--";
        if (!subjectDate.isValid()) return QString("未开播 · 预定全 %1 话").arg(totalEpsStr);
        QDate earliest, latest;
        int airedCount = 0;
        for (const auto &epVal : episodes) {
            QDate airDate = QDate::fromString(epVal.toObject()["airdate"].toString(), Qt::ISODate);
            if (!airDate.isValid()) airDate = subjectDate;
            if (!airDate.isValid()) continue;
            if (!earliest.isValid() || airDate < earliest) earliest = airDate;
            if (!latest.isValid() || airDate > latest) latest = airDate;
            if (airDate <= currentDate) ++airedCount;
        }
        QString progress;
        if (totalEpisodes > 0 && subjectsData.epStatus >= totalEpisodes) progress = "已看完";
        else progress = QString("看到 %1").arg(subjectsData.epStatus);
        if (earliest > currentDate) return QString("%1 开播 · 预定全 %2 话").arg(earliest.toString(Qt::ISODate), totalEpsStr);
        if (latest <= currentDate) return QString("已完结 · 全 %1 话 · %2").arg(totalEpsStr, progress);
        return QString("连载至第 %1 话 · 全 %2 话 · %3").arg(airedCount).arg(totalEpsStr, progress);
    }
    if (subjectsData.subjectType == 7 || subjectsData.subjectType == 8 || subjectsData.subjectType == 1) return QString("全 %1 卷 · 全 %2 话").arg(subjectsData.subjectVolumes > 0 ? QString::number(subjectsData.subjectVolumes) : "--", subjectsData.subjectEps > 0 ? QString::number(subjectsData.subjectEps) : "--");
    if (subjectsData.subjectType == 4) return QString("%1 发售").arg(subjectsData.date.isNull() ? "--" : subjectsData.date);
    return {};
}