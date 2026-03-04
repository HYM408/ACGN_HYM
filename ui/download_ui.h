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
    QFrame *frame_2;
    QHBoxLayout *horizontalLayout_3;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_2;
    QFrame *frame_3;
    QVBoxLayout *verticalLayout_3;
    QFrame *frame;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *lineEdit;
    QPushButton *pushButton;
    QScrollArea *scrollArea_2;
    QWidget *scrollAreaWidgetContents_3;
    QVBoxLayout *verticalLayout_4;

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
        frame_2 = new QFrame(DownloaderPage);
        frame_2->setObjectName("frame_2");
        frame_2->setStyleSheet(QString::fromUtf8("QFrame {\n"
"       background-color: white;\n"
"       }"));
        frame_2->setFrameShape(QFrame::Shape::StyledPanel);
        frame_2->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_3 = new QHBoxLayout(frame_2);
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        scrollArea = new QScrollArea(frame_2);
        scrollArea->setObjectName("scrollArea");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(scrollArea->sizePolicy().hasHeightForWidth());
        scrollArea->setSizePolicy(sizePolicy);
        scrollArea->setStyleSheet(QString::fromUtf8("border: none;"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 762, 940));
        scrollAreaWidgetContents->setStyleSheet(QString::fromUtf8("border-right: 2px solid #c0c0c0;"));
        verticalLayout_2 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_2->setObjectName("verticalLayout_2");
        scrollArea->setWidget(scrollAreaWidgetContents);

        horizontalLayout_3->addWidget(scrollArea);

        frame_3 = new QFrame(frame_2);
        frame_3->setObjectName("frame_3");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(frame_3->sizePolicy().hasHeightForWidth());
        frame_3->setSizePolicy(sizePolicy1);
        frame_3->setFrameShape(QFrame::Shape::StyledPanel);
        frame_3->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayout_3 = new QVBoxLayout(frame_3);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(0, 10, 0, 0);
        frame = new QFrame(frame_3);
        frame->setObjectName("frame");
        frame->setFrameShape(QFrame::Shape::StyledPanel);
        frame->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_2 = new QHBoxLayout(frame);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        lineEdit = new QLineEdit(frame);
        lineEdit->setObjectName("lineEdit");
        QFont font;
        font.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font.setPointSize(10);
        lineEdit->setFont(font);
        lineEdit->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
        lineEdit->setReadOnly(true);

        horizontalLayout_2->addWidget(lineEdit);

        pushButton = new QPushButton(frame);
        pushButton->setObjectName("pushButton");

        horizontalLayout_2->addWidget(pushButton);


        verticalLayout_3->addWidget(frame);

        scrollArea_2 = new QScrollArea(frame_3);
        scrollArea_2->setObjectName("scrollArea_2");
        sizePolicy.setHeightForWidth(scrollArea_2->sizePolicy().hasHeightForWidth());
        scrollArea_2->setSizePolicy(sizePolicy);
        scrollArea_2->setStyleSheet(QString::fromUtf8("border:none;"));
        scrollArea_2->setWidgetResizable(true);
        scrollAreaWidgetContents_3 = new QWidget();
        scrollAreaWidgetContents_3->setObjectName("scrollAreaWidgetContents_3");
        scrollAreaWidgetContents_3->setGeometry(QRect(0, 0, 759, 903));
        verticalLayout_4 = new QVBoxLayout(scrollAreaWidgetContents_3);
        verticalLayout_4->setObjectName("verticalLayout_4");
        scrollArea_2->setWidget(scrollAreaWidgetContents_3);

        verticalLayout_3->addWidget(scrollArea_2);


        horizontalLayout_3->addWidget(frame_3);


        horizontalLayout->addWidget(frame_2);


        retranslateUi(DownloaderPage);

        QMetaObject::connectSlotsByName(DownloaderPage);
    } // setupUi

    void retranslateUi(QWidget *DownloaderPage) const
    {
        DownloaderPage->setWindowTitle(QCoreApplication::translate("DownloaderPage", "Form", nullptr));
        lineEdit->setText(QString());
        pushButton->setText(QCoreApplication::translate("DownloaderPage", "\351\200\211\346\213\251\350\267\257\345\276\204", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DownloaderPage: public Ui_DownloaderPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // DOWNLOAD_UI_H
