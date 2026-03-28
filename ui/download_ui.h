/********************************************************************************
** Form generated from reading UI file 'download_ui.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef DOWNLOAD_UI_H
#define DOWNLOAD_UI_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DownloaderPage
{
public:
    QHBoxLayout *horizontalLayout;
    QFrame *mainFrame;
    QHBoxLayout *horizontalLayout_3;
    QScrollArea *scrollAreaFileList;
    QWidget *fileListContent;
    QVBoxLayout *fileListLayout;
    QFrame *rightPanelFrame;
    QVBoxLayout *verticalLayout_3;
    QFrame *pathFrame;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *lineEditDownloadPath;
    QPushButton *btnSelectPath;
    QScrollArea *scrollAreaTaskList;
    QWidget *taskListContent;
    QVBoxLayout *taskListLayout;

    void setupUi(QWidget *DownloaderPage)
    {
        if (DownloaderPage->objectName().isEmpty())
            DownloaderPage->setObjectName("DownloaderPage");
        DownloaderPage->resize(1525, 942);
        DownloaderPage->setStyleSheet(QString::fromUtf8(""));
        horizontalLayout = new QHBoxLayout(DownloaderPage);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        mainFrame = new QFrame(DownloaderPage);
        mainFrame->setObjectName("mainFrame");
        mainFrame->setFrameShape(QFrame::Shape::StyledPanel);
        mainFrame->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_3 = new QHBoxLayout(mainFrame);
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        scrollAreaFileList = new QScrollArea(mainFrame);
        scrollAreaFileList->setObjectName("scrollAreaFileList");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(scrollAreaFileList->sizePolicy().hasHeightForWidth());
        scrollAreaFileList->setSizePolicy(sizePolicy);
        scrollAreaFileList->setStyleSheet(QString::fromUtf8("border: none;"));
        scrollAreaFileList->setWidgetResizable(true);
        fileListContent = new QWidget();
        fileListContent->setObjectName("fileListContent");
        fileListContent->setGeometry(QRect(0, 0, 762, 940));
        fileListContent->setStyleSheet(QString::fromUtf8("border-right: 2px solid #c0c0c0;"));
        fileListLayout = new QVBoxLayout(fileListContent);
        fileListLayout->setObjectName("fileListLayout");
        scrollAreaFileList->setWidget(fileListContent);

        horizontalLayout_3->addWidget(scrollAreaFileList);

        rightPanelFrame = new QFrame(mainFrame);
        rightPanelFrame->setObjectName("rightPanelFrame");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(rightPanelFrame->sizePolicy().hasHeightForWidth());
        rightPanelFrame->setSizePolicy(sizePolicy1);
        rightPanelFrame->setFrameShape(QFrame::Shape::StyledPanel);
        rightPanelFrame->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayout_3 = new QVBoxLayout(rightPanelFrame);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(0, 10, 0, 0);
        pathFrame = new QFrame(rightPanelFrame);
        pathFrame->setObjectName("pathFrame");
        pathFrame->setFrameShape(QFrame::Shape::StyledPanel);
        pathFrame->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_2 = new QHBoxLayout(pathFrame);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        lineEditDownloadPath = new QLineEdit(pathFrame);
        lineEditDownloadPath->setObjectName("lineEditDownloadPath");
        QFont font;
        font.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font.setPointSize(10);
        lineEditDownloadPath->setFont(font);
        lineEditDownloadPath->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
        lineEditDownloadPath->setReadOnly(true);

        horizontalLayout_2->addWidget(lineEditDownloadPath);

        btnSelectPath = new QPushButton(pathFrame);
        btnSelectPath->setObjectName("btnSelectPath");

        horizontalLayout_2->addWidget(btnSelectPath);


        verticalLayout_3->addWidget(pathFrame);

        scrollAreaTaskList = new QScrollArea(rightPanelFrame);
        scrollAreaTaskList->setObjectName("scrollAreaTaskList");
        sizePolicy.setHeightForWidth(scrollAreaTaskList->sizePolicy().hasHeightForWidth());
        scrollAreaTaskList->setSizePolicy(sizePolicy);
        scrollAreaTaskList->setStyleSheet(QString::fromUtf8("border:none;"));
        scrollAreaTaskList->setWidgetResizable(true);
        taskListContent = new QWidget();
        taskListContent->setObjectName("taskListContent");
        taskListContent->setGeometry(QRect(0, 0, 759, 903));
        taskListLayout = new QVBoxLayout(taskListContent);
        taskListLayout->setObjectName("taskListLayout");
        scrollAreaTaskList->setWidget(taskListContent);

        verticalLayout_3->addWidget(scrollAreaTaskList);


        horizontalLayout_3->addWidget(rightPanelFrame);


        horizontalLayout->addWidget(mainFrame);


        retranslateUi(DownloaderPage);

        QMetaObject::connectSlotsByName(DownloaderPage);
    } // setupUi

    void retranslateUi(QWidget *DownloaderPage) const
    {
        DownloaderPage->setWindowTitle(QCoreApplication::translate("DownloaderPage", "Form", nullptr));
        lineEditDownloadPath->setText(QString());
        btnSelectPath->setText(QCoreApplication::translate("DownloaderPage", "\351\200\211\346\213\251\350\267\257\345\276\204", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DownloaderPage: public Ui_DownloaderPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // DOWNLOAD_UI_H
