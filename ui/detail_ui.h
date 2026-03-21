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
    QFrame *frame_6;
    QVBoxLayout *verticalLayout;
    QFrame *frame_3;
    QHBoxLayout *horizontalLayout_11;
    QPushButton *pushButton_19;
    QPushButton *pushButton_20;
    QFrame *frame_4;
    QHBoxLayout *horizontalLayout_14;
    QLabel *cover_label_3;
    QFrame *frame;
    QVBoxLayout *verticalLayout_13;
    QTextEdit *textEdit;
    QLabel *pushButton_23;
    QLabel *pushButton_24;
    QFrame *frame_2;
    QHBoxLayout *horizontalLayout;
    QLabel *pushButton_21;
    QPushButton *pushButton;
    QLabel *pushButton_25;
    QPushButton *pushButton_26;
    QPushButton *pushButton_27;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout_14;
    QFrame *frame_5;
    QTextEdit *textEdit_2;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_2;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QWidget *tab_3;
    QWidget *tab_4;

    void setupUi(QWidget *DetailPage)
    {
        if (DetailPage->objectName().isEmpty())
            DetailPage->setObjectName("DetailPage");
        DetailPage->resize(1525, 942);
        verticalLayout_11 = new QVBoxLayout(DetailPage);
        verticalLayout_11->setSpacing(0);
        verticalLayout_11->setObjectName("verticalLayout_11");
        verticalLayout_11->setContentsMargins(0, 0, 0, 0);
        frame_6 = new QFrame(DetailPage);
        frame_6->setObjectName("frame_6");
        frame_6->setMinimumSize(QSize(0, 114));
        frame_6->setFrameShape(QFrame::Shape::StyledPanel);
        frame_6->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayout = new QVBoxLayout(frame_6);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        frame_3 = new QFrame(frame_6);
        frame_3->setObjectName("frame_3");
        frame_3->setStyleSheet(QString::fromUtf8("background-color: transparent;\n"
"border: none;"));
        frame_3->setFrameShape(QFrame::Shape::StyledPanel);
        frame_3->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_11 = new QHBoxLayout(frame_3);
        horizontalLayout_11->setSpacing(0);
        horizontalLayout_11->setObjectName("horizontalLayout_11");
        horizontalLayout_11->setContentsMargins(0, 0, 0, 0);
        pushButton_19 = new QPushButton(frame_3);
        pushButton_19->setObjectName("pushButton_19");
        pushButton_19->setMinimumSize(QSize(45, 45));
        pushButton_19->setMaximumSize(QSize(45, 45));
        pushButton_19->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"	background-color: transparent;\n"
"	border:none;\n"
"	border-radius:22px\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: rgb(237, 237, 238);\n"
"}"));
        QIcon icon;
        icon.addFile(QString::fromUtf8("icons/back.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        pushButton_19->setIcon(icon);
        pushButton_19->setIconSize(QSize(25, 25));

        horizontalLayout_11->addWidget(pushButton_19, 0, Qt::AlignmentFlag::AlignLeft);

        pushButton_20 = new QPushButton(frame_3);
        pushButton_20->setObjectName("pushButton_20");
        pushButton_20->setMinimumSize(QSize(45, 45));
        pushButton_20->setMaximumSize(QSize(45, 45));
        pushButton_20->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"	background-color: transparent;\n"
"	border:none;\n"
"	border-radius:22px\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"	background-color: rgb(237, 237, 238);\n"
"}"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8("icons/openUrl.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        pushButton_20->setIcon(icon1);
        pushButton_20->setIconSize(QSize(20, 20));

        horizontalLayout_11->addWidget(pushButton_20, 0, Qt::AlignmentFlag::AlignRight);


        verticalLayout->addWidget(frame_3);

        frame_4 = new QFrame(frame_6);
        frame_4->setObjectName("frame_4");
        frame_4->setMinimumSize(QSize(0, 310));
        frame_4->setMaximumSize(QSize(16777215, 310));
        frame_4->setStyleSheet(QString::fromUtf8("QFrame {\n"
"	background-color: transparent;\n"
"	border: none;\n"
"}"));
        frame_4->setFrameShape(QFrame::Shape::StyledPanel);
        frame_4->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_14 = new QHBoxLayout(frame_4);
        horizontalLayout_14->setSpacing(0);
        horizontalLayout_14->setObjectName("horizontalLayout_14");
        horizontalLayout_14->setContentsMargins(50, 0, 150, 0);
        cover_label_3 = new QLabel(frame_4);
        cover_label_3->setObjectName("cover_label_3");
        cover_label_3->setMinimumSize(QSize(220, 310));
        cover_label_3->setMaximumSize(QSize(220, 310));
        cover_label_3->setStyleSheet(QString::fromUtf8("border-radius: 15px;"));
        cover_label_3->setAlignment(Qt::AlignmentFlag::AlignCenter);

        horizontalLayout_14->addWidget(cover_label_3);

        frame = new QFrame(frame_4);
        frame->setObjectName("frame");
        verticalLayout_13 = new QVBoxLayout(frame);
        verticalLayout_13->setSpacing(13);
        verticalLayout_13->setObjectName("verticalLayout_13");
        verticalLayout_13->setContentsMargins(10, -1, -1, 1);
        textEdit = new QTextEdit(frame);
        textEdit->setObjectName("textEdit");
        textEdit->setMinimumSize(QSize(0, 45));
        textEdit->setMaximumSize(QSize(16777215, 45));
        QFont font;
        font.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font.setPointSize(20);
        font.setBold(true);
        textEdit->setFont(font);
        textEdit->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
        textEdit->setFrameShape(QFrame::Shape::NoFrame);
        textEdit->setReadOnly(true);

        verticalLayout_13->addWidget(textEdit);

        pushButton_23 = new QLabel(frame);
        pushButton_23->setObjectName("pushButton_23");
        pushButton_23->setMinimumSize(QSize(0, 35));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font1.setPointSize(15);
        font1.setBold(false);
        pushButton_23->setFont(font1);
        pushButton_23->setStyleSheet(QString::fromUtf8("border: 1px solid gray;\n"
"border-radius:8px;\n"
"padding: 2px 10px;"));
        pushButton_23->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_13->addWidget(pushButton_23, 0, Qt::AlignmentFlag::AlignLeft);

        pushButton_24 = new QLabel(frame);
        pushButton_24->setObjectName("pushButton_24");
        QFont font2;
        font2.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font2.setPointSize(15);
        pushButton_24->setFont(font2);
        pushButton_24->setStyleSheet(QString::fromUtf8("border: none;"));

        verticalLayout_13->addWidget(pushButton_24, 0, Qt::AlignmentFlag::AlignLeft);

        frame_2 = new QFrame(frame);
        frame_2->setObjectName("frame_2");
        frame_2->setFrameShape(QFrame::Shape::StyledPanel);
        frame_2->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout = new QHBoxLayout(frame_2);
        horizontalLayout->setSpacing(20);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        pushButton_21 = new QLabel(frame_2);
        pushButton_21->setObjectName("pushButton_21");
        pushButton_21->setFont(font2);
        pushButton_21->setStyleSheet(QString::fromUtf8("border: none;"));

        horizontalLayout->addWidget(pushButton_21);

        pushButton = new QPushButton(frame_2);
        pushButton->setObjectName("pushButton");
        pushButton->setMinimumSize(QSize(80, 35));
        pushButton->setFont(font2);

        horizontalLayout->addWidget(pushButton);


        verticalLayout_13->addWidget(frame_2, 0, Qt::AlignmentFlag::AlignLeft);

        pushButton_25 = new QLabel(frame);
        pushButton_25->setObjectName("pushButton_25");
        pushButton_25->setFont(font2);
        pushButton_25->setStyleSheet(QString::fromUtf8("border: none;"));

        verticalLayout_13->addWidget(pushButton_25, 0, Qt::AlignmentFlag::AlignLeft);

        pushButton_26 = new QPushButton(frame);
        pushButton_26->setObjectName("pushButton_26");
        pushButton_26->setMinimumSize(QSize(80, 35));
        pushButton_26->setFont(font2);

        verticalLayout_13->addWidget(pushButton_26, 0, Qt::AlignmentFlag::AlignLeft);

        pushButton_27 = new QPushButton(frame);
        pushButton_27->setObjectName("pushButton_27");
        pushButton_27->setMinimumSize(QSize(80, 35));
        QFont font3;
        font3.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font3.setPointSize(14);
        pushButton_27->setFont(font3);

        verticalLayout_13->addWidget(pushButton_27, 0, Qt::AlignmentFlag::AlignLeft);


        horizontalLayout_14->addWidget(frame);


        verticalLayout->addWidget(frame_4);

        tabWidget = new QTabWidget(frame_6);
        tabWidget->setObjectName("tabWidget");
        tabWidget->setMinimumSize(QSize(0, 100));
        QFont font4;
        font4.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font4.setPointSize(12);
        font4.setBold(false);
        tabWidget->setFont(font4);
        tabWidget->setStyleSheet(QString::fromUtf8("QTabWidget::tab-bar {\n"
"	alignment: center;\n"
"}\n"
"\n"
"QTabWidget {\n"
"	border:none;\n"
"}"));
        tab = new QWidget();
        tab->setObjectName("tab");
        verticalLayout_14 = new QVBoxLayout(tab);
        verticalLayout_14->setSpacing(10);
        verticalLayout_14->setObjectName("verticalLayout_14");
        verticalLayout_14->setContentsMargins(50, 20, 20, 0);
        frame_5 = new QFrame(tab);
        frame_5->setObjectName("frame_5");
        frame_5->setStyleSheet(QString::fromUtf8("border: none;"));
        frame_5->setFrameShape(QFrame::Shape::StyledPanel);
        frame_5->setFrameShadow(QFrame::Shadow::Raised);

        verticalLayout_14->addWidget(frame_5, 0, Qt::AlignmentFlag::AlignTop);

        textEdit_2 = new QTextEdit(tab);
        textEdit_2->setObjectName("textEdit_2");
        QFont font5;
        font5.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font5.setPointSize(10);
        font5.setBold(true);
        textEdit_2->setFont(font5);
        textEdit_2->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
        textEdit_2->setStyleSheet(QString::fromUtf8("QScrollBar:vertical {\n"
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
""));
        textEdit_2->setFrameShape(QFrame::Shape::NoFrame);
        textEdit_2->setReadOnly(true);

        verticalLayout_14->addWidget(textEdit_2);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName("tab_2");
        verticalLayout_2 = new QVBoxLayout(tab_2);
        verticalLayout_2->setSpacing(10);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(50, 20, 20, 0);
        scrollArea = new QScrollArea(tab_2);
        scrollArea->setObjectName("scrollArea");
        scrollArea->setStyleSheet(QString::fromUtf8("QScrollArea {\n"
"	border: none;\n"
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
""));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 1451, 534));
        scrollAreaWidgetContents->setStyleSheet(QString::fromUtf8("background-color: transparent;"));
        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout_2->addWidget(scrollArea);

        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName("tab_3");
        tabWidget->addTab(tab_3, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName("tab_4");
        tabWidget->addTab(tab_4, QString());

        verticalLayout->addWidget(tabWidget);


        verticalLayout_11->addWidget(frame_6);


        retranslateUi(DetailPage);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(DetailPage);
    } // setupUi

    void retranslateUi(QWidget *DetailPage) const
    {
        pushButton_19->setText(QString());
        pushButton_20->setText(QString());
        cover_label_3->setText(QString());
        pushButton_23->setText(QCoreApplication::translate("DetailPage", "TBA", nullptr));
        pushButton_24->setText(QCoreApplication::translate("DetailPage", "\345\205\250-\350\257\235", nullptr));
        pushButton_21->setText(QCoreApplication::translate("DetailPage", "|\344\272\272\350\257\204|#", nullptr));
        pushButton_25->setText(QCoreApplication::translate("DetailPage", "\346\224\266\350\227\217/\345\234\250\347\234\213/\346\212\233\345\274\203", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("DetailPage", "\350\257\246\346\203\205", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("DetailPage", "\350\247\222\350\211\262", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QCoreApplication::translate("DetailPage", "\347\233\270\345\205\263\346\235\241\347\233\256", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QCoreApplication::translate("DetailPage", "\345\210\266\344\275\234\344\272\272\345\221\230", nullptr));
        (void)DetailPage;
    } // retranslateUi

};

namespace Ui {
    class DetailPage: public Ui_DetailPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // DETAIL_UI_H
