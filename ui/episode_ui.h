/********************************************************************************
** Form generated from reading UI file 'episode_ui.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef EPISODE_UI_H
#define EPISODE_UI_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>

QT_BEGIN_NAMESPACE

class Ui_EpisodePage
{
public:
    QHBoxLayout *horizontalLayout_5;
    QFrame *frame_2;
    QVBoxLayout *verticalLayout_9;
    QFrame *frame;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_14;
    QLabel *pushButton_12;
    QPushButton *pushButton_15;
    QLabel *pushButton_13;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents_2;
    QGridLayout *gridLayout_3;

    void setupUi(QWidget *EpisodePage)
    {
        if (EpisodePage->objectName().isEmpty())
            EpisodePage->setObjectName("EpisodePage");
        EpisodePage->resize(600, 600);
        EpisodePage->setMinimumSize(QSize(600, 0));
        EpisodePage->setMaximumSize(QSize(600, 16777215));
        horizontalLayout_5 = new QHBoxLayout(EpisodePage);
        horizontalLayout_5->setSpacing(0);
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        frame_2 = new QFrame(EpisodePage);
        frame_2->setObjectName("frame_2");
        frame_2->setMinimumSize(QSize(600, 0));
        frame_2->setMaximumSize(QSize(600, 16777215));
        frame_2->setFrameShape(QFrame::Shape::StyledPanel);
        frame_2->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayout_9 = new QVBoxLayout(frame_2);
        verticalLayout_9->setObjectName("verticalLayout_9");
        verticalLayout_9->setContentsMargins(0, -1, 0, -1);
        frame = new QFrame(frame_2);
        frame->setObjectName("frame");
        horizontalLayout = new QHBoxLayout(frame);
        horizontalLayout->setObjectName("horizontalLayout");
        pushButton_14 = new QPushButton(frame);
        pushButton_14->setObjectName("pushButton_14");
        pushButton_14->setMinimumSize(QSize(44, 44));
        pushButton_14->setMaximumSize(QSize(44, 44));
        pushButton_14->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"	border:none;\n"
"	border-radius:22px\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: rgb(237, 237, 238);\n"
"}"));
        QIcon icon;
        icon.addFile(QString::fromUtf8("icons/back.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        pushButton_14->setIcon(icon);
        pushButton_14->setIconSize(QSize(25, 25));
        pushButton_14->setCheckable(true);

        horizontalLayout->addWidget(pushButton_14);

        pushButton_12 = new QLabel(frame);
        pushButton_12->setObjectName("pushButton_12");
        pushButton_12->setMinimumSize(QSize(468, 0));
        pushButton_12->setMaximumSize(QSize(468, 16777215));
        QFont font;
        font.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font.setPointSize(25);
        font.setBold(true);
        pushButton_12->setFont(font);
        pushButton_12->setAlignment(Qt::AlignmentFlag::AlignCenter);

        horizontalLayout->addWidget(pushButton_12);

        pushButton_15 = new QPushButton(frame);
        pushButton_15->setObjectName("pushButton_15");
        QFont font1;
        font1.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font1.setPointSize(10);
        font1.setBold(true);
        pushButton_15->setFont(font1);

        horizontalLayout->addWidget(pushButton_15);


        verticalLayout_9->addWidget(frame);

        pushButton_13 = new QLabel(frame_2);
        pushButton_13->setObjectName("pushButton_13");
        pushButton_13->setMinimumSize(QSize(585, 0));
        pushButton_13->setMaximumSize(QSize(585, 16777215));
        QFont font2;
        font2.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font2.setPointSize(15);
        font2.setBold(true);
        pushButton_13->setFont(font2);
        pushButton_13->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_9->addWidget(pushButton_13);

        scrollArea = new QScrollArea(frame_2);
        scrollArea->setObjectName("scrollArea");
        scrollArea->setStyleSheet(QString::fromUtf8("QScrollArea {\n"
"	border: none;\n"
"	background-color: transparent;\n"
"}\n"
"\n"
"QScrollArea > QWidget > QWidget {\n"
"	background-color: transparent;\n"
"}\n"
"\n"
"QScrollBar:vertical {\n"
"	border: none;\n"
"	background-color: rgba(200, 200, 200, 100);\n"
"	width: 8px;\n"
"	border-radius: 4px;\n"
"}\n"
"\n"
"QScrollBar::handle:vertical {\n"
"	background-color: rgba(150, 150, 150, 150);\n"
"	border-radius: 4px;\n"
"	min-height: 20px;\n"
"}\n"
"\n"
"QScrollBar::handle:vertical:hover {\n"
"	background-color: rgba(120, 120, 120, 200);\n"
"}\n"
"\n"
"QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {\n"
"	height: 0px;\n"
"}\n"
"         "));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents_2 = new QWidget();
        scrollAreaWidgetContents_2->setObjectName("scrollAreaWidgetContents_2");
        scrollAreaWidgetContents_2->setGeometry(QRect(0, 0, 598, 480));
        gridLayout_3 = new QGridLayout(scrollAreaWidgetContents_2);
        gridLayout_3->setObjectName("gridLayout_3");
        scrollArea->setWidget(scrollAreaWidgetContents_2);

        verticalLayout_9->addWidget(scrollArea);


        horizontalLayout_5->addWidget(frame_2, 0, Qt::AlignmentFlag::AlignLeft);


        retranslateUi(EpisodePage);

        QMetaObject::connectSlotsByName(EpisodePage);
    } // setupUi

    void retranslateUi(QWidget *EpisodePage) const
    {
        pushButton_14->setText(QString());
        pushButton_12->setText(QCoreApplication::translate("EpisodePage", "\351\200\211\351\233\206", nullptr));
        pushButton_15->setText(QString());
        pushButton_13->setText(QString());
        (void)EpisodePage;
    } // retranslateUi

};

namespace Ui {
    class EpisodePage: public Ui_EpisodePage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // EPISODE_UI_H
