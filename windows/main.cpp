#include <QStyle>
#include <QScreen>
#include "window.h"
#include <QElapsedTimer>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    window.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, window.size(), QApplication::primaryScreen()->availableGeometry()));
    window.show();
    return QApplication::exec();
}