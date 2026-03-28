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
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>

QT_BEGIN_NAMESPACE

class Ui_EpisodePage
{
public:
    QHBoxLayout *horizontalLayout_5;
    QFrame *mainFrame;
    QVBoxLayout *verticalLayout_9;
    QFrame *headerFrame;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnBack;
    QLabel *labelTitle;
    QPushButton *btnUpdate;
    QLabel *labelSubjectName;
    QScrollArea *scrollAreaEpisodes;
    QWidget *episodesContainer;
    QGridLayout *gridLayoutEpisodes;

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
        mainFrame = new QFrame(EpisodePage);
        mainFrame->setObjectName("mainFrame");
        mainFrame->setMinimumSize(QSize(600, 0));
        mainFrame->setMaximumSize(QSize(600, 16777215));
        mainFrame->setFrameShape(QFrame::Shape::StyledPanel);
        mainFrame->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayout_9 = new QVBoxLayout(mainFrame);
        verticalLayout_9->setObjectName("verticalLayout_9");
        verticalLayout_9->setContentsMargins(0, -1, 0, -1);
        headerFrame = new QFrame(mainFrame);
        headerFrame->setObjectName("headerFrame");
        horizontalLayout = new QHBoxLayout(headerFrame);
        horizontalLayout->setObjectName("horizontalLayout");
        btnBack = new QPushButton(headerFrame);
        btnBack->setObjectName("btnBack");
        btnBack->setMinimumSize(QSize(44, 44));
        btnBack->setMaximumSize(QSize(44, 44));
        btnBack->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"             border:none;\n"
"             border-radius:22px\n"
"             }\n"
"\n"
"             QPushButton:hover {\n"
"             background-color: rgb(237, 237, 238);\n"
"             }"));
        QIcon icon;
        icon.addFile(QString::fromUtf8("icons/back.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        btnBack->setIcon(icon);
        btnBack->setIconSize(QSize(25, 25));
        btnBack->setCheckable(true);

        horizontalLayout->addWidget(btnBack);

        labelTitle = new QLabel(headerFrame);
        labelTitle->setObjectName("labelTitle");
        labelTitle->setMinimumSize(QSize(468, 0));
        labelTitle->setMaximumSize(QSize(468, 16777215));
        QFont font;
        font.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font.setPointSize(25);
        font.setBold(true);
        labelTitle->setFont(font);
        labelTitle->setAlignment(Qt::AlignmentFlag::AlignCenter);

        horizontalLayout->addWidget(labelTitle);

        btnUpdate = new QPushButton(headerFrame);
        btnUpdate->setObjectName("btnUpdate");
        QFont font1;
        font1.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font1.setPointSize(10);
        font1.setBold(true);
        btnUpdate->setFont(font1);

        horizontalLayout->addWidget(btnUpdate);


        verticalLayout_9->addWidget(headerFrame);

        labelSubjectName = new QLabel(mainFrame);
        labelSubjectName->setObjectName("labelSubjectName");
        labelSubjectName->setMinimumSize(QSize(585, 0));
        labelSubjectName->setMaximumSize(QSize(585, 16777215));
        QFont font2;
        font2.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font2.setPointSize(15);
        font2.setBold(true);
        labelSubjectName->setFont(font2);
        labelSubjectName->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_9->addWidget(labelSubjectName);

        scrollAreaEpisodes = new QScrollArea(mainFrame);
        scrollAreaEpisodes->setObjectName("scrollAreaEpisodes");
        scrollAreaEpisodes->setStyleSheet(QString::fromUtf8("QScrollArea {\n"
"          border: none;\n"
"          background-color: transparent;\n"
"          }\n"
"\n"
"          QScrollArea > QWidget > QWidget {\n"
"          background-color: transparent;\n"
"          }\n"
"\n"
"          QScrollBar:vertical {\n"
"          border: none;\n"
"          background-color: rgba(200, 200, 200, 100);\n"
"          width: 8px;\n"
"          border-radius: 4px;\n"
"          }\n"
"\n"
"          QScrollBar::handle:vertical {\n"
"          background-color: rgba(150, 150, 150, 150);\n"
"          border-radius: 4px;\n"
"          min-height: 20px;\n"
"          }\n"
"\n"
"          QScrollBar::handle:vertical:hover {\n"
"          background-color: rgba(120, 120, 120, 200);\n"
"          }\n"
"\n"
"          QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {\n"
"          height: 0px;\n"
"          }\n"
"         "));
        scrollAreaEpisodes->setWidgetResizable(true);
        episodesContainer = new QWidget();
        episodesContainer->setObjectName("episodesContainer");
        episodesContainer->setGeometry(QRect(0, 0, 598, 480));
        gridLayoutEpisodes = new QGridLayout(episodesContainer);
        gridLayoutEpisodes->setObjectName("gridLayoutEpisodes");
        scrollAreaEpisodes->setWidget(episodesContainer);

        verticalLayout_9->addWidget(scrollAreaEpisodes);


        horizontalLayout_5->addWidget(mainFrame, 0, Qt::AlignmentFlag::AlignLeft);


        retranslateUi(EpisodePage);

        QMetaObject::connectSlotsByName(EpisodePage);
    } // setupUi

    void retranslateUi(QWidget *EpisodePage) const
    {
        btnBack->setText(QString());
        labelTitle->setText(QCoreApplication::translate("EpisodePage", "\351\200\211\351\233\206", nullptr));
        btnUpdate->setText(QString());
        labelSubjectName->setText(QString());
        (void)EpisodePage;
    } // retranslateUi

};

namespace Ui {
    class EpisodePage: public Ui_EpisodePage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // EPISODE_UI_H
