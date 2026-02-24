#include "rss.h"
#include <QTimer>
#include <QJsonArray>
#include <QCoreApplication>
#include "sql.h"
#include "config.h"
#include "api/bangumi_api.h"
#include "utils/network_util.h"

void updateRSS(BangumiAPI* api)
{   // 获取 Guid
    qDebug() << "RSS：开始获取";
    const QString userId = getConfig("Bangumi/user_id").toString();
    if (userId.isEmpty()) return;
    static QNetworkAccessManager netManager;
    static const QRegularExpression guidPattern(R"(<guid[^>]*>\s*(?:https?://)?[^/]+/user/\d+/timeline/(\d+)\s*</guid>)");
    const QString baseUrl = getConfig("Bangumi/bangumi_base_url").toString();
    const QString oldGuid = getConfig("Bangumi/rss_guid").toString();
    const QString token = getConfig("Bangumi/access_token").toString();
    QNetworkRequest request(QUrl(baseUrl % "feed/user/" % userId % "/timeline?type=subject"));
    request.setHeader(QNetworkRequest::UserAgentHeader, "ACGN_HYM/1.0");
    request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
    QString content = sendRequestHtml(netManager, request, "GET", {}, 3, nullptr, nullptr);
    QString newGuid = guidPattern.match(content).captured(1);
    if (newGuid == "") return;
    if (oldGuid == "0") {
        setConfig("Bangumi/rss_guid", newGuid);
        qDebug() << "RSS: 首次获取";
    } else if (oldGuid != newGuid) {
        const QJsonArray collections = api->getUserCollections(false, 3);
        DatabaseManager::insertManyCollectionData(collections);
        setConfig("Bangumi/rss_guid", newGuid);
        qDebug() << "RSS: 更新完成";

    } else qDebug() << "RSS无变化";
}

void startRSS(BangumiAPI* api)
{   // 启动 RSS 定时
    qDebug() << "启动RSS监控";
    updateRSS(api);
    static auto* timer = new QTimer(QCoreApplication::instance());
    QObject::connect(timer, &QTimer::timeout, [api]() {updateRSS(api);});
    timer->start(5 * 60 * 1000);
}