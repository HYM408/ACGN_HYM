/********************************************************************************
** Form generated from reading UI file 'settings_ui.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef SETTINGS_UI_H
#define SETTINGS_UI_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SettingsPage
{
public:
    QHBoxLayout *horizontalLayout_8;
    QFrame *settings_frame;
    QVBoxLayout *verticalLayout;
    QFrame *Frame;
    QHBoxLayout *horizontalLayout;
    QPushButton *back_Button_2;
    QPushButton *setting_Button;
    QSpacerItem *horizontalSpacer;
    QFrame *Frame_2;
    QVBoxLayout *verticalLayout_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QPushButton *pushButton_13;
    QSpacerItem *verticalSpacer;
    QStackedWidget *stackedWidget_2;
    QWidget *login_page;
    QVBoxLayout *verticalLayout_7;
    QLabel *pushButton;
    QFrame *Frame_3;
    QVBoxLayout *verticalLayout_4;
    QLabel *pushButton_7;
    QLabel *pushButton_2;
    QLabel *pushButton_9;
    QLabel *pushButton_8;
    QPushButton *login_Button;
    QPushButton *collection_Button;
    QFrame *Frame_4;
    QHBoxLayout *horizontalLayout_2;
    QLabel *pushButton_12;
    QComboBox *comboBox;
    QSpacerItem *horizontalSpacer_2;
    QFrame *frame;
    QVBoxLayout *verticalLayout_6;
    QLabel *pushButton_16;
    QFrame *frame_5;
    QVBoxLayout *verticalLayout_8;
    QCheckBox *checkBox;
    QCheckBox *checkBox_2;
    QCheckBox *checkBox_3;
    QFrame *frame_4;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *pushButton_17;
    QPushButton *pushButton_19;
    QPushButton *pushButton_20;
    QSpacerItem *verticalSpacer_3;
    QWidget *pikpak_page;
    QVBoxLayout *verticalLayout_3;
    QLabel *pushButton_5;
    QFrame *frame_2;
    QVBoxLayout *verticalLayout_9;
    QLabel *pushButton_6;
    QLabel *pushButton_10;
    QLabel *pushButton_11;
    QLabel *pushButton_15;
    QPushButton *login_Button_2;
    QSpacerItem *verticalSpacer_2;
    QWidget *download_page;
    QVBoxLayout *verticalLayout_5;
    QLabel *pushButton_14;
    QFrame *frame_3;
    QHBoxLayout *horizontalLayout_3;
    QLineEdit *lineEdit;
    QPushButton *login_Button_3;
    QSpacerItem *verticalSpacer_4;
    QButtonGroup *buttonGroup;

    void setupUi(QWidget *SettingsPage)
    {
        if (SettingsPage->objectName().isEmpty())
            SettingsPage->setObjectName("SettingsPage");
        SettingsPage->resize(1525, 942);
        horizontalLayout_8 = new QHBoxLayout(SettingsPage);
        horizontalLayout_8->setSpacing(0);
        horizontalLayout_8->setObjectName("horizontalLayout_8");
        horizontalLayout_8->setContentsMargins(0, 0, 0, 0);
        settings_frame = new QFrame(SettingsPage);
        settings_frame->setObjectName("settings_frame");
        settings_frame->setMinimumSize(QSize(300, 0));
        settings_frame->setMaximumSize(QSize(300, 16777215));
        settings_frame->setStyleSheet(QString::fromUtf8("background-color: white;"));
        settings_frame->setFrameShape(QFrame::Shape::StyledPanel);
        settings_frame->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayout = new QVBoxLayout(settings_frame);
        verticalLayout->setObjectName("verticalLayout");
        Frame = new QFrame(settings_frame);
        Frame->setObjectName("Frame");
        horizontalLayout = new QHBoxLayout(Frame);
        horizontalLayout->setObjectName("horizontalLayout");
        back_Button_2 = new QPushButton(Frame);
        back_Button_2->setObjectName("back_Button_2");
        back_Button_2->setMinimumSize(QSize(45, 45));
        back_Button_2->setMaximumSize(QSize(45, 45));
        QFont font;
        font.setBold(false);
        font.setKerning(false);
        back_Button_2->setFont(font);
        back_Button_2->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"             background-color: white;\n"
"             border:none;\n"
"             border-radius:22px\n"
"             }\n"
"\n"
"             QPushButton:hover {\n"
"             background-color: rgb(237, 237, 238);\n"
"             }"));
        QIcon icon;
        icon.addFile(QString::fromUtf8("icons/back.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        back_Button_2->setIcon(icon);
        back_Button_2->setIconSize(QSize(20, 20));
        back_Button_2->setCheckable(true);

        horizontalLayout->addWidget(back_Button_2);

        setting_Button = new QPushButton(Frame);
        setting_Button->setObjectName("setting_Button");
        setting_Button->setMinimumSize(QSize(175, 0));
        setting_Button->setMaximumSize(QSize(175, 16777215));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font1.setPointSize(25);
        font1.setBold(true);
        setting_Button->setFont(font1);
        setting_Button->setStyleSheet(QString::fromUtf8("border:none"));

        horizontalLayout->addWidget(setting_Button);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addWidget(Frame);

        Frame_2 = new QFrame(settings_frame);
        Frame_2->setObjectName("Frame_2");
        verticalLayout_2 = new QVBoxLayout(Frame_2);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(-1, 20, -1, -1);
        pushButton_3 = new QPushButton(Frame_2);
        buttonGroup = new QButtonGroup(SettingsPage);
        buttonGroup->setObjectName("buttonGroup");
        buttonGroup->addButton(pushButton_3);
        pushButton_3->setObjectName("pushButton_3");
        pushButton_3->setMinimumSize(QSize(0, 50));
        pushButton_3->setMaximumSize(QSize(16777215, 50));
        QFont font2;
        font2.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font2.setPointSize(15);
        font2.setBold(true);
        pushButton_3->setFont(font2);
        pushButton_3->setCheckable(true);

        verticalLayout_2->addWidget(pushButton_3);

        pushButton_4 = new QPushButton(Frame_2);
        buttonGroup->addButton(pushButton_4);
        pushButton_4->setObjectName("pushButton_4");
        pushButton_4->setMinimumSize(QSize(0, 50));
        pushButton_4->setMaximumSize(QSize(16777215, 50));
        pushButton_4->setFont(font2);
        pushButton_4->setCheckable(true);

        verticalLayout_2->addWidget(pushButton_4);

        pushButton_13 = new QPushButton(Frame_2);
        buttonGroup->addButton(pushButton_13);
        pushButton_13->setObjectName("pushButton_13");
        pushButton_13->setMinimumSize(QSize(0, 50));
        pushButton_13->setMaximumSize(QSize(16777215, 50));
        pushButton_13->setFont(font2);
        pushButton_13->setCheckable(true);

        verticalLayout_2->addWidget(pushButton_13);


        verticalLayout->addWidget(Frame_2);

        verticalSpacer = new QSpacerItem(20, 866, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout_8->addWidget(settings_frame);

        stackedWidget_2 = new QStackedWidget(SettingsPage);
        stackedWidget_2->setObjectName("stackedWidget_2");
        stackedWidget_2->setStyleSheet(QString::fromUtf8("QStackedWidget > QWidget {\n"
"	background-color: transparent;\n"
"}"));
        login_page = new QWidget();
        login_page->setObjectName("login_page");
        verticalLayout_7 = new QVBoxLayout(login_page);
        verticalLayout_7->setObjectName("verticalLayout_7");
        pushButton = new QLabel(login_page);
        pushButton->setObjectName("pushButton");
        pushButton->setMinimumSize(QSize(0, 0));
        pushButton->setMaximumSize(QSize(16777215, 16777215));
        QFont font3;
        font3.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font3.setPointSize(17);
        font3.setBold(true);
        pushButton->setFont(font3);

        verticalLayout_7->addWidget(pushButton, 0, Qt::AlignmentFlag::AlignLeft);

        Frame_3 = new QFrame(login_page);
        Frame_3->setObjectName("Frame_3");
        verticalLayout_4 = new QVBoxLayout(Frame_3);
        verticalLayout_4->setObjectName("verticalLayout_4");
        pushButton_7 = new QLabel(Frame_3);
        pushButton_7->setObjectName("pushButton_7");
        pushButton_7->setMinimumSize(QSize(0, 30));
        pushButton_7->setMaximumSize(QSize(16777215, 30));
        QFont font4;
        font4.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font4.setPointSize(13);
        font4.setBold(true);
        pushButton_7->setFont(font4);

        verticalLayout_4->addWidget(pushButton_7, 0, Qt::AlignmentFlag::AlignLeft);

        pushButton_2 = new QLabel(Frame_3);
        pushButton_2->setObjectName("pushButton_2");
        QFont font5;
        font5.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font5.setPointSize(13);
        font5.setBold(false);
        pushButton_2->setFont(font5);

        verticalLayout_4->addWidget(pushButton_2, 0, Qt::AlignmentFlag::AlignLeft);

        pushButton_9 = new QLabel(Frame_3);
        pushButton_9->setObjectName("pushButton_9");
        pushButton_9->setFont(font5);

        verticalLayout_4->addWidget(pushButton_9, 0, Qt::AlignmentFlag::AlignLeft);

        pushButton_8 = new QLabel(Frame_3);
        pushButton_8->setObjectName("pushButton_8");
        pushButton_8->setFont(font5);

        verticalLayout_4->addWidget(pushButton_8, 0, Qt::AlignmentFlag::AlignLeft);

        login_Button = new QPushButton(Frame_3);
        login_Button->setObjectName("login_Button");
        login_Button->setMinimumSize(QSize(76, 0));
        QFont font6;
        font6.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font6.setPointSize(13);
        login_Button->setFont(font6);

        verticalLayout_4->addWidget(login_Button, 0, Qt::AlignmentFlag::AlignLeft);

        collection_Button = new QPushButton(Frame_3);
        collection_Button->setObjectName("collection_Button");
        collection_Button->setMinimumSize(QSize(76, 0));
        collection_Button->setFont(font6);

        verticalLayout_4->addWidget(collection_Button, 0, Qt::AlignmentFlag::AlignLeft);


        verticalLayout_7->addWidget(Frame_3);

        Frame_4 = new QFrame(login_page);
        Frame_4->setObjectName("Frame_4");
        horizontalLayout_2 = new QHBoxLayout(Frame_4);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        pushButton_12 = new QLabel(Frame_4);
        pushButton_12->setObjectName("pushButton_12");
        pushButton_12->setMinimumSize(QSize(0, 30));
        pushButton_12->setMaximumSize(QSize(16777215, 30));
        pushButton_12->setFont(font4);

        horizontalLayout_2->addWidget(pushButton_12, 0, Qt::AlignmentFlag::AlignLeft);

        comboBox = new QComboBox(Frame_4);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName("comboBox");
        comboBox->setMinimumSize(QSize(0, 25));
        QFont font7;
        font7.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font7.setPointSize(10);
        comboBox->setFont(font7);

        horizontalLayout_2->addWidget(comboBox, 0, Qt::AlignmentFlag::AlignLeft);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout_7->addWidget(Frame_4);

        frame = new QFrame(login_page);
        frame->setObjectName("frame");
        frame->setFrameShape(QFrame::Shape::StyledPanel);
        frame->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayout_6 = new QVBoxLayout(frame);
        verticalLayout_6->setObjectName("verticalLayout_6");
        pushButton_16 = new QLabel(frame);
        pushButton_16->setObjectName("pushButton_16");
        pushButton_16->setMinimumSize(QSize(0, 30));
        pushButton_16->setMaximumSize(QSize(16777215, 30));
        pushButton_16->setFont(font4);

        verticalLayout_6->addWidget(pushButton_16, 0, Qt::AlignmentFlag::AlignLeft);

        frame_5 = new QFrame(frame);
        frame_5->setObjectName("frame_5");
        frame_5->setFrameShape(QFrame::Shape::StyledPanel);
        frame_5->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayout_8 = new QVBoxLayout(frame_5);
        verticalLayout_8->setObjectName("verticalLayout_8");
        verticalLayout_8->setContentsMargins(0, -1, -1, -1);
        checkBox = new QCheckBox(frame_5);
        checkBox->setObjectName("checkBox");
        checkBox->setFont(font7);
        checkBox->setChecked(true);

        verticalLayout_8->addWidget(checkBox);

        checkBox_2 = new QCheckBox(frame_5);
        checkBox_2->setObjectName("checkBox_2");
        checkBox_2->setFont(font7);

        verticalLayout_8->addWidget(checkBox_2);

        checkBox_3 = new QCheckBox(frame_5);
        checkBox_3->setObjectName("checkBox_3");
        checkBox_3->setFont(font7);

        verticalLayout_8->addWidget(checkBox_3);

        frame_4 = new QFrame(frame_5);
        frame_4->setObjectName("frame_4");
        frame_4->setFrameShape(QFrame::Shape::StyledPanel);
        frame_4->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_4 = new QHBoxLayout(frame_4);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalLayout_4->setContentsMargins(0, 0, -1, 5);
        pushButton_17 = new QPushButton(frame_4);
        pushButton_17->setObjectName("pushButton_17");
        pushButton_17->setMinimumSize(QSize(76, 0));
        pushButton_17->setMaximumSize(QSize(76, 16777215));
        pushButton_17->setFont(font6);

        horizontalLayout_4->addWidget(pushButton_17);

        pushButton_19 = new QPushButton(frame_4);
        pushButton_19->setObjectName("pushButton_19");
        pushButton_19->setMinimumSize(QSize(76, 0));
        pushButton_19->setMaximumSize(QSize(76, 16777215));
        pushButton_19->setFont(font6);

        horizontalLayout_4->addWidget(pushButton_19);

        pushButton_20 = new QPushButton(frame_4);
        pushButton_20->setObjectName("pushButton_20");
        pushButton_20->setFont(font6);
        pushButton_20->setStyleSheet(QString::fromUtf8("border: none;"));

        horizontalLayout_4->addWidget(pushButton_20);


        verticalLayout_8->addWidget(frame_4, 0, Qt::AlignmentFlag::AlignLeft);


        verticalLayout_6->addWidget(frame_5);


        verticalLayout_7->addWidget(frame);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_7->addItem(verticalSpacer_3);

        stackedWidget_2->addWidget(login_page);
        pikpak_page = new QWidget();
        pikpak_page->setObjectName("pikpak_page");
        verticalLayout_3 = new QVBoxLayout(pikpak_page);
        verticalLayout_3->setObjectName("verticalLayout_3");
        pushButton_5 = new QLabel(pikpak_page);
        pushButton_5->setObjectName("pushButton_5");
        pushButton_5->setMinimumSize(QSize(0, 0));
        pushButton_5->setMaximumSize(QSize(16777215, 16777215));
        QFont font8;
        font8.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font8.setPointSize(20);
        font8.setBold(true);
        pushButton_5->setFont(font8);

        verticalLayout_3->addWidget(pushButton_5, 0, Qt::AlignmentFlag::AlignLeft);

        frame_2 = new QFrame(pikpak_page);
        frame_2->setObjectName("frame_2");
        frame_2->setFrameShape(QFrame::Shape::StyledPanel);
        frame_2->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayout_9 = new QVBoxLayout(frame_2);
        verticalLayout_9->setObjectName("verticalLayout_9");
        pushButton_6 = new QLabel(frame_2);
        pushButton_6->setObjectName("pushButton_6");
        pushButton_6->setFont(font5);

        verticalLayout_9->addWidget(pushButton_6, 0, Qt::AlignmentFlag::AlignLeft);

        pushButton_10 = new QLabel(frame_2);
        pushButton_10->setObjectName("pushButton_10");
        pushButton_10->setFont(font5);

        verticalLayout_9->addWidget(pushButton_10, 0, Qt::AlignmentFlag::AlignLeft);

        pushButton_11 = new QLabel(frame_2);
        pushButton_11->setObjectName("pushButton_11");
        pushButton_11->setFont(font6);

        verticalLayout_9->addWidget(pushButton_11, 0, Qt::AlignmentFlag::AlignLeft);

        pushButton_15 = new QLabel(frame_2);
        pushButton_15->setObjectName("pushButton_15");
        pushButton_15->setFont(font6);

        verticalLayout_9->addWidget(pushButton_15, 0, Qt::AlignmentFlag::AlignLeft);

        login_Button_2 = new QPushButton(frame_2);
        login_Button_2->setObjectName("login_Button_2");
        login_Button_2->setMinimumSize(QSize(0, 30));
        login_Button_2->setFont(font5);
        login_Button_2->setStyleSheet(QString::fromUtf8(""));

        verticalLayout_9->addWidget(login_Button_2, 0, Qt::AlignmentFlag::AlignLeft);


        verticalLayout_3->addWidget(frame_2);

        verticalSpacer_2 = new QSpacerItem(20, 759, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_2);

        stackedWidget_2->addWidget(pikpak_page);
        download_page = new QWidget();
        download_page->setObjectName("download_page");
        verticalLayout_5 = new QVBoxLayout(download_page);
        verticalLayout_5->setObjectName("verticalLayout_5");
        pushButton_14 = new QLabel(download_page);
        pushButton_14->setObjectName("pushButton_14");
        pushButton_14->setMinimumSize(QSize(0, 0));
        pushButton_14->setMaximumSize(QSize(16777215, 16777215));
        pushButton_14->setFont(font3);

        verticalLayout_5->addWidget(pushButton_14, 0, Qt::AlignmentFlag::AlignLeft);

        frame_3 = new QFrame(download_page);
        frame_3->setObjectName("frame_3");
        frame_3->setFrameShape(QFrame::Shape::StyledPanel);
        frame_3->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_3 = new QHBoxLayout(frame_3);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        lineEdit = new QLineEdit(frame_3);
        lineEdit->setObjectName("lineEdit");
        lineEdit->setReadOnly(true);

        horizontalLayout_3->addWidget(lineEdit);

        login_Button_3 = new QPushButton(frame_3);
        login_Button_3->setObjectName("login_Button_3");
        login_Button_3->setMinimumSize(QSize(0, 30));
        login_Button_3->setFont(font5);
        login_Button_3->setStyleSheet(QString::fromUtf8(""));

        horizontalLayout_3->addWidget(login_Button_3);


        verticalLayout_5->addWidget(frame_3);

        verticalSpacer_4 = new QSpacerItem(20, 829, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_5->addItem(verticalSpacer_4);

        stackedWidget_2->addWidget(download_page);

        horizontalLayout_8->addWidget(stackedWidget_2);


        retranslateUi(SettingsPage);

        stackedWidget_2->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(SettingsPage);
    } // setupUi

    void retranslateUi(QWidget *SettingsPage) const
    {
        back_Button_2->setText(QString());
        setting_Button->setText(QCoreApplication::translate("SettingsPage", "\350\256\276\347\275\256", nullptr));
        pushButton_3->setText(QCoreApplication::translate("SettingsPage", "Bangumi", nullptr));
        pushButton_4->setText(QCoreApplication::translate("SettingsPage", "PikPak", nullptr));
        pushButton_13->setText(QCoreApplication::translate("SettingsPage", "\344\270\213\350\275\275", nullptr));
        pushButton->setText(QCoreApplication::translate("SettingsPage", "Bangumi", nullptr));
        pushButton_7->setText(QCoreApplication::translate("SettingsPage", "Bangumi \346\216\210\346\235\203", nullptr));
        pushButton_2->setText(QString());
        pushButton_9->setText(QString());
        pushButton_8->setText(QString());
        login_Button->setText(QCoreApplication::translate("SettingsPage", "\345\274\200\345\247\213\346\216\210\346\235\203", nullptr));
        collection_Button->setText(QCoreApplication::translate("SettingsPage", "\350\216\267\345\217\226\346\224\266\350\227\217", nullptr));
        pushButton_12->setText(QCoreApplication::translate("SettingsPage", "Bangumi \345\237\237\345\220\215 \357\274\232", nullptr));
        comboBox->setItemText(0, QCoreApplication::translate("SettingsPage", "bangumi.tv", nullptr));
        comboBox->setItemText(1, QCoreApplication::translate("SettingsPage", "bgm.tv", nullptr));
        comboBox->setItemText(2, QCoreApplication::translate("SettingsPage", "chii.in", nullptr));

        pushButton_16->setText(QCoreApplication::translate("SettingsPage", "Bangumi \345\205\254\345\205\261\346\225\260\346\215\256", nullptr));
        checkBox->setText(QCoreApplication::translate("SettingsPage", "\345\212\250\347\224\273", nullptr));
        checkBox_2->setText(QCoreApplication::translate("SettingsPage", "\344\271\246\347\261\215", nullptr));
        checkBox_3->setText(QCoreApplication::translate("SettingsPage", "\346\270\270\346\210\217", nullptr));
        pushButton_17->setText(QCoreApplication::translate("SettingsPage", "\344\270\213\350\275\275", nullptr));
        pushButton_19->setText(QCoreApplication::translate("SettingsPage", "\351\225\234\345\203\217", nullptr));
        pushButton_20->setText(QString());
        pushButton_5->setText(QCoreApplication::translate("SettingsPage", "PikPak \347\231\273\345\275\225", nullptr));
        pushButton_6->setText(QString());
        pushButton_10->setText(QString());
        pushButton_11->setText(QString());
        pushButton_15->setText(QString());
        login_Button_2->setText(QCoreApplication::translate("SettingsPage", "\345\274\200\345\247\213\347\231\273\345\275\225", nullptr));
        pushButton_14->setText(QCoreApplication::translate("SettingsPage", "\344\270\213\350\275\275\350\267\257\345\276\204", nullptr));
        login_Button_3->setText(QCoreApplication::translate("SettingsPage", "\351\200\211\346\213\251\350\267\257\345\276\204", nullptr));
        (void)SettingsPage;
    } // retranslateUi

};

namespace Ui {
    class SettingsPage: public Ui_SettingsPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // SETTINGS_UI_H
