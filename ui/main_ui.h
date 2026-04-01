/********************************************************************************
** Form generated from reading UI file 'main_ui.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef MAIN_UI_H
#define MAIN_UI_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QFrame *titlebar_frame;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *spacerTitleLeft;
    QPushButton *btnMinimize;
    QPushButton *btnMaximize;
    QPushButton *btnClose;
    QStackedWidget *stackedMainWindow;
    QWidget *showmain_page;
    QHBoxLayout *mainContentLayout;
    QFrame *toolbar_frame;
    QVBoxLayout *toolbarLayout;
    QPushButton *btnSearch;
    QFrame *acgnselect_frame;
    QVBoxLayout *categoryLayout;
    QPushButton *btnAnime;
    QPushButton *btnNovel;
    QPushButton *btnGame;
    QPushButton *btnComic;
    QPushButton *btnDownload;
    QSpacerItem *spacerToolbar;
    QPushButton *btnSettings;
    QStackedWidget *stackedMainContent;
    QWidget *pageMainContent;
    QVBoxLayout *mainPageLayout;
    QFrame *header_frame;
    QVBoxLayout *headerLayout;
    QFrame *titleFrame;
    QHBoxLayout *titleLayout;
    QLabel *labelCategoryTitle;
    QFrame *statusBarFrame;
    QHBoxLayout *statusBarLayout;
    QPushButton *statusBtnWish;
    QHBoxLayout *statusLayoutWish;
    QLabel *statusLabelWish;
    QLabel *statusCountWish;
    QPushButton *statusBtnWatched;
    QHBoxLayout *statusLayoutWatched;
    QLabel *statusLabelWatched;
    QLabel *statusCountWatched;
    QPushButton *statusBtnWatching;
    QHBoxLayout *statusLayoutWatching;
    QLabel *statusLabelWatching;
    QLabel *statusCountWatching;
    QPushButton *statusBtnOnHold;
    QHBoxLayout *statusLayoutOnHold;
    QLabel *statusLabelOnHold;
    QLabel *statusCountOnHold;
    QPushButton *statusBtnDropped;
    QHBoxLayout *statusLayoutDropped;
    QLabel *statusLabelDropped;
    QLabel *statusCountDropped;
    QPushButton *btnRefresh;
    QSpacerItem *spacerStatusRight;
    QPushButton *btnPlus;
    QLineEdit *lineEditSearch;
    QFrame *cardGridFrame;
    QGridLayout *gridLayout_2;
    QFrame *pageControlFrame;
    QHBoxLayout *pageControlLayout;
    QSpacerItem *spacerPageLeft;
    QPushButton *btnPrev;
    QLabel *labelPage;
    QPushButton *btnNext;
    QSpacerItem *spacerPageRight;
    QButtonGroup *buttonGroup;
    QButtonGroup *buttonGroup_2;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1525, 942);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        titlebar_frame = new QFrame(centralwidget);
        titlebar_frame->setObjectName("titlebar_frame");
        titlebar_frame->setMinimumSize(QSize(0, 25));
        titlebar_frame->setMaximumSize(QSize(16777215, 25));
        titlebar_frame->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);"));
        titlebar_frame->setFrameShape(QFrame::Shape::StyledPanel);
        titlebar_frame->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout = new QHBoxLayout(titlebar_frame);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        spacerTitleLeft = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(spacerTitleLeft);

        btnMinimize = new QPushButton(titlebar_frame);
        btnMinimize->setObjectName("btnMinimize");
        btnMinimize->setMinimumSize(QSize(45, 25));
        btnMinimize->setMaximumSize(QSize(45, 25));
        btnMinimize->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"           border: none\n"
"           }\n"
"           QPushButton:hover {\n"
"           background-color:rgb(229, 229, 229)\n"
"           }"));
        QIcon icon;
        icon.addFile(QString::fromUtf8("icons/title3.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        btnMinimize->setIcon(icon);

        horizontalLayout->addWidget(btnMinimize);

        btnMaximize = new QPushButton(titlebar_frame);
        btnMaximize->setObjectName("btnMaximize");
        btnMaximize->setMinimumSize(QSize(45, 25));
        btnMaximize->setMaximumSize(QSize(45, 25));
        btnMaximize->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"           border: none\n"
"           }\n"
"           QPushButton:hover {\n"
"           background-color:rgb(229, 229, 229)\n"
"           }"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8("icons/title2.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        btnMaximize->setIcon(icon1);
        btnMaximize->setIconSize(QSize(13, 13));

        horizontalLayout->addWidget(btnMaximize, 0, Qt::AlignmentFlag::AlignRight);

        btnClose = new QPushButton(titlebar_frame);
        btnClose->setObjectName("btnClose");
        btnClose->setMinimumSize(QSize(45, 25));
        btnClose->setMaximumSize(QSize(45, 25));
        btnClose->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"           border: none\n"
"           }\n"
"           QPushButton:hover {\n"
"           background-color:rgb(232, 17, 35)\n"
"           }"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8("icons/title1.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        btnClose->setIcon(icon2);
        btnClose->setIconSize(QSize(14, 14));

        horizontalLayout->addWidget(btnClose, 0, Qt::AlignmentFlag::AlignRight);


        verticalLayout->addWidget(titlebar_frame);

        stackedMainWindow = new QStackedWidget(centralwidget);
        stackedMainWindow->setObjectName("stackedMainWindow");
        stackedMainWindow->setStyleSheet(QString::fromUtf8(""));
        showmain_page = new QWidget();
        showmain_page->setObjectName("showmain_page");
        mainContentLayout = new QHBoxLayout(showmain_page);
        mainContentLayout->setSpacing(0);
        mainContentLayout->setObjectName("mainContentLayout");
        mainContentLayout->setContentsMargins(0, 0, 0, 0);
        toolbar_frame = new QFrame(showmain_page);
        toolbar_frame->setObjectName("toolbar_frame");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(toolbar_frame->sizePolicy().hasHeightForWidth());
        toolbar_frame->setSizePolicy(sizePolicy);
        toolbar_frame->setMinimumSize(QSize(80, 0));
        toolbar_frame->setMaximumSize(QSize(80, 16777215));
        toolbar_frame->setFrameShape(QFrame::Shape::NoFrame);
        toolbarLayout = new QVBoxLayout(toolbar_frame);
        toolbarLayout->setSpacing(0);
        toolbarLayout->setObjectName("toolbarLayout");
        toolbarLayout->setContentsMargins(0, 40, 0, -1);
        btnSearch = new QPushButton(toolbar_frame);
        btnSearch->setObjectName("btnSearch");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(btnSearch->sizePolicy().hasHeightForWidth());
        btnSearch->setSizePolicy(sizePolicy1);
        btnSearch->setMinimumSize(QSize(50, 50));
        btnSearch->setMaximumSize(QSize(50, 50));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8("icons/search.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        btnSearch->setIcon(icon3);
        btnSearch->setIconSize(QSize(20, 20));
        btnSearch->setCheckable(true);

        toolbarLayout->addWidget(btnSearch, 0, Qt::AlignmentFlag::AlignHCenter);

        acgnselect_frame = new QFrame(toolbar_frame);
        acgnselect_frame->setObjectName("acgnselect_frame");
        acgnselect_frame->setMinimumSize(QSize(80, 0));
        acgnselect_frame->setFrameShape(QFrame::Shape::NoFrame);
        categoryLayout = new QVBoxLayout(acgnselect_frame);
        categoryLayout->setSpacing(40);
        categoryLayout->setObjectName("categoryLayout");
        categoryLayout->setContentsMargins(-1, 15, -1, -1);
        btnAnime = new QPushButton(acgnselect_frame);
        buttonGroup = new QButtonGroup(MainWindow);
        buttonGroup->setObjectName("buttonGroup");
        buttonGroup->addButton(btnAnime);
        btnAnime->setObjectName("btnAnime");
        btnAnime->setMinimumSize(QSize(0, 50));
        btnAnime->setMaximumSize(QSize(80, 50));
        QFont font;
        font.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font.setPointSize(15);
        font.setBold(true);
        btnAnime->setFont(font);
        btnAnime->setCheckable(true);
        btnAnime->setChecked(true);

        categoryLayout->addWidget(btnAnime);

        btnNovel = new QPushButton(acgnselect_frame);
        buttonGroup->addButton(btnNovel);
        btnNovel->setObjectName("btnNovel");
        btnNovel->setMinimumSize(QSize(0, 50));
        btnNovel->setMaximumSize(QSize(80, 50));
        btnNovel->setFont(font);
        btnNovel->setCheckable(true);

        categoryLayout->addWidget(btnNovel);

        btnGame = new QPushButton(acgnselect_frame);
        buttonGroup->addButton(btnGame);
        btnGame->setObjectName("btnGame");
        btnGame->setMinimumSize(QSize(0, 50));
        btnGame->setMaximumSize(QSize(80, 50));
        btnGame->setFont(font);
        btnGame->setCheckable(true);

        categoryLayout->addWidget(btnGame);

        btnComic = new QPushButton(acgnselect_frame);
        buttonGroup->addButton(btnComic);
        btnComic->setObjectName("btnComic");
        btnComic->setMinimumSize(QSize(0, 50));
        btnComic->setMaximumSize(QSize(80, 50));
        btnComic->setFont(font);
        btnComic->setCheckable(true);

        categoryLayout->addWidget(btnComic);

        btnDownload = new QPushButton(acgnselect_frame);
        buttonGroup->addButton(btnDownload);
        btnDownload->setObjectName("btnDownload");
        btnDownload->setMinimumSize(QSize(0, 50));
        btnDownload->setMaximumSize(QSize(80, 50));
        btnDownload->setFont(font);
        btnDownload->setCheckable(true);

        categoryLayout->addWidget(btnDownload);


        toolbarLayout->addWidget(acgnselect_frame);

        spacerToolbar = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        toolbarLayout->addItem(spacerToolbar);

        btnSettings = new QPushButton(toolbar_frame);
        btnSettings->setObjectName("btnSettings");
        btnSettings->setMinimumSize(QSize(50, 50));
        btnSettings->setMaximumSize(QSize(50, 50));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8("icons/settings.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        btnSettings->setIcon(icon4);
        btnSettings->setCheckable(false);

        toolbarLayout->addWidget(btnSettings, 0, Qt::AlignmentFlag::AlignHCenter);


        mainContentLayout->addWidget(toolbar_frame);

        stackedMainContent = new QStackedWidget(showmain_page);
        stackedMainContent->setObjectName("stackedMainContent");
        pageMainContent = new QWidget();
        pageMainContent->setObjectName("pageMainContent");
        mainPageLayout = new QVBoxLayout(pageMainContent);
        mainPageLayout->setSpacing(0);
        mainPageLayout->setObjectName("mainPageLayout");
        mainPageLayout->setContentsMargins(0, 0, 0, 0);
        header_frame = new QFrame(pageMainContent);
        header_frame->setObjectName("header_frame");
        header_frame->setMinimumSize(QSize(0, 100));
        header_frame->setMaximumSize(QSize(16777215, 100));
        header_frame->setStyleSheet(QString::fromUtf8("background-color: white;"));
        header_frame->setFrameShape(QFrame::Shape::NoFrame);
        headerLayout = new QVBoxLayout(header_frame);
        headerLayout->setSpacing(0);
        headerLayout->setObjectName("headerLayout");
        headerLayout->setContentsMargins(-1, 0, -1, 0);
        titleFrame = new QFrame(header_frame);
        titleFrame->setObjectName("titleFrame");
        titleFrame->setFrameShape(QFrame::Shape::StyledPanel);
        titleFrame->setFrameShadow(QFrame::Shadow::Raised);
        titleLayout = new QHBoxLayout(titleFrame);
        titleLayout->setSpacing(0);
        titleLayout->setObjectName("titleLayout");
        titleLayout->setContentsMargins(-1, 0, -1, 0);
        labelCategoryTitle = new QLabel(titleFrame);
        labelCategoryTitle->setObjectName("labelCategoryTitle");
        labelCategoryTitle->setMinimumSize(QSize(105, 35));
        labelCategoryTitle->setMaximumSize(QSize(16777215, 25));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font1.setPointSize(25);
        font1.setBold(true);
        labelCategoryTitle->setFont(font1);
        labelCategoryTitle->setStyleSheet(QString::fromUtf8("border: none;"));

        titleLayout->addWidget(labelCategoryTitle, 0, Qt::AlignmentFlag::AlignLeft);


        headerLayout->addWidget(titleFrame);

        statusBarFrame = new QFrame(header_frame);
        statusBarFrame->setObjectName("statusBarFrame");
        statusBarFrame->setFrameShape(QFrame::Shape::StyledPanel);
        statusBarFrame->setFrameShadow(QFrame::Shadow::Raised);
        statusBarLayout = new QHBoxLayout(statusBarFrame);
        statusBarLayout->setSpacing(20);
        statusBarLayout->setObjectName("statusBarLayout");
        statusBarLayout->setContentsMargins(-1, 0, -1, 0);
        statusBtnWish = new QPushButton(statusBarFrame);
        buttonGroup_2 = new QButtonGroup(MainWindow);
        buttonGroup_2->setObjectName("buttonGroup_2");
        buttonGroup_2->addButton(statusBtnWish);
        statusBtnWish->setObjectName("statusBtnWish");
        statusBtnWish->setMinimumSize(QSize(90, 50));
        statusBtnWish->setMaximumSize(QSize(90, 50));
        statusBtnWish->setCheckable(true);
        statusLayoutWish = new QHBoxLayout(statusBtnWish);
        statusLayoutWish->setSpacing(0);
        statusLayoutWish->setObjectName("statusLayoutWish");
        statusLayoutWish->setContentsMargins(5, 0, 5, 0);
        statusLabelWish = new QLabel(statusBtnWish);
        statusLabelWish->setObjectName("statusLabelWish");
        statusLabelWish->setFont(font);
        statusLabelWish->setStyleSheet(QString::fromUtf8("border:none; background-color: transparent"));

        statusLayoutWish->addWidget(statusLabelWish);

        statusCountWish = new QLabel(statusBtnWish);
        statusCountWish->setObjectName("statusCountWish");
        statusCountWish->setMinimumSize(QSize(30, 30));
        statusCountWish->setMaximumSize(QSize(30, 30));
        QFont font2;
        font2.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        statusCountWish->setFont(font2);
        statusCountWish->setStyleSheet(QString::fromUtf8("border:none; background-color: transparent"));
        statusCountWish->setAlignment(Qt::AlignmentFlag::AlignCenter);

        statusLayoutWish->addWidget(statusCountWish);


        statusBarLayout->addWidget(statusBtnWish);

        statusBtnWatched = new QPushButton(statusBarFrame);
        buttonGroup_2->addButton(statusBtnWatched);
        statusBtnWatched->setObjectName("statusBtnWatched");
        statusBtnWatched->setMinimumSize(QSize(90, 50));
        statusBtnWatched->setMaximumSize(QSize(90, 50));
        statusBtnWatched->setCheckable(true);
        statusBtnWatched->setChecked(true);
        statusLayoutWatched = new QHBoxLayout(statusBtnWatched);
        statusLayoutWatched->setSpacing(0);
        statusLayoutWatched->setObjectName("statusLayoutWatched");
        statusLayoutWatched->setContentsMargins(5, 0, 5, 0);
        statusLabelWatched = new QLabel(statusBtnWatched);
        statusLabelWatched->setObjectName("statusLabelWatched");
        statusLabelWatched->setFont(font);
        statusLabelWatched->setStyleSheet(QString::fromUtf8("border:none; background-color: transparent"));

        statusLayoutWatched->addWidget(statusLabelWatched);

        statusCountWatched = new QLabel(statusBtnWatched);
        statusCountWatched->setObjectName("statusCountWatched");
        statusCountWatched->setMinimumSize(QSize(30, 30));
        statusCountWatched->setMaximumSize(QSize(30, 30));
        statusCountWatched->setFont(font2);
        statusCountWatched->setStyleSheet(QString::fromUtf8("border:none; background-color: transparent"));
        statusCountWatched->setAlignment(Qt::AlignmentFlag::AlignCenter);

        statusLayoutWatched->addWidget(statusCountWatched);


        statusBarLayout->addWidget(statusBtnWatched);

        statusBtnWatching = new QPushButton(statusBarFrame);
        buttonGroup_2->addButton(statusBtnWatching);
        statusBtnWatching->setObjectName("statusBtnWatching");
        statusBtnWatching->setMinimumSize(QSize(90, 50));
        statusBtnWatching->setMaximumSize(QSize(90, 50));
        statusBtnWatching->setCheckable(true);
        statusLayoutWatching = new QHBoxLayout(statusBtnWatching);
        statusLayoutWatching->setSpacing(0);
        statusLayoutWatching->setObjectName("statusLayoutWatching");
        statusLayoutWatching->setContentsMargins(5, 0, 5, 0);
        statusLabelWatching = new QLabel(statusBtnWatching);
        statusLabelWatching->setObjectName("statusLabelWatching");
        statusLabelWatching->setFont(font);
        statusLabelWatching->setStyleSheet(QString::fromUtf8("border:none; background-color: transparent"));

        statusLayoutWatching->addWidget(statusLabelWatching);

        statusCountWatching = new QLabel(statusBtnWatching);
        statusCountWatching->setObjectName("statusCountWatching");
        statusCountWatching->setMinimumSize(QSize(30, 30));
        statusCountWatching->setMaximumSize(QSize(30, 30));
        statusCountWatching->setFont(font2);
        statusCountWatching->setStyleSheet(QString::fromUtf8("border:none; background-color: transparent"));
        statusCountWatching->setAlignment(Qt::AlignmentFlag::AlignCenter);

        statusLayoutWatching->addWidget(statusCountWatching);


        statusBarLayout->addWidget(statusBtnWatching);

        statusBtnOnHold = new QPushButton(statusBarFrame);
        buttonGroup_2->addButton(statusBtnOnHold);
        statusBtnOnHold->setObjectName("statusBtnOnHold");
        statusBtnOnHold->setMinimumSize(QSize(90, 50));
        statusBtnOnHold->setMaximumSize(QSize(90, 50));
        statusBtnOnHold->setCheckable(true);
        statusLayoutOnHold = new QHBoxLayout(statusBtnOnHold);
        statusLayoutOnHold->setSpacing(0);
        statusLayoutOnHold->setObjectName("statusLayoutOnHold");
        statusLayoutOnHold->setContentsMargins(5, 0, 5, 0);
        statusLabelOnHold = new QLabel(statusBtnOnHold);
        statusLabelOnHold->setObjectName("statusLabelOnHold");
        statusLabelOnHold->setFont(font);
        statusLabelOnHold->setStyleSheet(QString::fromUtf8("border:none; background-color: transparent"));

        statusLayoutOnHold->addWidget(statusLabelOnHold);

        statusCountOnHold = new QLabel(statusBtnOnHold);
        statusCountOnHold->setObjectName("statusCountOnHold");
        statusCountOnHold->setMinimumSize(QSize(30, 30));
        statusCountOnHold->setMaximumSize(QSize(30, 30));
        statusCountOnHold->setFont(font2);
        statusCountOnHold->setStyleSheet(QString::fromUtf8("border:none; background-color: transparent"));
        statusCountOnHold->setAlignment(Qt::AlignmentFlag::AlignCenter);

        statusLayoutOnHold->addWidget(statusCountOnHold);


        statusBarLayout->addWidget(statusBtnOnHold);

        statusBtnDropped = new QPushButton(statusBarFrame);
        buttonGroup_2->addButton(statusBtnDropped);
        statusBtnDropped->setObjectName("statusBtnDropped");
        statusBtnDropped->setMinimumSize(QSize(90, 50));
        statusBtnDropped->setMaximumSize(QSize(90, 50));
        statusBtnDropped->setCheckable(true);
        statusLayoutDropped = new QHBoxLayout(statusBtnDropped);
        statusLayoutDropped->setSpacing(0);
        statusLayoutDropped->setObjectName("statusLayoutDropped");
        statusLayoutDropped->setContentsMargins(5, 0, 5, 0);
        statusLabelDropped = new QLabel(statusBtnDropped);
        statusLabelDropped->setObjectName("statusLabelDropped");
        statusLabelDropped->setFont(font);
        statusLabelDropped->setStyleSheet(QString::fromUtf8("border:none; background-color: transparent"));

        statusLayoutDropped->addWidget(statusLabelDropped);

        statusCountDropped = new QLabel(statusBtnDropped);
        statusCountDropped->setObjectName("statusCountDropped");
        statusCountDropped->setMinimumSize(QSize(30, 30));
        statusCountDropped->setMaximumSize(QSize(30, 30));
        statusCountDropped->setFont(font2);
        statusCountDropped->setStyleSheet(QString::fromUtf8("border:none; background-color: transparent"));
        statusCountDropped->setAlignment(Qt::AlignmentFlag::AlignCenter);

        statusLayoutDropped->addWidget(statusCountDropped);


        statusBarLayout->addWidget(statusBtnDropped);

        btnRefresh = new QPushButton(statusBarFrame);
        btnRefresh->setObjectName("btnRefresh");
        btnRefresh->setMinimumSize(QSize(50, 50));
        btnRefresh->setMaximumSize(QSize(50, 50));
        btnRefresh->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"                      border-bottom-color: rgb(24, 59, 255);\n"
"                      background-color: white;\n"
"                      border:none;\n"
"                      border-radius:25px\n"
"                      }\n"
"\n"
"                      QPushButton:hover {\n"
"                      background-color: rgb(237, 237, 238);\n"
"                      }\n"
"\n"
"                      QPushButton:checked:hover {\n"
"                      background-color: rgb(237, 237, 238);\n"
"                      }"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8("icons/refresh.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        btnRefresh->setIcon(icon5);
        btnRefresh->setIconSize(QSize(20, 20));

        statusBarLayout->addWidget(btnRefresh);

        spacerStatusRight = new QSpacerItem(774, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        statusBarLayout->addItem(spacerStatusRight);

        btnPlus = new QPushButton(statusBarFrame);
        btnPlus->setObjectName("btnPlus");
        btnPlus->setMinimumSize(QSize(50, 50));
        btnPlus->setMaximumSize(QSize(50, 50));
        btnPlus->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"                      border-bottom-color: rgb(24, 59, 255);\n"
"                      background-color: white;\n"
"                      border:none;\n"
"                      border-radius:25px\n"
"                      }\n"
"\n"
"                      QPushButton:hover {\n"
"                      background-color: rgb(237, 237, 238);\n"
"                      }\n"
"\n"
"                      QPushButton:checked:hover {\n"
"                      background-color: rgb(237, 237, 238);\n"
"                      }"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8("icons/plus.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        btnPlus->setIcon(icon6);
        btnPlus->setIconSize(QSize(20, 20));

        statusBarLayout->addWidget(btnPlus);

        lineEditSearch = new QLineEdit(statusBarFrame);
        lineEditSearch->setObjectName("lineEditSearch");
        lineEditSearch->setMinimumSize(QSize(200, 25));
        lineEditSearch->setMaximumSize(QSize(200, 25));
        lineEditSearch->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
        lineEditSearch->setStyleSheet(QString::fromUtf8(""));

        statusBarLayout->addWidget(lineEditSearch);


        headerLayout->addWidget(statusBarFrame);


        mainPageLayout->addWidget(header_frame);

        cardGridFrame = new QFrame(pageMainContent);
        cardGridFrame->setObjectName("cardGridFrame");
        cardGridFrame->setStyleSheet(QString::fromUtf8("background-color: white;"));
        cardGridFrame->setFrameShape(QFrame::Shape::NoFrame);
        gridLayout_2 = new QGridLayout(cardGridFrame);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setHorizontalSpacing(35);
        gridLayout_2->setVerticalSpacing(18);
        gridLayout_2->setContentsMargins(0, 10, 0, 10);

        mainPageLayout->addWidget(cardGridFrame);

        pageControlFrame = new QFrame(pageMainContent);
        pageControlFrame->setObjectName("pageControlFrame");
        pageControlFrame->setMinimumSize(QSize(0, 45));
        pageControlFrame->setMaximumSize(QSize(16777215, 45));
        pageControlFrame->setStyleSheet(QString::fromUtf8("QFrame {\n"
"                background-color: white\n"
"                }\n"
"\n"
"                QPushButton {\n"
"                border-width: 1px;\n"
"                border-style: solid;\n"
"                border-color: rgba(0,0,0,100);\n"
"                border-radius: 4px;\n"
"                }"));
        pageControlFrame->setFrameShape(QFrame::Shape::NoFrame);
        pageControlLayout = new QHBoxLayout(pageControlFrame);
        pageControlLayout->setObjectName("pageControlLayout");
        spacerPageLeft = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        pageControlLayout->addItem(spacerPageLeft);

        btnPrev = new QPushButton(pageControlFrame);
        btnPrev->setObjectName("btnPrev");
        btnPrev->setMinimumSize(QSize(75, 20));
        btnPrev->setMaximumSize(QSize(75, 20));
        btnPrev->setStyleSheet(QString::fromUtf8("QPushButton:pressed {\n"
"                   background-color: rgba(0,0,0,20)\n"
"                   }"));
        btnPrev->setCheckable(true);

        pageControlLayout->addWidget(btnPrev);

        labelPage = new QLabel(pageControlFrame);
        labelPage->setObjectName("labelPage");
        labelPage->setMinimumSize(QSize(75, 20));
        labelPage->setMaximumSize(QSize(75, 20));
        labelPage->setStyleSheet(QString::fromUtf8("border-width: 1px;\n"
"                   border-style: solid;\n"
"                   border-color: rgba(0,0,0,100);\n"
"                   border-radius: 4px;"));
        labelPage->setAlignment(Qt::AlignmentFlag::AlignCenter);

        pageControlLayout->addWidget(labelPage);

        btnNext = new QPushButton(pageControlFrame);
        btnNext->setObjectName("btnNext");
        btnNext->setMinimumSize(QSize(75, 20));
        btnNext->setMaximumSize(QSize(75, 20));
        btnNext->setStyleSheet(QString::fromUtf8("QPushButton:pressed {\n"
"                   background-color: rgba(0,0,0,20)\n"
"                   }"));

        pageControlLayout->addWidget(btnNext);

        spacerPageRight = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        pageControlLayout->addItem(spacerPageRight);


        mainPageLayout->addWidget(pageControlFrame);

        stackedMainContent->addWidget(pageMainContent);

        mainContentLayout->addWidget(stackedMainContent);

        stackedMainWindow->addWidget(showmain_page);

        verticalLayout->addWidget(stackedMainWindow);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        stackedMainWindow->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow) const
    {
        btnMinimize->setText(QString());
        btnMaximize->setText(QString());
        btnClose->setText(QString());
        btnAnime->setText(QCoreApplication::translate("MainWindow", "\345\212\250\347\224\273", nullptr));
        btnNovel->setText(QCoreApplication::translate("MainWindow", "\345\260\217\350\257\264", nullptr));
        btnGame->setText(QCoreApplication::translate("MainWindow", "\346\270\270\346\210\217", nullptr));
        btnComic->setText(QCoreApplication::translate("MainWindow", "\346\274\253\347\224\273", nullptr));
        btnDownload->setText(QCoreApplication::translate("MainWindow", "\344\270\213\350\275\275", nullptr));
        btnSettings->setText(QString());
        labelCategoryTitle->setText(QCoreApplication::translate("MainWindow", "\345\212\250\347\224\273", nullptr));
        statusCountWish->setText(QString());
        statusCountWatched->setText(QString());
        statusCountWatching->setText(QString());
        statusCountOnHold->setText(QString());
        statusCountDropped->setText(QString());
        btnRefresh->setText(QString());
        btnPlus->setText(QString());
        lineEditSearch->setPlaceholderText(QCoreApplication::translate("MainWindow", "\346\220\234\347\264\242\345\275\223\345\211\215\347\212\266\346\200\201", nullptr));
        btnPrev->setText(QCoreApplication::translate("MainWindow", "\344\270\212\344\270\200\351\241\265", nullptr));
        labelPage->setText(QString());
        btnNext->setText(QCoreApplication::translate("MainWindow", "\344\270\213\344\270\200\351\241\265", nullptr));
        (void)MainWindow;
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // MAIN_UI_H
