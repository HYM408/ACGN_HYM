#include <QTimer>
#include <QApplication>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineUrlRequestInterceptor>

int main(int argc, char *argv[])
{
    const QApplication app(argc, argv);
    struct : QWebEngineUrlRequestInterceptor {
        QStringList patterns;
        void interceptRequest(QWebEngineUrlRequestInfo& info) override {
            const QString url = info.requestUrl().toString();
            for (const auto &pat : patterns) {
                if (url.contains(pat)) {
                    printf("VIDEO_URL:%s\n", qPrintable(url));
                    QCoreApplication::quit();
                    return;
                }
            }
        }
    } interceptor;
    interceptor.patterns = QApplication::arguments().mid(2);
    QWebEngineProfile::defaultProfile()->setUrlRequestInterceptor(&interceptor);
    QWebEnginePage page;
    page.load(QApplication::arguments().value(1));
    QTimer::singleShot(20000, &app, &QApplication::quit);
    return QApplication::exec();
}