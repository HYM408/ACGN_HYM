/********************************************************************************
** Form generated from reading UI file 'search_ui.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef SEARCH_UI_H
#define SEARCH_UI_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SearchPage
{
public:
    QHBoxLayout *horizontalLayout;
    QFrame *search_frame;
    QVBoxLayout *verticalLayout_6;
    QFrame *searchback_frame;
    QHBoxLayout *horizontalLayout_7;
    QPushButton *back_Button;
    QPushButton *search_pushButton;
    QFrame *frame;
    QHBoxLayout *horizontalLayout_10;
    QLineEdit *search_lineEdit;
    QComboBox *comboBox;
    QFrame *frame_2;
    QVBoxLayout *verticalLayout;
    QCheckBox *checkBox;
    QScrollArea *searchresult_scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_10;
    QSpacerItem *verticalSpacer_4;
    QStackedWidget *stackedWidget;
    QWidget *page;
    QWidget *page_2;

    void setupUi(QWidget *SearchPage)
    {
        if (SearchPage->objectName().isEmpty())
            SearchPage->setObjectName("SearchPage");
        SearchPage->resize(1525, 942);
        horizontalLayout = new QHBoxLayout(SearchPage);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        search_frame = new QFrame(SearchPage);
        search_frame->setObjectName("search_frame");
        search_frame->setMinimumSize(QSize(400, 0));
        search_frame->setMaximumSize(QSize(400, 16777215));
        search_frame->setStyleSheet(QString::fromUtf8("background-color: white;"));
        search_frame->setFrameShape(QFrame::Shape::StyledPanel);
        search_frame->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayout_6 = new QVBoxLayout(search_frame);
        verticalLayout_6->setSpacing(10);
        verticalLayout_6->setObjectName("verticalLayout_6");
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        searchback_frame = new QFrame(search_frame);
        searchback_frame->setObjectName("searchback_frame");
        searchback_frame->setMinimumSize(QSize(0, 50));
        searchback_frame->setMaximumSize(QSize(300, 16777215));
        searchback_frame->setStyleSheet(QString::fromUtf8("background-color: white;"));
        searchback_frame->setFrameShape(QFrame::Shape::StyledPanel);
        searchback_frame->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_7 = new QHBoxLayout(searchback_frame);
        horizontalLayout_7->setSpacing(0);
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        horizontalLayout_7->setContentsMargins(0, 0, 0, 0);
        back_Button = new QPushButton(searchback_frame);
        back_Button->setObjectName("back_Button");
        back_Button->setMinimumSize(QSize(45, 45));
        back_Button->setMaximumSize(QSize(45, 45));
        QFont font;
        font.setBold(false);
        font.setKerning(false);
        back_Button->setFont(font);
        back_Button->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"	background-color: white;\n"
"	border:none;\n"
"	border-radius:22px\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgb(237, 237, 238);\n"
"}"));
        QIcon icon;
        icon.addFile(QString::fromUtf8("icons/back.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        back_Button->setIcon(icon);
        back_Button->setIconSize(QSize(20, 20));
        back_Button->setCheckable(true);

        horizontalLayout_7->addWidget(back_Button);

        search_pushButton = new QPushButton(searchback_frame);
        search_pushButton->setObjectName("search_pushButton");
        search_pushButton->setMinimumSize(QSize(50, 0));
        search_pushButton->setMaximumSize(QSize(50, 16777215));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font1.setPointSize(15);
        font1.setBold(true);
        search_pushButton->setFont(font1);
        search_pushButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    border: none;\n"
"}\n"
""));

        horizontalLayout_7->addWidget(search_pushButton);


        verticalLayout_6->addWidget(searchback_frame, 0, Qt::AlignmentFlag::AlignLeft);

        frame = new QFrame(search_frame);
        frame->setObjectName("frame");
        horizontalLayout_10 = new QHBoxLayout(frame);
        horizontalLayout_10->setObjectName("horizontalLayout_10");
        search_lineEdit = new QLineEdit(frame);
        search_lineEdit->setObjectName("search_lineEdit");
        search_lineEdit->setMinimumSize(QSize(0, 50));
        search_lineEdit->setMaximumSize(QSize(450, 16777215));
        search_lineEdit->setFont(font1);
        search_lineEdit->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
        search_lineEdit->setStyleSheet(QString::fromUtf8("background-color: rgb(236, 230, 238);\n"
"border-radius:25px;\n"
"padding-left: 10px;\n"
""));

        horizontalLayout_10->addWidget(search_lineEdit);

        comboBox = new QComboBox(frame);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName("comboBox");
        comboBox->setMinimumSize(QSize(0, 30));
        comboBox->setMaximumSize(QSize(16777215, 25));
        QFont font2;
        font2.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font2.setPointSize(15);
        comboBox->setFont(font2);
        comboBox->setStyleSheet(QString::fromUtf8("border-color:none;"));

        horizontalLayout_10->addWidget(comboBox);


        verticalLayout_6->addWidget(frame);

        frame_2 = new QFrame(search_frame);
        frame_2->setObjectName("frame_2");
        frame_2->setFrameShape(QFrame::Shape::StyledPanel);
        frame_2->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayout = new QVBoxLayout(frame_2);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(13, 0, 0, 0);
        checkBox = new QCheckBox(frame_2);
        checkBox->setObjectName("checkBox");
        QFont font3;
        font3.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font3.setPointSize(13);
        font3.setBold(false);
        checkBox->setFont(font3);

        verticalLayout->addWidget(checkBox);


        verticalLayout_6->addWidget(frame_2);

        searchresult_scrollArea = new QScrollArea(search_frame);
        searchresult_scrollArea->setObjectName("searchresult_scrollArea");
        searchresult_scrollArea->setStyleSheet(QString::fromUtf8("QScrollArea {\n"
"	border: none;\n"
"}\n"
"\n"
"QScrollArea > QWidget > QWidget {\n"
"    background-color: white;\n"
"}\n"
"\n"
"QScrollBar:vertical {\n"
"    border: none;\n"
"    background: #f0f0f0;\n"
"    width: 11px;\n"
"}\n"
"\n"
"QScrollBar::handle:vertical {\n"
"    background: #c0c0c0;\n"
"    border-radius: 5px;\n"
"    min-height: 20px;\n"
"}\n"
"\n"
"QScrollBar::handle:vertical:hover {\n"
"    background: #a0a0a0;\n"
"}\n"
"\n"
"QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {\n"
"    border: none;\n"
"    background: none;\n"
"    height: 0px;\n"
"}\n"
"\n"
"QScrollBar:horizontal {\n"
"    border: none;\n"
"    background: #f0f0f0;\n"
"    height: 10px;\n"
"}\n"
"\n"
"QScrollBar::handle:horizontal {\n"
"    background: #c0c0c0;\n"
"    border-radius: 5px;\n"
"    min-width: 20px;\n"
"}\n"
"\n"
"QScrollBar::handle:horizontal:hover {\n"
"    background: #a0a0a0;\n"
"}"));
        searchresult_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        searchresult_scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 398, 764));
        verticalLayout_10 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_10->setSpacing(0);
        verticalLayout_10->setObjectName("verticalLayout_10");
        verticalLayout_10->setContentsMargins(5, 0, 0, 0);
        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_10->addItem(verticalSpacer_4);

        searchresult_scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout_6->addWidget(searchresult_scrollArea);


        horizontalLayout->addWidget(search_frame);

        stackedWidget = new QStackedWidget(SearchPage);
        stackedWidget->setObjectName("stackedWidget");
        stackedWidget->setStyleSheet(QString::fromUtf8(""));
        page = new QWidget();
        page->setObjectName("page");
        stackedWidget->addWidget(page);
        page_2 = new QWidget();
        page_2->setObjectName("page_2");
        stackedWidget->addWidget(page_2);

        horizontalLayout->addWidget(stackedWidget);


        retranslateUi(SearchPage);

        QMetaObject::connectSlotsByName(SearchPage);
    } // setupUi

    void retranslateUi(QWidget *SearchPage)
    {
        back_Button->setText(QString());
        search_pushButton->setText(QCoreApplication::translate("SearchPage", "\346\220\234\347\264\242", nullptr));
        search_lineEdit->setPlaceholderText(QCoreApplication::translate("SearchPage", "\345\233\236\350\275\246\346\220\234\347\264\242", nullptr));
        comboBox->setItemText(0, QCoreApplication::translate("SearchPage", "\345\212\250\347\224\273", nullptr));
        comboBox->setItemText(1, QCoreApplication::translate("SearchPage", "\344\271\246\347\261\215", nullptr));
        comboBox->setItemText(2, QCoreApplication::translate("SearchPage", "\346\270\270\346\210\217", nullptr));

        checkBox->setText(QCoreApplication::translate("SearchPage", "tag\346\220\234\347\264\242", nullptr));
        (void)SearchPage;
    } // retranslateUi

};

namespace Ui {
    class SearchPage: public Ui_SearchPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // SEARCH_UI_H
