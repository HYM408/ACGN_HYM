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
    QSpacerItem *horizontalSpacer_5;
    QPushButton *pushButton;
    QPushButton *pushButton_8;
    QPushButton *pushButton_2;
    QStackedWidget *main_stackedWidget;
    QWidget *showmain_page;
    QHBoxLayout *horizontalLayout_6;
    QFrame *toolbar_frame;
    QVBoxLayout *verticalLayout_3;
    QPushButton *search_Button;
    QFrame *acgnselect_frame;
    QVBoxLayout *verticalLayout_4;
    QPushButton *animation_Button;
    QPushButton *novel_Button;
    QPushButton *game_Button;
    QPushButton *comic_Button;
    QPushButton *pushButton_9;
    QSpacerItem *verticalSpacer;
    QPushButton *settings_Button;
    QStackedWidget *showmain_stackedWidget;
    QWidget *showmain_stackedWidgetPage1;
    QVBoxLayout *verticalLayout_2;
    QFrame *header_frame;
    QVBoxLayout *verticalLayout_5;
    QFrame *frame;
    QHBoxLayout *horizontalLayout_4;
    QLabel *project_Button;
    QFrame *frame_2;
    QHBoxLayout *horizontalLayout_5;
    QFrame *frame_3;
    QHBoxLayout *horizontalLayout_7;
    QLabel *pushButton_3;
    QLabel *pushButton_10;
    QFrame *frame_5;
    QHBoxLayout *horizontalLayout_8;
    QLabel *pushButton_4;
    QLabel *pushButton_11;
    QFrame *frame_7;
    QHBoxLayout *horizontalLayout_9;
    QLabel *pushButton_5;
    QLabel *pushButton_12;
    QFrame *frame_9;
    QHBoxLayout *horizontalLayout_10;
    QLabel *pushButton_6;
    QLabel *pushButton_13;
    QFrame *frame_11;
    QHBoxLayout *horizontalLayout_2;
    QLabel *pushButton_7;
    QLabel *pushButton_14;
    QPushButton *refresh_Button;
    QSpacerItem *horizontalSpacer;
    QLineEdit *searchlist_lineEdit;
    QFrame *animationdata_frame;
    QGridLayout *gridLayout_2;
    QFrame *page_frame;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *previous_Button;
    QLabel *pages_Button;
    QPushButton *next_Button;
    QSpacerItem *horizontalSpacer_2;
    QButtonGroup *buttonGroup;

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
        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_5);

        pushButton = new QPushButton(titlebar_frame);
        pushButton->setObjectName("pushButton");
        pushButton->setMinimumSize(QSize(45, 25));
        pushButton->setMaximumSize(QSize(45, 25));
        pushButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"           border: none\n"
"           }\n"
"           QPushButton:hover {\n"
"           background-color:rgb(229, 229, 229)\n"
"           }"));
        QIcon icon;
        icon.addFile(QString::fromUtf8("icons/title3.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        pushButton->setIcon(icon);

        horizontalLayout->addWidget(pushButton);

        pushButton_8 = new QPushButton(titlebar_frame);
        pushButton_8->setObjectName("pushButton_8");
        pushButton_8->setMinimumSize(QSize(45, 25));
        pushButton_8->setMaximumSize(QSize(45, 25));
        pushButton_8->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"           border: none\n"
"           }\n"
"           QPushButton:hover {\n"
"           background-color:rgb(229, 229, 229)\n"
"           }"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8("icons/title2.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        pushButton_8->setIcon(icon1);
        pushButton_8->setIconSize(QSize(13, 13));

        horizontalLayout->addWidget(pushButton_8, 0, Qt::AlignmentFlag::AlignRight);

        pushButton_2 = new QPushButton(titlebar_frame);
        pushButton_2->setObjectName("pushButton_2");
        pushButton_2->setMinimumSize(QSize(45, 25));
        pushButton_2->setMaximumSize(QSize(45, 25));
        pushButton_2->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"           border: none\n"
"           }\n"
"           QPushButton:hover {\n"
"           background-color:rgb(232, 17, 35)\n"
"           }"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8("icons/title1.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        pushButton_2->setIcon(icon2);
        pushButton_2->setIconSize(QSize(14, 14));

        horizontalLayout->addWidget(pushButton_2, 0, Qt::AlignmentFlag::AlignRight);


        verticalLayout->addWidget(titlebar_frame);

        main_stackedWidget = new QStackedWidget(centralwidget);
        main_stackedWidget->setObjectName("main_stackedWidget");
        main_stackedWidget->setStyleSheet(QString::fromUtf8(""));
        showmain_page = new QWidget();
        showmain_page->setObjectName("showmain_page");
        horizontalLayout_6 = new QHBoxLayout(showmain_page);
        horizontalLayout_6->setSpacing(0);
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        horizontalLayout_6->setContentsMargins(0, 0, 0, 0);
        toolbar_frame = new QFrame(showmain_page);
        toolbar_frame->setObjectName("toolbar_frame");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(toolbar_frame->sizePolicy().hasHeightForWidth());
        toolbar_frame->setSizePolicy(sizePolicy);
        toolbar_frame->setMinimumSize(QSize(80, 0));
        toolbar_frame->setMaximumSize(QSize(80, 16777215));
        toolbar_frame->setStyleSheet(QString::fromUtf8("background-color: rgb(242, 236, 244);"));
        toolbar_frame->setFrameShape(QFrame::Shape::NoFrame);
        verticalLayout_3 = new QVBoxLayout(toolbar_frame);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(0, 40, 0, -1);
        search_Button = new QPushButton(toolbar_frame);
        search_Button->setObjectName("search_Button");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(search_Button->sizePolicy().hasHeightForWidth());
        search_Button->setSizePolicy(sizePolicy1);
        search_Button->setMinimumSize(QSize(50, 50));
        search_Button->setMaximumSize(QSize(50, 50));
        search_Button->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"               background-color: rgb(233, 221, 255);\n"
"               border: none;\n"
"               border-radius:15px\n"
"               }\n"
"\n"
"               QPushButton:hover {\n"
"               background-color: rgb(221, 208, 246);\n"
"               }"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8("icons/search.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        search_Button->setIcon(icon3);
        search_Button->setIconSize(QSize(20, 20));
        search_Button->setCheckable(true);

        verticalLayout_3->addWidget(search_Button, 0, Qt::AlignmentFlag::AlignHCenter);

        acgnselect_frame = new QFrame(toolbar_frame);
        acgnselect_frame->setObjectName("acgnselect_frame");
        acgnselect_frame->setMinimumSize(QSize(80, 0));
        acgnselect_frame->setStyleSheet(QString::fromUtf8("background-color: rgb(242, 236, 244);"));
        acgnselect_frame->setFrameShape(QFrame::Shape::NoFrame);
        verticalLayout_4 = new QVBoxLayout(acgnselect_frame);
        verticalLayout_4->setSpacing(40);
        verticalLayout_4->setObjectName("verticalLayout_4");
        verticalLayout_4->setContentsMargins(-1, 15, -1, -1);
        animation_Button = new QPushButton(acgnselect_frame);
        buttonGroup = new QButtonGroup(MainWindow);
        buttonGroup->setObjectName("buttonGroup");
        buttonGroup->addButton(animation_Button);
        animation_Button->setObjectName("animation_Button");
        animation_Button->setMinimumSize(QSize(0, 50));
        animation_Button->setMaximumSize(QSize(80, 50));
        QFont font;
        font.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font.setPointSize(15);
        font.setBold(true);
        animation_Button->setFont(font);
        animation_Button->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"                  background-color: rgb(242, 236, 244);\n"
"                  border: none;\n"
"                  border-radius:15px\n"
"                  }\n"
"\n"
"                  QPushButton:checked {\n"
"                  background-color: rgb(233, 221, 255);\n"
"                  }\n"
"\n"
"                  QPushButton:hover {\n"
"                  background-color: rgb(225, 219, 228);\n"
"                  }\n"
"\n"
"                  QPushButton:checked:hover {\n"
"                  background-color: rgb(216, 207, 232);\n"
"                  }"));
        animation_Button->setCheckable(true);

        verticalLayout_4->addWidget(animation_Button);

        novel_Button = new QPushButton(acgnselect_frame);
        buttonGroup->addButton(novel_Button);
        novel_Button->setObjectName("novel_Button");
        novel_Button->setMinimumSize(QSize(0, 50));
        novel_Button->setMaximumSize(QSize(80, 50));
        novel_Button->setFont(font);
        novel_Button->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"                  background-color: rgb(242, 236, 244);\n"
"                  border: none;\n"
"                  border-radius:15px\n"
"                  }\n"
"\n"
"                  QPushButton:checked {\n"
"                  background-color: rgb(233, 221, 255);\n"
"                  }\n"
"\n"
"                  QPushButton:hover {\n"
"                  background-color: rgb(225, 219, 228);\n"
"                  }\n"
"\n"
"                  QPushButton:checked:hover {\n"
"                  background-color: rgb(216, 207, 232);\n"
"                  }"));
        novel_Button->setCheckable(true);

        verticalLayout_4->addWidget(novel_Button);

        game_Button = new QPushButton(acgnselect_frame);
        buttonGroup->addButton(game_Button);
        game_Button->setObjectName("game_Button");
        game_Button->setMinimumSize(QSize(0, 50));
        game_Button->setMaximumSize(QSize(80, 50));
        game_Button->setFont(font);
        game_Button->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"                  background-color: rgb(242, 236, 244);\n"
"                  border: none;\n"
"                  border-radius:15px\n"
"                  }\n"
"\n"
"                  QPushButton:checked {\n"
"                  background-color: rgb(233, 221, 255);\n"
"                  }\n"
"\n"
"                  QPushButton:hover {\n"
"                  background-color: rgb(225, 219, 228);\n"
"                  }\n"
"\n"
"                  QPushButton:checked:hover {\n"
"                  background-color: rgb(216, 207, 232);\n"
"                  }"));
        game_Button->setCheckable(true);

        verticalLayout_4->addWidget(game_Button);

        comic_Button = new QPushButton(acgnselect_frame);
        buttonGroup->addButton(comic_Button);
        comic_Button->setObjectName("comic_Button");
        comic_Button->setMinimumSize(QSize(0, 50));
        comic_Button->setMaximumSize(QSize(80, 50));
        comic_Button->setFont(font);
        comic_Button->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"                  background-color: rgb(242, 236, 244);\n"
"                  border: none;\n"
"                  border-radius:15px\n"
"                  }\n"
"\n"
"                  QPushButton:checked {\n"
"                  background-color: rgb(233, 221, 255);\n"
"                  }\n"
"\n"
"                  QPushButton:hover {\n"
"                  background-color: rgb(225, 219, 228);\n"
"                  }\n"
"\n"
"                  QPushButton:checked:hover {\n"
"                  background-color: rgb(216, 207, 232);\n"
"                  }"));
        comic_Button->setCheckable(true);

        verticalLayout_4->addWidget(comic_Button);

        pushButton_9 = new QPushButton(acgnselect_frame);
        buttonGroup->addButton(pushButton_9);
        pushButton_9->setObjectName("pushButton_9");
        pushButton_9->setMinimumSize(QSize(0, 50));
        pushButton_9->setMaximumSize(QSize(80, 50));
        pushButton_9->setFont(font);
        pushButton_9->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"                  background-color: rgb(242, 236, 244);\n"
"                  border: none;\n"
"                  border-radius:15px\n"
"                  }\n"
"\n"
"                  QPushButton:checked {\n"
"                  background-color: rgb(233, 221, 255);\n"
"                  }\n"
"\n"
"                  QPushButton:hover {\n"
"                  background-color: rgb(225, 219, 228);\n"
"                  }\n"
"\n"
"                  QPushButton:checked:hover {\n"
"                  background-color: rgb(216, 207, 232);\n"
"                  }"));
        pushButton_9->setCheckable(true);

        verticalLayout_4->addWidget(pushButton_9);


        verticalLayout_3->addWidget(acgnselect_frame);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);

        settings_Button = new QPushButton(toolbar_frame);
        settings_Button->setObjectName("settings_Button");
        settings_Button->setMinimumSize(QSize(50, 50));
        settings_Button->setMaximumSize(QSize(50, 50));
        settings_Button->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"               background-color: rgb(242, 236, 244);\n"
"               border: none;\n"
"               border-radius:15px\n"
"               }\n"
"\n"
"               QPushButton:hover {\n"
"               background-color: rgb(225, 219, 228);\n"
"               }"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8("icons/settings.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        settings_Button->setIcon(icon4);
        settings_Button->setCheckable(false);

        verticalLayout_3->addWidget(settings_Button, 0, Qt::AlignmentFlag::AlignHCenter);


        horizontalLayout_6->addWidget(toolbar_frame);

        showmain_stackedWidget = new QStackedWidget(showmain_page);
        showmain_stackedWidget->setObjectName("showmain_stackedWidget");
        showmain_stackedWidgetPage1 = new QWidget();
        showmain_stackedWidgetPage1->setObjectName("showmain_stackedWidgetPage1");
        verticalLayout_2 = new QVBoxLayout(showmain_stackedWidgetPage1);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        header_frame = new QFrame(showmain_stackedWidgetPage1);
        header_frame->setObjectName("header_frame");
        header_frame->setMinimumSize(QSize(0, 100));
        header_frame->setMaximumSize(QSize(16777215, 100));
        header_frame->setStyleSheet(QString::fromUtf8("background-color: white;"));
        header_frame->setFrameShape(QFrame::Shape::NoFrame);
        verticalLayout_5 = new QVBoxLayout(header_frame);
        verticalLayout_5->setSpacing(0);
        verticalLayout_5->setObjectName("verticalLayout_5");
        verticalLayout_5->setContentsMargins(-1, 0, -1, 0);
        frame = new QFrame(header_frame);
        frame->setObjectName("frame");
        frame->setFrameShape(QFrame::Shape::StyledPanel);
        frame->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_4 = new QHBoxLayout(frame);
        horizontalLayout_4->setSpacing(0);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalLayout_4->setContentsMargins(-1, 0, -1, 0);
        project_Button = new QLabel(frame);
        project_Button->setObjectName("project_Button");
        project_Button->setMinimumSize(QSize(105, 35));
        project_Button->setMaximumSize(QSize(16777215, 25));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font1.setPointSize(25);
        font1.setBold(true);
        project_Button->setFont(font1);
        project_Button->setStyleSheet(QString::fromUtf8("border: none;"));

        horizontalLayout_4->addWidget(project_Button, 0, Qt::AlignmentFlag::AlignLeft);


        verticalLayout_5->addWidget(frame);

        frame_2 = new QFrame(header_frame);
        frame_2->setObjectName("frame_2");
        frame_2->setFrameShape(QFrame::Shape::StyledPanel);
        frame_2->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_5 = new QHBoxLayout(frame_2);
        horizontalLayout_5->setSpacing(20);
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        horizontalLayout_5->setContentsMargins(-1, 0, -1, 0);
        frame_3 = new QFrame(frame_2);
        frame_3->setObjectName("frame_3");
        frame_3->setMinimumSize(QSize(90, 0));
        frame_3->setMaximumSize(QSize(90, 16777215));
        frame_3->setFrameShape(QFrame::Shape::StyledPanel);
        frame_3->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_7 = new QHBoxLayout(frame_3);
        horizontalLayout_7->setSpacing(0);
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        horizontalLayout_7->setContentsMargins(0, 0, 0, 0);
        pushButton_3 = new QLabel(frame_3);
        pushButton_3->setObjectName("pushButton_3");
        pushButton_3->setFont(font);
        pushButton_3->setStyleSheet(QString::fromUtf8("border:none;\n"
"                         background-color: transparent"));

        horizontalLayout_7->addWidget(pushButton_3);

        pushButton_10 = new QLabel(frame_3);
        pushButton_10->setObjectName("pushButton_10");
        pushButton_10->setMinimumSize(QSize(30, 30));
        pushButton_10->setMaximumSize(QSize(30, 30));
        QFont font2;
        font2.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        pushButton_10->setFont(font2);
        pushButton_10->setStyleSheet(QString::fromUtf8("border:none;\n"
"                         border-radius:15px;\n"
"                         background-color: rgb(242, 236, 244)"));
        pushButton_10->setAlignment(Qt::AlignmentFlag::AlignCenter);

        horizontalLayout_7->addWidget(pushButton_10);


        horizontalLayout_5->addWidget(frame_3);

        frame_5 = new QFrame(frame_2);
        frame_5->setObjectName("frame_5");
        frame_5->setMinimumSize(QSize(90, 0));
        frame_5->setMaximumSize(QSize(90, 16777215));
        frame_5->setFrameShape(QFrame::Shape::StyledPanel);
        frame_5->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_8 = new QHBoxLayout(frame_5);
        horizontalLayout_8->setSpacing(0);
        horizontalLayout_8->setObjectName("horizontalLayout_8");
        horizontalLayout_8->setContentsMargins(0, 0, 0, 0);
        pushButton_4 = new QLabel(frame_5);
        pushButton_4->setObjectName("pushButton_4");
        pushButton_4->setFont(font);
        pushButton_4->setStyleSheet(QString::fromUtf8("border:none;\n"
"                         background-color: transparent"));

        horizontalLayout_8->addWidget(pushButton_4);

        pushButton_11 = new QLabel(frame_5);
        pushButton_11->setObjectName("pushButton_11");
        pushButton_11->setMinimumSize(QSize(30, 30));
        pushButton_11->setMaximumSize(QSize(30, 30));
        pushButton_11->setFont(font2);
        pushButton_11->setStyleSheet(QString::fromUtf8("border:none;\n"
"                         border-radius:15px;\n"
"                         background-color: rgb(242, 236, 244)"));
        pushButton_11->setAlignment(Qt::AlignmentFlag::AlignCenter);

        horizontalLayout_8->addWidget(pushButton_11);


        horizontalLayout_5->addWidget(frame_5);

        frame_7 = new QFrame(frame_2);
        frame_7->setObjectName("frame_7");
        frame_7->setMinimumSize(QSize(90, 0));
        frame_7->setMaximumSize(QSize(90, 16777215));
        frame_7->setFrameShape(QFrame::Shape::StyledPanel);
        frame_7->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_9 = new QHBoxLayout(frame_7);
        horizontalLayout_9->setSpacing(0);
        horizontalLayout_9->setObjectName("horizontalLayout_9");
        horizontalLayout_9->setContentsMargins(0, 0, 0, 0);
        pushButton_5 = new QLabel(frame_7);
        pushButton_5->setObjectName("pushButton_5");
        pushButton_5->setFont(font);
        pushButton_5->setStyleSheet(QString::fromUtf8("border:none;\n"
"                         background-color: transparent"));

        horizontalLayout_9->addWidget(pushButton_5);

        pushButton_12 = new QLabel(frame_7);
        pushButton_12->setObjectName("pushButton_12");
        pushButton_12->setMinimumSize(QSize(30, 30));
        pushButton_12->setMaximumSize(QSize(30, 30));
        pushButton_12->setFont(font2);
        pushButton_12->setStyleSheet(QString::fromUtf8("border:none;\n"
"                         border-radius:15px;\n"
"                         background-color: rgb(242, 236, 244)"));
        pushButton_12->setAlignment(Qt::AlignmentFlag::AlignCenter);

        horizontalLayout_9->addWidget(pushButton_12);


        horizontalLayout_5->addWidget(frame_7);

        frame_9 = new QFrame(frame_2);
        frame_9->setObjectName("frame_9");
        frame_9->setMinimumSize(QSize(90, 0));
        frame_9->setMaximumSize(QSize(90, 16777215));
        frame_9->setFrameShape(QFrame::Shape::StyledPanel);
        frame_9->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_10 = new QHBoxLayout(frame_9);
        horizontalLayout_10->setSpacing(0);
        horizontalLayout_10->setObjectName("horizontalLayout_10");
        horizontalLayout_10->setContentsMargins(0, 0, 0, 0);
        pushButton_6 = new QLabel(frame_9);
        pushButton_6->setObjectName("pushButton_6");
        pushButton_6->setFont(font);
        pushButton_6->setStyleSheet(QString::fromUtf8("border:none;\n"
"                         background-color: transparent"));

        horizontalLayout_10->addWidget(pushButton_6);

        pushButton_13 = new QLabel(frame_9);
        pushButton_13->setObjectName("pushButton_13");
        pushButton_13->setMinimumSize(QSize(30, 30));
        pushButton_13->setMaximumSize(QSize(30, 30));
        pushButton_13->setFont(font2);
        pushButton_13->setStyleSheet(QString::fromUtf8("border:none;\n"
"                         border-radius:15px;\n"
"                         background-color: rgb(242, 236, 244)"));
        pushButton_13->setAlignment(Qt::AlignmentFlag::AlignCenter);

        horizontalLayout_10->addWidget(pushButton_13);


        horizontalLayout_5->addWidget(frame_9);

        frame_11 = new QFrame(frame_2);
        frame_11->setObjectName("frame_11");
        frame_11->setMinimumSize(QSize(90, 0));
        frame_11->setMaximumSize(QSize(90, 16777215));
        frame_11->setStyleSheet(QString::fromUtf8(""));
        frame_11->setFrameShape(QFrame::Shape::StyledPanel);
        frame_11->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_2 = new QHBoxLayout(frame_11);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        pushButton_7 = new QLabel(frame_11);
        pushButton_7->setObjectName("pushButton_7");
        pushButton_7->setFont(font);
        pushButton_7->setStyleSheet(QString::fromUtf8("border:none;\n"
"                         background-color: transparent"));

        horizontalLayout_2->addWidget(pushButton_7);

        pushButton_14 = new QLabel(frame_11);
        pushButton_14->setObjectName("pushButton_14");
        pushButton_14->setMinimumSize(QSize(30, 30));
        pushButton_14->setMaximumSize(QSize(30, 30));
        pushButton_14->setFont(font2);
        pushButton_14->setStyleSheet(QString::fromUtf8("border:none;\n"
"                         border-radius:15px;\n"
"                         background-color: rgb(242, 236, 244)"));
        pushButton_14->setAlignment(Qt::AlignmentFlag::AlignCenter);

        horizontalLayout_2->addWidget(pushButton_14, 0, Qt::AlignmentFlag::AlignRight);


        horizontalLayout_5->addWidget(frame_11);

        refresh_Button = new QPushButton(frame_2);
        refresh_Button->setObjectName("refresh_Button");
        refresh_Button->setMinimumSize(QSize(50, 50));
        refresh_Button->setMaximumSize(QSize(50, 50));
        refresh_Button->setStyleSheet(QString::fromUtf8("QPushButton {\n"
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
        refresh_Button->setIcon(icon5);
        refresh_Button->setIconSize(QSize(20, 20));

        horizontalLayout_5->addWidget(refresh_Button);

        horizontalSpacer = new QSpacerItem(774, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer);

        searchlist_lineEdit = new QLineEdit(frame_2);
        searchlist_lineEdit->setObjectName("searchlist_lineEdit");
        searchlist_lineEdit->setMinimumSize(QSize(200, 25));
        searchlist_lineEdit->setMaximumSize(QSize(200, 25));
        searchlist_lineEdit->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
        searchlist_lineEdit->setStyleSheet(QString::fromUtf8(""));

        horizontalLayout_5->addWidget(searchlist_lineEdit);


        verticalLayout_5->addWidget(frame_2);


        verticalLayout_2->addWidget(header_frame);

        animationdata_frame = new QFrame(showmain_stackedWidgetPage1);
        animationdata_frame->setObjectName("animationdata_frame");
        animationdata_frame->setStyleSheet(QString::fromUtf8("background-color: white;"));
        animationdata_frame->setFrameShape(QFrame::Shape::NoFrame);
        gridLayout_2 = new QGridLayout(animationdata_frame);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setHorizontalSpacing(35);
        gridLayout_2->setVerticalSpacing(18);
        gridLayout_2->setContentsMargins(0, 10, 0, 10);

        verticalLayout_2->addWidget(animationdata_frame);

        page_frame = new QFrame(showmain_stackedWidgetPage1);
        page_frame->setObjectName("page_frame");
        page_frame->setMinimumSize(QSize(0, 45));
        page_frame->setMaximumSize(QSize(16777215, 45));
        page_frame->setStyleSheet(QString::fromUtf8("QFrame {\n"
"                background-color: white\n"
"                }\n"
"\n"
"                QPushButton {\n"
"                border-width: 1px;\n"
"                border-style: solid;\n"
"                border-color: rgba(0,0,0,100);\n"
"                border-radius: 4px;\n"
"                }"));
        page_frame->setFrameShape(QFrame::Shape::NoFrame);
        horizontalLayout_3 = new QHBoxLayout(page_frame);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);

        previous_Button = new QPushButton(page_frame);
        previous_Button->setObjectName("previous_Button");
        previous_Button->setMinimumSize(QSize(75, 20));
        previous_Button->setMaximumSize(QSize(75, 20));
        previous_Button->setStyleSheet(QString::fromUtf8("QPushButton:pressed {\n"
"                   background-color: rgba(0,0,0,20)\n"
"                   }"));
        previous_Button->setCheckable(true);

        horizontalLayout_3->addWidget(previous_Button);

        pages_Button = new QLabel(page_frame);
        pages_Button->setObjectName("pages_Button");
        pages_Button->setMinimumSize(QSize(75, 20));
        pages_Button->setMaximumSize(QSize(75, 20));
        pages_Button->setStyleSheet(QString::fromUtf8("border-width: 1px;\n"
"                   border-style: solid;\n"
"                   border-color: rgba(0,0,0,100);\n"
"                   border-radius: 4px;"));
        pages_Button->setAlignment(Qt::AlignmentFlag::AlignCenter);

        horizontalLayout_3->addWidget(pages_Button);

        next_Button = new QPushButton(page_frame);
        next_Button->setObjectName("next_Button");
        next_Button->setMinimumSize(QSize(75, 20));
        next_Button->setMaximumSize(QSize(75, 20));
        next_Button->setStyleSheet(QString::fromUtf8("QPushButton:pressed {\n"
"                   background-color: rgba(0,0,0,20)\n"
"                   }"));

        horizontalLayout_3->addWidget(next_Button);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);


        verticalLayout_2->addWidget(page_frame);

        showmain_stackedWidget->addWidget(showmain_stackedWidgetPage1);

        horizontalLayout_6->addWidget(showmain_stackedWidget);

        main_stackedWidget->addWidget(showmain_page);

        verticalLayout->addWidget(main_stackedWidget);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        main_stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow) const
    {
        pushButton->setText(QString());
        pushButton_8->setText(QString());
        pushButton_2->setText(QString());
        animation_Button->setText(QCoreApplication::translate("MainWindow", "\345\212\250\347\224\273", nullptr));
        novel_Button->setText(QCoreApplication::translate("MainWindow", "\345\260\217\350\257\264", nullptr));
        game_Button->setText(QCoreApplication::translate("MainWindow", "\346\270\270\346\210\217", nullptr));
        comic_Button->setText(QCoreApplication::translate("MainWindow", "\346\274\253\347\224\273", nullptr));
        pushButton_9->setText(QCoreApplication::translate("MainWindow", "\344\270\213\350\275\275", nullptr));
        settings_Button->setText(QString());
        project_Button->setText(QCoreApplication::translate("MainWindow", "\345\212\250\347\224\273", nullptr));
        pushButton_3->setText(QString());
        pushButton_10->setText(QString());
        pushButton_4->setText(QString());
        pushButton_11->setText(QString());
        pushButton_5->setText(QString());
        pushButton_12->setText(QString());
        pushButton_6->setText(QCoreApplication::translate("MainWindow", "\346\220\201\347\275\256", nullptr));
        pushButton_13->setText(QString());
        pushButton_7->setText(QCoreApplication::translate("MainWindow", "\346\212\233\345\274\203", nullptr));
        pushButton_14->setText(QString());
        refresh_Button->setText(QString());
        searchlist_lineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\346\220\234\347\264\242\345\275\223\345\211\215\345\210\227\350\241\250", nullptr));
        previous_Button->setText(QCoreApplication::translate("MainWindow", "\344\270\212\344\270\200\351\241\265", nullptr));
        pages_Button->setText(QString());
        next_Button->setText(QCoreApplication::translate("MainWindow", "\344\270\213\344\270\200\351\241\265", nullptr));
        (void)MainWindow;
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // MAIN_UI_H
