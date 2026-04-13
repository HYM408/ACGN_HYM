#include "temp_mail_api.h"
#include <QJsonArray>
#include <QJsonObject>
#include "../utils/network_util.h"

void TempMailApi::generateTempEmail(const std::function<void(const QString &email, const QString &error)> &callback)
{   // 生成临时邮箱
    const QString url = "https://api.tempmail.ing/api/generate";
    const QNetworkRequest req(url);
    new RequestHandler(m_manager, req, "POST", QByteArray(), 3, [callback](const QByteArray &rawData, const int, const QString &error) {
        if (!error.isEmpty()) return callback(QString(), error);
        callback(QJsonDocument::fromJson(rawData).object().value("email").toObject().value("address").toString(), error);
    }, nullptr);
}

void TempMailApi::fetchEmails(const QString &email, const std::function<void(const QString &code, const QString &error)> &callback)
{   // 获取邮件
    const QString url = "https://api.tempmail.ing/api/emails/" + email;
    const QNetworkRequest req(url);
    new RequestHandler(m_manager, req, "GET", QByteArray(), 3, [callback](const QByteArray &rawData, const int, const QString &error) {
        if (!error.isEmpty()) return callback(QString(), error);
        QJsonArray emailsArray = QJsonDocument::fromJson(rawData).object().value("emails").toArray();
        if (emailsArray.isEmpty()) return callback(QString(), "暂无邮件");
        QString content = emailsArray[0].toObject().value("content").toString();
        static const QRegularExpression re("\\d{6}");
        QString verificationCode = re.match(content).captured(0);
        callback(verificationCode, QString());
    }, nullptr);
}