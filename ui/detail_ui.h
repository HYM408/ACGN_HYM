/********************************************************************************
** Form generated from reading UI file 'detail_ui.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef DETAIL_UI_H
#define DETAIL_UI_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DetailPage
{
public:
    QVBoxLayout *verticalLayout_11;
    QFrame *mainFrame;
    QVBoxLayout *verticalLayout;
    QFrame *headerFrame;
    QHBoxLayout *horizontalLayout_11;
    QPushButton *btnBack;
    QPushButton *btnOpenUrl;
    QFrame *infoFrame;
    QHBoxLayout *horizontalLayout_14;
    QLabel *labelCover;
    QFrame *infoTextFrame;
    QVBoxLayout *verticalLayout_5;
    QLineEdit *lineEditTitle;
    QFrame *statsFrame;
    QHBoxLayout *horizontalLayout_2;
    QFrame *infoLabelsFrame;
    QVBoxLayout *verticalLayout_4;
    QLabel *labelStatus;
    QLabel *labelProgress;
    QFrame *ratingInfoFrame;
    QHBoxLayout *horizontalLayout;
    QLabel *labelRatingInfo;
    QPushButton *btnRating;
    QLabel *labelCollectionStats;
    QPushButton *btnStatus;
    QPushButton *btnAction;
    QTabWidget *tabWidget;
    QWidget *tabDetail;
    QVBoxLayout *verticalLayout_14;
    QFrame *frameTags;
    QTextEdit *textEditSummary;
    QWidget *tabCharacters;
    QVBoxLayout *verticalLayout_2;
    QScrollArea *scrollAreaCharacters;
    QWidget *charactersContent;
    QWidget *tabRelations;
    QVBoxLayout *verticalLayout_6;
    QScrollArea *scrollAreaRelations;
    QWidget *relationsContent;
    QWidget *tabStaff;
    QVBoxLayout *verticalLayout_3;
    QScrollArea *scrollAreaStaff;
    QWidget *staffContent;

    void setupUi(QWidget *DetailPage)
    {
        if (DetailPage->objectName().isEmpty())
            DetailPage->setObjectName("DetailPage");
        DetailPage->resize(1525, 942);
        verticalLayout_11 = new QVBoxLayout(DetailPage);
        verticalLayout_11->setSpacing(0);
        verticalLayout_11->setObjectName("verticalLayout_11");
        verticalLayout_11->setContentsMargins(0, 0, 0, 0);
        mainFrame = new QFrame(DetailPage);
        mainFrame->setObjectName("mainFrame");
        mainFrame->setMinimumSize(QSize(0, 114));
        mainFrame->setFrameShape(QFrame::Shape::StyledPanel);
        mainFrame->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayout = new QVBoxLayout(mainFrame);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        headerFrame = new QFrame(mainFrame);
        headerFrame->setObjectName("headerFrame");
        headerFrame->setStyleSheet(QString::fromUtf8("background-color: transparent;\n"
"          border: none;"));
        headerFrame->setFrameShape(QFrame::Shape::StyledPanel);
        headerFrame->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_11 = new QHBoxLayout(headerFrame);
        horizontalLayout_11->setSpacing(0);
        horizontalLayout_11->setObjectName("horizontalLayout_11");
        horizontalLayout_11->setContentsMargins(0, 0, 0, 0);
        btnBack = new QPushButton(headerFrame);
        btnBack->setObjectName("btnBack");
        btnBack->setMinimumSize(QSize(45, 45));
        btnBack->setMaximumSize(QSize(45, 45));
        btnBack->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"             background-color: transparent;\n"
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

        horizontalLayout_11->addWidget(btnBack, 0, Qt::AlignmentFlag::AlignLeft);

        btnOpenUrl = new QPushButton(headerFrame);
        btnOpenUrl->setObjectName("btnOpenUrl");
        btnOpenUrl->setMinimumSize(QSize(45, 45));
        btnOpenUrl->setMaximumSize(QSize(45, 45));
        btnOpenUrl->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"             background-color: transparent;\n"
"             border:none;\n"
"             border-radius:22px\n"
"             }\n"
"\n"
"             QPushButton:hover {\n"
"             background-color: rgb(237, 237, 238);\n"
"             }"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8("icons/openUrl.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        btnOpenUrl->setIcon(icon1);
        btnOpenUrl->setIconSize(QSize(20, 20));

        horizontalLayout_11->addWidget(btnOpenUrl, 0, Qt::AlignmentFlag::AlignRight);


        verticalLayout->addWidget(headerFrame);

        infoFrame = new QFrame(mainFrame);
        infoFrame->setObjectName("infoFrame");
        infoFrame->setMinimumSize(QSize(0, 310));
        infoFrame->setMaximumSize(QSize(16777215, 310));
        infoFrame->setStyleSheet(QString::fromUtf8("QFrame {\n"
"          background-color: transparent;\n"
"          border: none;\n"
"          }"));
        infoFrame->setFrameShape(QFrame::Shape::StyledPanel);
        infoFrame->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_14 = new QHBoxLayout(infoFrame);
        horizontalLayout_14->setSpacing(0);
        horizontalLayout_14->setObjectName("horizontalLayout_14");
        horizontalLayout_14->setContentsMargins(50, 0, 0, 0);
        labelCover = new QLabel(infoFrame);
        labelCover->setObjectName("labelCover");
        labelCover->setMinimumSize(QSize(220, 310));
        labelCover->setMaximumSize(QSize(220, 310));
        labelCover->setStyleSheet(QString::fromUtf8("border-radius: 15px;"));
        labelCover->setAlignment(Qt::AlignmentFlag::AlignCenter);

        horizontalLayout_14->addWidget(labelCover);

        infoTextFrame = new QFrame(infoFrame);
        infoTextFrame->setObjectName("infoTextFrame");
        verticalLayout_5 = new QVBoxLayout(infoTextFrame);
        verticalLayout_5->setSpacing(0);
        verticalLayout_5->setObjectName("verticalLayout_5");
        verticalLayout_5->setContentsMargins(10, 0, 0, 0);
        lineEditTitle = new QLineEdit(infoTextFrame);
        lineEditTitle->setObjectName("lineEditTitle");
        QFont font;
        font.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font.setPointSize(20);
        font.setBold(true);
        lineEditTitle->setFont(font);
        lineEditTitle->setStyleSheet(QString::fromUtf8("border:none;\n"
"                background-color: transparent;"));
        lineEditTitle->setReadOnly(true);

        verticalLayout_5->addWidget(lineEditTitle);

        statsFrame = new QFrame(infoTextFrame);
        statsFrame->setObjectName("statsFrame");
        statsFrame->setFrameShape(QFrame::Shape::StyledPanel);
        statsFrame->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_2 = new QHBoxLayout(statsFrame);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 150, 0);
        infoLabelsFrame = new QFrame(statsFrame);
        infoLabelsFrame->setObjectName("infoLabelsFrame");
        infoLabelsFrame->setFrameShape(QFrame::Shape::StyledPanel);
        infoLabelsFrame->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayout_4 = new QVBoxLayout(infoLabelsFrame);
        verticalLayout_4->setSpacing(13);
        verticalLayout_4->setObjectName("verticalLayout_4");
        verticalLayout_4->setContentsMargins(0, -1, -1, 0);
        labelStatus = new QLabel(infoLabelsFrame);
        labelStatus->setObjectName("labelStatus");
        labelStatus->setMinimumSize(QSize(0, 35));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font1.setPointSize(15);
        font1.setBold(false);
        labelStatus->setFont(font1);
        labelStatus->setStyleSheet(QString::fromUtf8("border: 1px solid gray;\n"
"                      border-radius:8px;\n"
"                      padding: 2px 10px;"));
        labelStatus->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_4->addWidget(labelStatus, 0, Qt::AlignmentFlag::AlignLeft);

        labelProgress = new QLabel(infoLabelsFrame);
        labelProgress->setObjectName("labelProgress");
        QFont font2;
        font2.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font2.setPointSize(15);
        labelProgress->setFont(font2);
        labelProgress->setStyleSheet(QString::fromUtf8("border: none;"));

        verticalLayout_4->addWidget(labelProgress, 0, Qt::AlignmentFlag::AlignLeft);

        ratingInfoFrame = new QFrame(infoLabelsFrame);
        ratingInfoFrame->setObjectName("ratingInfoFrame");
        ratingInfoFrame->setFrameShape(QFrame::Shape::StyledPanel);
        ratingInfoFrame->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout = new QHBoxLayout(ratingInfoFrame);
        horizontalLayout->setSpacing(20);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        labelRatingInfo = new QLabel(ratingInfoFrame);
        labelRatingInfo->setObjectName("labelRatingInfo");
        labelRatingInfo->setFont(font2);
        labelRatingInfo->setStyleSheet(QString::fromUtf8("border: none;"));

        horizontalLayout->addWidget(labelRatingInfo);

        btnRating = new QPushButton(ratingInfoFrame);
        btnRating->setObjectName("btnRating");
        btnRating->setMinimumSize(QSize(80, 35));
        btnRating->setFont(font2);

        horizontalLayout->addWidget(btnRating);


        verticalLayout_4->addWidget(ratingInfoFrame, 0, Qt::AlignmentFlag::AlignLeft);

        labelCollectionStats = new QLabel(infoLabelsFrame);
        labelCollectionStats->setObjectName("labelCollectionStats");
        labelCollectionStats->setFont(font2);
        labelCollectionStats->setStyleSheet(QString::fromUtf8("border: none;"));

        verticalLayout_4->addWidget(labelCollectionStats, 0, Qt::AlignmentFlag::AlignLeft);

        btnStatus = new QPushButton(infoLabelsFrame);
        btnStatus->setObjectName("btnStatus");
        btnStatus->setMinimumSize(QSize(80, 35));
        btnStatus->setFont(font2);

        verticalLayout_4->addWidget(btnStatus, 0, Qt::AlignmentFlag::AlignLeft);

        btnAction = new QPushButton(infoLabelsFrame);
        btnAction->setObjectName("btnAction");
        btnAction->setMinimumSize(QSize(80, 35));
        QFont font3;
        font3.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font3.setPointSize(14);
        btnAction->setFont(font3);

        verticalLayout_4->addWidget(btnAction, 0, Qt::AlignmentFlag::AlignLeft);


        horizontalLayout_2->addWidget(infoLabelsFrame);


        verticalLayout_5->addWidget(statsFrame);


        horizontalLayout_14->addWidget(infoTextFrame);


        verticalLayout->addWidget(infoFrame);

        tabWidget = new QTabWidget(mainFrame);
        tabWidget->setObjectName("tabWidget");
        tabWidget->setMinimumSize(QSize(0, 100));
        QFont font4;
        font4.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font4.setPointSize(12);
        font4.setBold(false);
        tabWidget->setFont(font4);
        tabWidget->setStyleSheet(QString::fromUtf8("QTabWidget::tab-bar {\n"
"          alignment: center;\n"
"          }\n"
"\n"
"          QTabWidget {\n"
"          border:none;\n"
"          }"));
        tabDetail = new QWidget();
        tabDetail->setObjectName("tabDetail");
        verticalLayout_14 = new QVBoxLayout(tabDetail);
        verticalLayout_14->setSpacing(10);
        verticalLayout_14->setObjectName("verticalLayout_14");
        verticalLayout_14->setContentsMargins(50, 20, 20, 0);
        frameTags = new QFrame(tabDetail);
        frameTags->setObjectName("frameTags");
        frameTags->setStyleSheet(QString::fromUtf8("border: none;"));
        frameTags->setFrameShape(QFrame::Shape::StyledPanel);
        frameTags->setFrameShadow(QFrame::Shadow::Raised);

        verticalLayout_14->addWidget(frameTags, 0, Qt::AlignmentFlag::AlignTop);

        textEditSummary = new QTextEdit(tabDetail);
        textEditSummary->setObjectName("textEditSummary");
        QFont font5;
        font5.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font5.setPointSize(10);
        font5.setBold(true);
        textEditSummary->setFont(font5);
        textEditSummary->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
        textEditSummary->setStyleSheet(QString::fromUtf8("QScrollBar:vertical {\n"
"              border: none;\n"
"              background: #f0f0f0;\n"
"              width: 11px;\n"
"              }\n"
"\n"
"              QScrollBar::handle:vertical {\n"
"              background: #c0c0c0;\n"
"              border-radius: 5px;\n"
"              min-height: 20px;\n"
"              }\n"
"\n"
"              QScrollBar::handle:vertical:hover {\n"
"              background: #a0a0a0;\n"
"              }\n"
"\n"
"              QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {\n"
"              border: none;\n"
"              background: none;\n"
"              height: 0px;\n"
"              }\n"
"\n"
"             "));
        textEditSummary->setFrameShape(QFrame::Shape::NoFrame);
        textEditSummary->setReadOnly(true);

        verticalLayout_14->addWidget(textEditSummary);

        tabWidget->addTab(tabDetail, QString());
        tabCharacters = new QWidget();
        tabCharacters->setObjectName("tabCharacters");
        verticalLayout_2 = new QVBoxLayout(tabCharacters);
        verticalLayout_2->setSpacing(10);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(50, 20, 20, 0);
        scrollAreaCharacters = new QScrollArea(tabCharacters);
        scrollAreaCharacters->setObjectName("scrollAreaCharacters");
        scrollAreaCharacters->setStyleSheet(QString::fromUtf8("QScrollArea {\n"
"              border: none;\n"
"              }\n"
"\n"
"              QScrollBar:vertical {\n"
"              border: none;\n"
"              background: #f0f0f0;\n"
"              width: 11px;\n"
"              }\n"
"\n"
"              QScrollBar::handle:vertical {\n"
"              background: #c0c0c0;\n"
"              border-radius: 5px;\n"
"              min-height: 20px;\n"
"              }\n"
"\n"
"              QScrollBar::handle:vertical:hover {\n"
"              background: #a0a0a0;\n"
"              }\n"
"\n"
"              QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {\n"
"              border: none;\n"
"              background: none;\n"
"              height: 0px;\n"
"              }\n"
"\n"
"             "));
        scrollAreaCharacters->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOn);
        scrollAreaCharacters->setWidgetResizable(true);
        charactersContent = new QWidget();
        charactersContent->setObjectName("charactersContent");
        charactersContent->setGeometry(QRect(0, 0, 89, 30));
        charactersContent->setStyleSheet(QString::fromUtf8("background-color: transparent;"));
        scrollAreaCharacters->setWidget(charactersContent);

        verticalLayout_2->addWidget(scrollAreaCharacters);

        tabWidget->addTab(tabCharacters, QString());
        tabRelations = new QWidget();
        tabRelations->setObjectName("tabRelations");
        verticalLayout_6 = new QVBoxLayout(tabRelations);
        verticalLayout_6->setSpacing(10);
        verticalLayout_6->setObjectName("verticalLayout_6");
        verticalLayout_6->setContentsMargins(50, 20, 20, 0);
        scrollAreaRelations = new QScrollArea(tabRelations);
        scrollAreaRelations->setObjectName("scrollAreaRelations");
        scrollAreaRelations->setStyleSheet(QString::fromUtf8("QScrollArea {\n"
"              border: none;\n"
"              }\n"
"\n"
"              QScrollBar:vertical {\n"
"              border: none;\n"
"              background: #f0f0f0;\n"
"              width: 11px;\n"
"              }\n"
"\n"
"              QScrollBar::handle:vertical {\n"
"              background: #c0c0c0;\n"
"              border-radius: 5px;\n"
"              min-height: 20px;\n"
"              }\n"
"\n"
"              QScrollBar::handle:vertical:hover {\n"
"              background: #a0a0a0;\n"
"              }\n"
"\n"
"              QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {\n"
"              border: none;\n"
"              background: none;\n"
"              height: 0px;\n"
"              }\n"
"\n"
"             "));
        scrollAreaRelations->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOn);
        scrollAreaRelations->setWidgetResizable(true);
        relationsContent = new QWidget();
        relationsContent->setObjectName("relationsContent");
        relationsContent->setGeometry(QRect(0, 0, 1440, 534));
        relationsContent->setStyleSheet(QString::fromUtf8("background-color: transparent;"));
        scrollAreaRelations->setWidget(relationsContent);

        verticalLayout_6->addWidget(scrollAreaRelations);

        tabWidget->addTab(tabRelations, QString());
        tabStaff = new QWidget();
        tabStaff->setObjectName("tabStaff");
        verticalLayout_3 = new QVBoxLayout(tabStaff);
        verticalLayout_3->setSpacing(10);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(50, 20, 20, 0);
        scrollAreaStaff = new QScrollArea(tabStaff);
        scrollAreaStaff->setObjectName("scrollAreaStaff");
        scrollAreaStaff->setStyleSheet(QString::fromUtf8("QScrollArea {\n"
"              border: none;\n"
"              }\n"
"\n"
"              QScrollBar:vertical {\n"
"              border: none;\n"
"              background: #f0f0f0;\n"
"              width: 11px;\n"
"              }\n"
"\n"
"              QScrollBar::handle:vertical {\n"
"              background: #c0c0c0;\n"
"              border-radius: 5px;\n"
"              min-height: 20px;\n"
"              }\n"
"\n"
"              QScrollBar::handle:vertical:hover {\n"
"              background: #a0a0a0;\n"
"              }\n"
"\n"
"              QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {\n"
"              border: none;\n"
"              background: none;\n"
"              height: 0px;\n"
"              }\n"
"\n"
"             "));
        scrollAreaStaff->setWidgetResizable(true);
        staffContent = new QWidget();
        staffContent->setObjectName("staffContent");
        staffContent->setGeometry(QRect(0, 0, 100, 30));
        staffContent->setStyleSheet(QString::fromUtf8("background-color: transparent;"));
        scrollAreaStaff->setWidget(staffContent);

        verticalLayout_3->addWidget(scrollAreaStaff);

        tabWidget->addTab(tabStaff, QString());

        verticalLayout->addWidget(tabWidget);


        verticalLayout_11->addWidget(mainFrame);


        retranslateUi(DetailPage);

        tabWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(DetailPage);
    } // setupUi

    void retranslateUi(QWidget *DetailPage) const
    {
        btnBack->setText(QString());
        btnOpenUrl->setText(QString());
        labelCover->setText(QString());
        labelStatus->setText(QCoreApplication::translate("DetailPage", "TBA", nullptr));
        labelProgress->setText(QCoreApplication::translate("DetailPage", "\345\205\250-\350\257\235", nullptr));
        labelRatingInfo->setText(QCoreApplication::translate("DetailPage", "|\344\272\272\350\257\204|#", nullptr));
        labelCollectionStats->setText(QCoreApplication::translate("DetailPage", "\346\224\266\350\227\217/\345\234\250\347\234\213/\346\212\233\345\274\203", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabDetail), QCoreApplication::translate("DetailPage", "\350\257\246\346\203\205", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabCharacters), QCoreApplication::translate("DetailPage", "\350\247\222\350\211\262", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabRelations), QCoreApplication::translate("DetailPage", "\347\233\270\345\205\263\346\235\241\347\233\256", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabStaff), QCoreApplication::translate("DetailPage", "\345\210\266\344\275\234\344\272\272\345\221\230", nullptr));
        (void)DetailPage;
    } // retranslateUi

};

namespace Ui {
    class DetailPage: public Ui_DetailPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // DETAIL_UI_H
