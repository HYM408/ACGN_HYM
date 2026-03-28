/********************************************************************************
** Form generated from reading UI file 'search_ui.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef SEARCH_UI_H
#define SEARCH_UI_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SearchPage
{
public:
    QHBoxLayout *horizontalLayout;
    QFrame *searchFrame;
    QVBoxLayout *verticalLayout_6;
    QFrame *searchBackFrame;
    QHBoxLayout *horizontalLayout_7;
    QPushButton *btnBack;
    QPushButton *btnSearch;
    QFrame *searchInputFrame;
    QHBoxLayout *horizontalLayout_10;
    QLineEdit *lineEditSearch;
    QComboBox *comboSearchType;
    QFrame *searchOptionsFrame;
    QHBoxLayout *horizontalLayout_2;
    QCheckBox *checkBoxTag;
    QCheckBox *checkBoxNsfw;
    QScrollArea *searchResultScrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_10;
    QSpacerItem *verticalSpacer;
    QStackedWidget *stackedWidget;
    QWidget *emptyPage;
    QWidget *detailPageContainer;

    void setupUi(QWidget *SearchPage)
    {
        if (SearchPage->objectName().isEmpty())
            SearchPage->setObjectName("SearchPage");
        SearchPage->resize(1525, 942);
        horizontalLayout = new QHBoxLayout(SearchPage);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        searchFrame = new QFrame(SearchPage);
        searchFrame->setObjectName("searchFrame");
        searchFrame->setMinimumSize(QSize(400, 0));
        searchFrame->setMaximumSize(QSize(400, 16777215));
        searchFrame->setStyleSheet(QString::fromUtf8("QFrame {\n"
"       background-color: rgb(255, 255, 255);\n"
"       }"));
        searchFrame->setFrameShape(QFrame::Shape::StyledPanel);
        searchFrame->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayout_6 = new QVBoxLayout(searchFrame);
        verticalLayout_6->setSpacing(10);
        verticalLayout_6->setObjectName("verticalLayout_6");
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        searchBackFrame = new QFrame(searchFrame);
        searchBackFrame->setObjectName("searchBackFrame");
        searchBackFrame->setMinimumSize(QSize(0, 50));
        searchBackFrame->setMaximumSize(QSize(300, 16777215));
        searchBackFrame->setStyleSheet(QString::fromUtf8(""));
        searchBackFrame->setFrameShape(QFrame::Shape::StyledPanel);
        searchBackFrame->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_7 = new QHBoxLayout(searchBackFrame);
        horizontalLayout_7->setSpacing(0);
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        horizontalLayout_7->setContentsMargins(0, 0, 0, 0);
        btnBack = new QPushButton(searchBackFrame);
        btnBack->setObjectName("btnBack");
        btnBack->setMinimumSize(QSize(45, 45));
        btnBack->setMaximumSize(QSize(45, 45));
        QFont font;
        font.setBold(false);
        font.setKerning(false);
        btnBack->setFont(font);
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
        btnBack->setIconSize(QSize(20, 20));
        btnBack->setCheckable(true);

        horizontalLayout_7->addWidget(btnBack);

        btnSearch = new QPushButton(searchBackFrame);
        btnSearch->setObjectName("btnSearch");
        btnSearch->setMinimumSize(QSize(50, 0));
        btnSearch->setMaximumSize(QSize(50, 16777215));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font1.setPointSize(15);
        font1.setBold(true);
        btnSearch->setFont(font1);
        btnSearch->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"             border: none;\n"
"             }\n"
"            "));

        horizontalLayout_7->addWidget(btnSearch);


        verticalLayout_6->addWidget(searchBackFrame, 0, Qt::AlignmentFlag::AlignLeft);

        searchInputFrame = new QFrame(searchFrame);
        searchInputFrame->setObjectName("searchInputFrame");
        horizontalLayout_10 = new QHBoxLayout(searchInputFrame);
        horizontalLayout_10->setObjectName("horizontalLayout_10");
        lineEditSearch = new QLineEdit(searchInputFrame);
        lineEditSearch->setObjectName("lineEditSearch");
        lineEditSearch->setMinimumSize(QSize(0, 50));
        lineEditSearch->setMaximumSize(QSize(450, 16777215));
        lineEditSearch->setFont(font1);
        lineEditSearch->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
        lineEditSearch->setStyleSheet(QString::fromUtf8("border-radius:25px;\n"
"             padding-left: 10px;"));

        horizontalLayout_10->addWidget(lineEditSearch);

        comboSearchType = new QComboBox(searchInputFrame);
        comboSearchType->addItem(QString());
        comboSearchType->addItem(QString());
        comboSearchType->addItem(QString());
        comboSearchType->setObjectName("comboSearchType");
        comboSearchType->setMinimumSize(QSize(0, 30));
        comboSearchType->setMaximumSize(QSize(16777215, 25));
        QFont font2;
        font2.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font2.setPointSize(15);
        comboSearchType->setFont(font2);

        horizontalLayout_10->addWidget(comboSearchType);


        verticalLayout_6->addWidget(searchInputFrame);

        searchOptionsFrame = new QFrame(searchFrame);
        searchOptionsFrame->setObjectName("searchOptionsFrame");
        searchOptionsFrame->setFrameShape(QFrame::Shape::StyledPanel);
        searchOptionsFrame->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_2 = new QHBoxLayout(searchOptionsFrame);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(13, 0, 0, 0);
        checkBoxTag = new QCheckBox(searchOptionsFrame);
        checkBoxTag->setObjectName("checkBoxTag");
        QFont font3;
        font3.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font3.setPointSize(13);
        font3.setBold(false);
        checkBoxTag->setFont(font3);

        horizontalLayout_2->addWidget(checkBoxTag);

        checkBoxNsfw = new QCheckBox(searchOptionsFrame);
        checkBoxNsfw->setObjectName("checkBoxNsfw");
        QFont font4;
        font4.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font4.setPointSize(13);
        checkBoxNsfw->setFont(font4);

        horizontalLayout_2->addWidget(checkBoxNsfw);


        verticalLayout_6->addWidget(searchOptionsFrame, 0, Qt::AlignmentFlag::AlignLeft);

        searchResultScrollArea = new QScrollArea(searchFrame);
        searchResultScrollArea->setObjectName("searchResultScrollArea");
        searchResultScrollArea->setStyleSheet(QString::fromUtf8("QScrollArea {\n"
"          border: none;\n"
"          }\n"
"\n"
"          QScrollBar:vertical {\n"
"          border: none;\n"
"          background: #f0f0f0;\n"
"          width: 11px;\n"
"          }\n"
"\n"
"          QScrollBar::handle:vertical {\n"
"          background: #c0c0c0;\n"
"          border-radius: 5px;\n"
"          min-height: 20px;\n"
"          }\n"
"\n"
"          QScrollBar::handle:vertical:hover {\n"
"          background: #a0a0a0;\n"
"          }\n"
"\n"
"          QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {\n"
"          border: none;\n"
"          background: none;\n"
"          height: 0px;\n"
"          }"));
        searchResultScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        searchResultScrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 398, 764));
        scrollAreaWidgetContents->setStyleSheet(QString::fromUtf8("background-color: transparent"));
        verticalLayout_10 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_10->setObjectName("verticalLayout_10");
        verticalLayout_10->setContentsMargins(5, -1, -1, -1);
        verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_10->addItem(verticalSpacer);

        searchResultScrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout_6->addWidget(searchResultScrollArea);


        horizontalLayout->addWidget(searchFrame);

        stackedWidget = new QStackedWidget(SearchPage);
        stackedWidget->setObjectName("stackedWidget");
        emptyPage = new QWidget();
        emptyPage->setObjectName("emptyPage");
        stackedWidget->addWidget(emptyPage);
        detailPageContainer = new QWidget();
        detailPageContainer->setObjectName("detailPageContainer");
        stackedWidget->addWidget(detailPageContainer);

        horizontalLayout->addWidget(stackedWidget);


        retranslateUi(SearchPage);

        QMetaObject::connectSlotsByName(SearchPage);
    } // setupUi

    void retranslateUi(QWidget *SearchPage) const
    {
        btnBack->setText(QString());
        btnSearch->setText(QCoreApplication::translate("SearchPage", "\346\220\234\347\264\242", nullptr));
        lineEditSearch->setPlaceholderText(QCoreApplication::translate("SearchPage", "\345\233\236\350\275\246\346\220\234\347\264\242", nullptr));
        comboSearchType->setItemText(0, QCoreApplication::translate("SearchPage", "\345\212\250\347\224\273", nullptr));
        comboSearchType->setItemText(1, QCoreApplication::translate("SearchPage", "\344\271\246\347\261\215", nullptr));
        comboSearchType->setItemText(2, QCoreApplication::translate("SearchPage", "\346\270\270\346\210\217", nullptr));

        checkBoxTag->setText(QCoreApplication::translate("SearchPage", "tag\346\220\234\347\264\242", nullptr));
        checkBoxNsfw->setText(QCoreApplication::translate("SearchPage", "NSFW", nullptr));
        (void)SearchPage;
    } // retranslateUi

};

namespace Ui {
    class SearchPage: public Ui_SearchPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // SEARCH_UI_H
