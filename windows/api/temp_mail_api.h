#ifndef TEMP_MAIL_API_H
#define TEMP_MAIL_API_H

#include <QNetworkAccessManager>

class TempMailApi : public QObject
{
    Q_OBJECT

public:
    explicit TempMailApi(QObject *parent = nullptr) : QObject(parent) {}
    void generateTempEmail(const std::function<void(const QString &email, const QString &error)> &callback);
    void fetchEmails(const QString &email, const std::function<void(const QString &code, const QString &error)> &callback);

private:
    QNetworkAccessManager m_manager;
};

#endif // TEMP_MAIL_API_H