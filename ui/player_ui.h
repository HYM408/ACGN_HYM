/********************************************************************************
** Form generated from reading UI file 'player_ui.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef PLAYER_UI_H
#define PLAYER_UI_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_choicePage
{
public:
    QHBoxLayout *horizontalLayout;
    QWidget *video_container;
    QVBoxLayout *verticalLayout_2;
    QFrame *sidePanelFrame;
    QVBoxLayout *verticalLayout_4;
    QTabWidget *tabWidget;
    QWidget *tabBasic;
    QVBoxLayout *verticalLayout;
    QScrollArea *scrollAreaBasic;
    QWidget *basicContent;
    QWidget *tabDetail;
    QVBoxLayout *verticalLayout_3;

    void setupUi(QWidget *choicePage)
    {
        if (choicePage->objectName().isEmpty())
            choicePage->setObjectName("choicePage");
        choicePage->resize(1525, 942);
        horizontalLayout = new QHBoxLayout(choicePage);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        video_container = new QWidget(choicePage);
        video_container->setObjectName("video_container");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(3);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(video_container->sizePolicy().hasHeightForWidth());
        video_container->setSizePolicy(sizePolicy);
        video_container->setStyleSheet(QString::fromUtf8("background-color: black;"));
        verticalLayout_2 = new QVBoxLayout(video_container);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);

        horizontalLayout->addWidget(video_container);

        sidePanelFrame = new QFrame(choicePage);
        sidePanelFrame->setObjectName("sidePanelFrame");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(sidePanelFrame->sizePolicy().hasHeightForWidth());
        sidePanelFrame->setSizePolicy(sizePolicy1);
        sidePanelFrame->setStyleSheet(QString::fromUtf8("QFrame {\n"
"	border:none;\n"
"}"));
        sidePanelFrame->setFrameShape(QFrame::Shape::StyledPanel);
        sidePanelFrame->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayout_4 = new QVBoxLayout(sidePanelFrame);
        verticalLayout_4->setSpacing(0);
        verticalLayout_4->setObjectName("verticalLayout_4");
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        tabWidget = new QTabWidget(sidePanelFrame);
        tabWidget->setObjectName("tabWidget");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy2.setHorizontalStretch(1);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy2);
        tabWidget->setStyleSheet(QString::fromUtf8("QTabWidget::pane {\n"
"    border-right: none;\n"
"}"));
        tabBasic = new QWidget();
        tabBasic->setObjectName("tabBasic");
        verticalLayout = new QVBoxLayout(tabBasic);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        scrollAreaBasic = new QScrollArea(tabBasic);
        scrollAreaBasic->setObjectName("scrollAreaBasic");
        scrollAreaBasic->setStyleSheet(QString::fromUtf8("QScrollBar:vertical {\n"
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
"}\n"
"\n"
""));
        scrollAreaBasic->setWidgetResizable(true);
        basicContent = new QWidget();
        basicContent->setObjectName("basicContent");
        basicContent->setGeometry(QRect(0, 0, 381, 919));
        basicContent->setStyleSheet(QString::fromUtf8("background-color: transparent;"));
        scrollAreaBasic->setWidget(basicContent);

        verticalLayout->addWidget(scrollAreaBasic);

        tabWidget->addTab(tabBasic, QString());
        tabDetail = new QWidget();
        tabDetail->setObjectName("tabDetail");
        verticalLayout_3 = new QVBoxLayout(tabDetail);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        tabWidget->addTab(tabDetail, QString());

        verticalLayout_4->addWidget(tabWidget);


        horizontalLayout->addWidget(sidePanelFrame);


        retranslateUi(choicePage);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(choicePage);
    } // setupUi

    void retranslateUi(QWidget *choicePage) const
    {
        choicePage->setWindowTitle(QCoreApplication::translate("choicePage", "Form", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabBasic), QCoreApplication::translate("choicePage", "\345\237\272\347\241\200", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabDetail), QCoreApplication::translate("choicePage", "\350\257\246\347\273\206", nullptr));
    } // retranslateUi

};

namespace Ui {
    class choicePage: public Ui_choicePage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // PLAYER_UI_H
