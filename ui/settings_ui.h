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
    QFrame *settingsFrame;
    QVBoxLayout *verticalLayout;
    QFrame *headerFrame;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnBack;
    QPushButton *btnSettingsTitle;
    QSpacerItem *horizontalSpacer;
    QFrame *menuFrame;
    QVBoxLayout *verticalLayout_2;
    QPushButton *btnBangumi;
    QPushButton *btnPikPak;
    QPushButton *btnDownloadMenu;
    QSpacerItem *verticalSpacer;
    QStackedWidget *stackedWidget;
    QWidget *bangumiPage;
    QVBoxLayout *verticalLayout_7;
    QLabel *labelBangumiTitle;
    QFrame *authFrame;
    QVBoxLayout *verticalLayout_4;
    QLabel *labelAuthSection;
    QLabel *labelUserId;
    QLabel *labelAccessToken;
    QLabel *labelRefreshToken;
    QPushButton *btnBangumiAuth;
    QPushButton *btnGetCollection;
    QFrame *domainFrame;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labelDomainSection;
    QComboBox *comboBangumiDomain;
    QSpacerItem *horizontalSpacer_2;
    QFrame *publicDataFrame;
    QVBoxLayout *verticalLayout_6;
    QLabel *labelPublicDataSection;
    QFrame *publicDataOptionsFrame;
    QVBoxLayout *verticalLayout_8;
    QCheckBox *checkBoxAnime;
    QCheckBox *checkBoxBook;
    QCheckBox *checkBoxGame;
    QFrame *publicDataButtonsFrame;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *btnDownloadPublic;
    QPushButton *btnDownloadMirror;
    QPushButton *labelDownloadStatus;
    QFrame *searchOptionsFrame;
    QVBoxLayout *verticalLayout_10;
    QLabel *labelSearchSection;
    QCheckBox *checkBoxNsfw;
    QSpacerItem *verticalSpacer_3;
    QWidget *pikpakPage;
    QVBoxLayout *verticalLayout_3;
    QLabel *labelPikPakTitle;
    QFrame *pikPakInfoFrame;
    QVBoxLayout *verticalLayout_9;
    QLabel *labelPikPakUsername;
    QLabel *labelPikPakPassword;
    QLabel *labelPikPakAccessToken;
    QLabel *labelPikPakRefreshToken;
    QPushButton *btnPikPakLogin;
    QSpacerItem *verticalSpacer_2;
    QWidget *downloadPage;
    QVBoxLayout *verticalLayout_5;
    QLabel *labelDownloadTitle;
    QFrame *downloadPathFrame;
    QHBoxLayout *horizontalLayout_3;
    QLineEdit *lineEditDownloadPath;
    QPushButton *btnSelectDownloadPath;
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
        settingsFrame = new QFrame(SettingsPage);
        settingsFrame->setObjectName("settingsFrame");
        settingsFrame->setMinimumSize(QSize(300, 0));
        settingsFrame->setMaximumSize(QSize(300, 16777215));
        settingsFrame->setStyleSheet(QString::fromUtf8("background-color: white;"));
        settingsFrame->setFrameShape(QFrame::Shape::StyledPanel);
        settingsFrame->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayout = new QVBoxLayout(settingsFrame);
        verticalLayout->setObjectName("verticalLayout");
        headerFrame = new QFrame(settingsFrame);
        headerFrame->setObjectName("headerFrame");
        horizontalLayout = new QHBoxLayout(headerFrame);
        horizontalLayout->setObjectName("horizontalLayout");
        btnBack = new QPushButton(headerFrame);
        btnBack->setObjectName("btnBack");
        btnBack->setMinimumSize(QSize(45, 45));
        btnBack->setMaximumSize(QSize(45, 45));
        QFont font;
        font.setBold(false);
        font.setKerning(false);
        btnBack->setFont(font);
        btnBack->setStyleSheet(QString::fromUtf8("QPushButton {\n"
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
        btnBack->setIcon(icon);
        btnBack->setIconSize(QSize(20, 20));
        btnBack->setCheckable(true);

        horizontalLayout->addWidget(btnBack);

        btnSettingsTitle = new QPushButton(headerFrame);
        btnSettingsTitle->setObjectName("btnSettingsTitle");
        btnSettingsTitle->setMinimumSize(QSize(175, 0));
        btnSettingsTitle->setMaximumSize(QSize(175, 16777215));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font1.setPointSize(25);
        font1.setBold(true);
        btnSettingsTitle->setFont(font1);
        btnSettingsTitle->setStyleSheet(QString::fromUtf8("border:none"));

        horizontalLayout->addWidget(btnSettingsTitle);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addWidget(headerFrame);

        menuFrame = new QFrame(settingsFrame);
        menuFrame->setObjectName("menuFrame");
        verticalLayout_2 = new QVBoxLayout(menuFrame);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(-1, 20, -1, -1);
        btnBangumi = new QPushButton(menuFrame);
        buttonGroup = new QButtonGroup(SettingsPage);
        buttonGroup->setObjectName("buttonGroup");
        buttonGroup->addButton(btnBangumi);
        btnBangumi->setObjectName("btnBangumi");
        btnBangumi->setMinimumSize(QSize(0, 50));
        btnBangumi->setMaximumSize(QSize(16777215, 50));
        QFont font2;
        font2.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font2.setPointSize(15);
        font2.setBold(true);
        btnBangumi->setFont(font2);
        btnBangumi->setCheckable(true);

        verticalLayout_2->addWidget(btnBangumi);

        btnPikPak = new QPushButton(menuFrame);
        buttonGroup->addButton(btnPikPak);
        btnPikPak->setObjectName("btnPikPak");
        btnPikPak->setMinimumSize(QSize(0, 50));
        btnPikPak->setMaximumSize(QSize(16777215, 50));
        btnPikPak->setFont(font2);
        btnPikPak->setCheckable(true);

        verticalLayout_2->addWidget(btnPikPak);

        btnDownloadMenu = new QPushButton(menuFrame);
        buttonGroup->addButton(btnDownloadMenu);
        btnDownloadMenu->setObjectName("btnDownloadMenu");
        btnDownloadMenu->setMinimumSize(QSize(0, 50));
        btnDownloadMenu->setMaximumSize(QSize(16777215, 50));
        btnDownloadMenu->setFont(font2);
        btnDownloadMenu->setCheckable(true);

        verticalLayout_2->addWidget(btnDownloadMenu);


        verticalLayout->addWidget(menuFrame);

        verticalSpacer = new QSpacerItem(20, 866, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout_8->addWidget(settingsFrame);

        stackedWidget = new QStackedWidget(SettingsPage);
        stackedWidget->setObjectName("stackedWidget");
        stackedWidget->setStyleSheet(QString::fromUtf8("QStackedWidget > QWidget {\n"
"       background-color: transparent;\n"
"       }"));
        bangumiPage = new QWidget();
        bangumiPage->setObjectName("bangumiPage");
        verticalLayout_7 = new QVBoxLayout(bangumiPage);
        verticalLayout_7->setObjectName("verticalLayout_7");
        labelBangumiTitle = new QLabel(bangumiPage);
        labelBangumiTitle->setObjectName("labelBangumiTitle");
        labelBangumiTitle->setMinimumSize(QSize(0, 0));
        labelBangumiTitle->setMaximumSize(QSize(16777215, 16777215));
        QFont font3;
        font3.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font3.setPointSize(17);
        font3.setBold(true);
        labelBangumiTitle->setFont(font3);

        verticalLayout_7->addWidget(labelBangumiTitle, 0, Qt::AlignmentFlag::AlignLeft);

        authFrame = new QFrame(bangumiPage);
        authFrame->setObjectName("authFrame");
        verticalLayout_4 = new QVBoxLayout(authFrame);
        verticalLayout_4->setObjectName("verticalLayout_4");
        labelAuthSection = new QLabel(authFrame);
        labelAuthSection->setObjectName("labelAuthSection");
        labelAuthSection->setMinimumSize(QSize(0, 30));
        labelAuthSection->setMaximumSize(QSize(16777215, 30));
        QFont font4;
        font4.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font4.setPointSize(13);
        font4.setBold(true);
        labelAuthSection->setFont(font4);

        verticalLayout_4->addWidget(labelAuthSection, 0, Qt::AlignmentFlag::AlignLeft);

        labelUserId = new QLabel(authFrame);
        labelUserId->setObjectName("labelUserId");
        QFont font5;
        font5.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font5.setPointSize(13);
        font5.setBold(false);
        labelUserId->setFont(font5);

        verticalLayout_4->addWidget(labelUserId, 0, Qt::AlignmentFlag::AlignLeft);

        labelAccessToken = new QLabel(authFrame);
        labelAccessToken->setObjectName("labelAccessToken");
        labelAccessToken->setFont(font5);

        verticalLayout_4->addWidget(labelAccessToken, 0, Qt::AlignmentFlag::AlignLeft);

        labelRefreshToken = new QLabel(authFrame);
        labelRefreshToken->setObjectName("labelRefreshToken");
        labelRefreshToken->setFont(font5);

        verticalLayout_4->addWidget(labelRefreshToken, 0, Qt::AlignmentFlag::AlignLeft);

        btnBangumiAuth = new QPushButton(authFrame);
        btnBangumiAuth->setObjectName("btnBangumiAuth");
        btnBangumiAuth->setMinimumSize(QSize(76, 0));
        QFont font6;
        font6.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font6.setPointSize(13);
        btnBangumiAuth->setFont(font6);

        verticalLayout_4->addWidget(btnBangumiAuth, 0, Qt::AlignmentFlag::AlignLeft);

        btnGetCollection = new QPushButton(authFrame);
        btnGetCollection->setObjectName("btnGetCollection");
        btnGetCollection->setMinimumSize(QSize(76, 0));
        btnGetCollection->setFont(font6);

        verticalLayout_4->addWidget(btnGetCollection, 0, Qt::AlignmentFlag::AlignLeft);


        verticalLayout_7->addWidget(authFrame);

        domainFrame = new QFrame(bangumiPage);
        domainFrame->setObjectName("domainFrame");
        horizontalLayout_2 = new QHBoxLayout(domainFrame);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        labelDomainSection = new QLabel(domainFrame);
        labelDomainSection->setObjectName("labelDomainSection");
        labelDomainSection->setMinimumSize(QSize(0, 30));
        labelDomainSection->setMaximumSize(QSize(16777215, 30));
        labelDomainSection->setFont(font4);

        horizontalLayout_2->addWidget(labelDomainSection, 0, Qt::AlignmentFlag::AlignLeft);

        comboBangumiDomain = new QComboBox(domainFrame);
        comboBangumiDomain->addItem(QString());
        comboBangumiDomain->addItem(QString());
        comboBangumiDomain->addItem(QString());
        comboBangumiDomain->setObjectName("comboBangumiDomain");
        comboBangumiDomain->setMinimumSize(QSize(0, 25));
        QFont font7;
        font7.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font7.setPointSize(10);
        comboBangumiDomain->setFont(font7);

        horizontalLayout_2->addWidget(comboBangumiDomain, 0, Qt::AlignmentFlag::AlignLeft);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout_7->addWidget(domainFrame);

        publicDataFrame = new QFrame(bangumiPage);
        publicDataFrame->setObjectName("publicDataFrame");
        publicDataFrame->setFrameShape(QFrame::Shape::StyledPanel);
        publicDataFrame->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayout_6 = new QVBoxLayout(publicDataFrame);
        verticalLayout_6->setObjectName("verticalLayout_6");
        labelPublicDataSection = new QLabel(publicDataFrame);
        labelPublicDataSection->setObjectName("labelPublicDataSection");
        labelPublicDataSection->setMinimumSize(QSize(0, 30));
        labelPublicDataSection->setMaximumSize(QSize(16777215, 30));
        labelPublicDataSection->setFont(font4);

        verticalLayout_6->addWidget(labelPublicDataSection, 0, Qt::AlignmentFlag::AlignLeft);

        publicDataOptionsFrame = new QFrame(publicDataFrame);
        publicDataOptionsFrame->setObjectName("publicDataOptionsFrame");
        publicDataOptionsFrame->setFrameShape(QFrame::Shape::StyledPanel);
        publicDataOptionsFrame->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayout_8 = new QVBoxLayout(publicDataOptionsFrame);
        verticalLayout_8->setObjectName("verticalLayout_8");
        verticalLayout_8->setContentsMargins(0, -1, -1, -1);
        checkBoxAnime = new QCheckBox(publicDataOptionsFrame);
        checkBoxAnime->setObjectName("checkBoxAnime");
        checkBoxAnime->setFont(font7);

        verticalLayout_8->addWidget(checkBoxAnime);

        checkBoxBook = new QCheckBox(publicDataOptionsFrame);
        checkBoxBook->setObjectName("checkBoxBook");
        checkBoxBook->setFont(font7);

        verticalLayout_8->addWidget(checkBoxBook);

        checkBoxGame = new QCheckBox(publicDataOptionsFrame);
        checkBoxGame->setObjectName("checkBoxGame");
        checkBoxGame->setFont(font7);

        verticalLayout_8->addWidget(checkBoxGame);

        publicDataButtonsFrame = new QFrame(publicDataOptionsFrame);
        publicDataButtonsFrame->setObjectName("publicDataButtonsFrame");
        publicDataButtonsFrame->setFrameShape(QFrame::Shape::StyledPanel);
        publicDataButtonsFrame->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_4 = new QHBoxLayout(publicDataButtonsFrame);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalLayout_4->setContentsMargins(0, 0, -1, 5);
        btnDownloadPublic = new QPushButton(publicDataButtonsFrame);
        btnDownloadPublic->setObjectName("btnDownloadPublic");
        btnDownloadPublic->setMinimumSize(QSize(76, 0));
        btnDownloadPublic->setMaximumSize(QSize(76, 16777215));
        btnDownloadPublic->setFont(font6);

        horizontalLayout_4->addWidget(btnDownloadPublic);

        btnDownloadMirror = new QPushButton(publicDataButtonsFrame);
        btnDownloadMirror->setObjectName("btnDownloadMirror");
        btnDownloadMirror->setMinimumSize(QSize(76, 0));
        btnDownloadMirror->setMaximumSize(QSize(76, 16777215));
        btnDownloadMirror->setFont(font6);

        horizontalLayout_4->addWidget(btnDownloadMirror);

        labelDownloadStatus = new QPushButton(publicDataButtonsFrame);
        labelDownloadStatus->setObjectName("labelDownloadStatus");
        labelDownloadStatus->setFont(font6);
        labelDownloadStatus->setStyleSheet(QString::fromUtf8("border: none;"));

        horizontalLayout_4->addWidget(labelDownloadStatus);


        verticalLayout_8->addWidget(publicDataButtonsFrame, 0, Qt::AlignmentFlag::AlignLeft);


        verticalLayout_6->addWidget(publicDataOptionsFrame);


        verticalLayout_7->addWidget(publicDataFrame);

        searchOptionsFrame = new QFrame(bangumiPage);
        searchOptionsFrame->setObjectName("searchOptionsFrame");
        searchOptionsFrame->setFrameShape(QFrame::Shape::StyledPanel);
        searchOptionsFrame->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayout_10 = new QVBoxLayout(searchOptionsFrame);
        verticalLayout_10->setObjectName("verticalLayout_10");
        labelSearchSection = new QLabel(searchOptionsFrame);
        labelSearchSection->setObjectName("labelSearchSection");
        labelSearchSection->setMinimumSize(QSize(0, 30));
        labelSearchSection->setMaximumSize(QSize(16777215, 30));
        labelSearchSection->setFont(font4);

        verticalLayout_10->addWidget(labelSearchSection, 0, Qt::AlignmentFlag::AlignLeft);

        checkBoxNsfw = new QCheckBox(searchOptionsFrame);
        checkBoxNsfw->setObjectName("checkBoxNsfw");
        checkBoxNsfw->setFont(font7);

        verticalLayout_10->addWidget(checkBoxNsfw);


        verticalLayout_7->addWidget(searchOptionsFrame);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_7->addItem(verticalSpacer_3);

        stackedWidget->addWidget(bangumiPage);
        pikpakPage = new QWidget();
        pikpakPage->setObjectName("pikpakPage");
        verticalLayout_3 = new QVBoxLayout(pikpakPage);
        verticalLayout_3->setObjectName("verticalLayout_3");
        labelPikPakTitle = new QLabel(pikpakPage);
        labelPikPakTitle->setObjectName("labelPikPakTitle");
        labelPikPakTitle->setMinimumSize(QSize(0, 0));
        labelPikPakTitle->setMaximumSize(QSize(16777215, 16777215));
        QFont font8;
        font8.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font8.setPointSize(20);
        font8.setBold(true);
        labelPikPakTitle->setFont(font8);

        verticalLayout_3->addWidget(labelPikPakTitle, 0, Qt::AlignmentFlag::AlignLeft);

        pikPakInfoFrame = new QFrame(pikpakPage);
        pikPakInfoFrame->setObjectName("pikPakInfoFrame");
        pikPakInfoFrame->setFrameShape(QFrame::Shape::StyledPanel);
        pikPakInfoFrame->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayout_9 = new QVBoxLayout(pikPakInfoFrame);
        verticalLayout_9->setObjectName("verticalLayout_9");
        labelPikPakUsername = new QLabel(pikPakInfoFrame);
        labelPikPakUsername->setObjectName("labelPikPakUsername");
        labelPikPakUsername->setFont(font5);

        verticalLayout_9->addWidget(labelPikPakUsername, 0, Qt::AlignmentFlag::AlignLeft);

        labelPikPakPassword = new QLabel(pikPakInfoFrame);
        labelPikPakPassword->setObjectName("labelPikPakPassword");
        labelPikPakPassword->setFont(font5);

        verticalLayout_9->addWidget(labelPikPakPassword, 0, Qt::AlignmentFlag::AlignLeft);

        labelPikPakAccessToken = new QLabel(pikPakInfoFrame);
        labelPikPakAccessToken->setObjectName("labelPikPakAccessToken");
        labelPikPakAccessToken->setFont(font6);

        verticalLayout_9->addWidget(labelPikPakAccessToken, 0, Qt::AlignmentFlag::AlignLeft);

        labelPikPakRefreshToken = new QLabel(pikPakInfoFrame);
        labelPikPakRefreshToken->setObjectName("labelPikPakRefreshToken");
        labelPikPakRefreshToken->setFont(font6);

        verticalLayout_9->addWidget(labelPikPakRefreshToken, 0, Qt::AlignmentFlag::AlignLeft);

        btnPikPakLogin = new QPushButton(pikPakInfoFrame);
        btnPikPakLogin->setObjectName("btnPikPakLogin");
        btnPikPakLogin->setMinimumSize(QSize(0, 30));
        btnPikPakLogin->setFont(font5);
        btnPikPakLogin->setStyleSheet(QString::fromUtf8(""));

        verticalLayout_9->addWidget(btnPikPakLogin, 0, Qt::AlignmentFlag::AlignLeft);


        verticalLayout_3->addWidget(pikPakInfoFrame);

        verticalSpacer_2 = new QSpacerItem(20, 759, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_2);

        stackedWidget->addWidget(pikpakPage);
        downloadPage = new QWidget();
        downloadPage->setObjectName("downloadPage");
        verticalLayout_5 = new QVBoxLayout(downloadPage);
        verticalLayout_5->setObjectName("verticalLayout_5");
        labelDownloadTitle = new QLabel(downloadPage);
        labelDownloadTitle->setObjectName("labelDownloadTitle");
        labelDownloadTitle->setMinimumSize(QSize(0, 0));
        labelDownloadTitle->setMaximumSize(QSize(16777215, 16777215));
        labelDownloadTitle->setFont(font3);

        verticalLayout_5->addWidget(labelDownloadTitle, 0, Qt::AlignmentFlag::AlignLeft);

        downloadPathFrame = new QFrame(downloadPage);
        downloadPathFrame->setObjectName("downloadPathFrame");
        downloadPathFrame->setFrameShape(QFrame::Shape::StyledPanel);
        downloadPathFrame->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_3 = new QHBoxLayout(downloadPathFrame);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        lineEditDownloadPath = new QLineEdit(downloadPathFrame);
        lineEditDownloadPath->setObjectName("lineEditDownloadPath");
        lineEditDownloadPath->setReadOnly(true);

        horizontalLayout_3->addWidget(lineEditDownloadPath);

        btnSelectDownloadPath = new QPushButton(downloadPathFrame);
        btnSelectDownloadPath->setObjectName("btnSelectDownloadPath");
        btnSelectDownloadPath->setMinimumSize(QSize(0, 30));
        btnSelectDownloadPath->setFont(font5);
        btnSelectDownloadPath->setStyleSheet(QString::fromUtf8(""));

        horizontalLayout_3->addWidget(btnSelectDownloadPath);


        verticalLayout_5->addWidget(downloadPathFrame);

        verticalSpacer_4 = new QSpacerItem(20, 829, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_5->addItem(verticalSpacer_4);

        stackedWidget->addWidget(downloadPage);

        horizontalLayout_8->addWidget(stackedWidget);


        retranslateUi(SettingsPage);

        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(SettingsPage);
    } // setupUi

    void retranslateUi(QWidget *SettingsPage) const
    {
        btnBack->setText(QString());
        btnSettingsTitle->setText(QCoreApplication::translate("SettingsPage", "\350\256\276\347\275\256", nullptr));
        btnBangumi->setText(QCoreApplication::translate("SettingsPage", "Bangumi", nullptr));
        btnPikPak->setText(QCoreApplication::translate("SettingsPage", "PikPak", nullptr));
        btnDownloadMenu->setText(QCoreApplication::translate("SettingsPage", "\344\270\213\350\275\275", nullptr));
        labelBangumiTitle->setText(QCoreApplication::translate("SettingsPage", "Bangumi", nullptr));
        labelAuthSection->setText(QCoreApplication::translate("SettingsPage", "Bangumi \346\216\210\346\235\203", nullptr));
        labelUserId->setText(QString());
        labelAccessToken->setText(QString());
        labelRefreshToken->setText(QString());
        btnBangumiAuth->setText(QCoreApplication::translate("SettingsPage", "\345\274\200\345\247\213\346\216\210\346\235\203", nullptr));
        btnGetCollection->setText(QCoreApplication::translate("SettingsPage", "\350\216\267\345\217\226\346\224\266\350\227\217", nullptr));
        labelDomainSection->setText(QCoreApplication::translate("SettingsPage", "Bangumi \345\237\237\345\220\215 \357\274\232", nullptr));
        comboBangumiDomain->setItemText(0, QCoreApplication::translate("SettingsPage", "bangumi.tv", nullptr));
        comboBangumiDomain->setItemText(1, QCoreApplication::translate("SettingsPage", "bgm.tv", nullptr));
        comboBangumiDomain->setItemText(2, QCoreApplication::translate("SettingsPage", "chii.in", nullptr));

        labelPublicDataSection->setText(QCoreApplication::translate("SettingsPage", "Bangumi \345\205\254\345\205\261\346\225\260\346\215\256", nullptr));
        checkBoxAnime->setText(QCoreApplication::translate("SettingsPage", "\345\212\250\347\224\273", nullptr));
        checkBoxBook->setText(QCoreApplication::translate("SettingsPage", "\344\271\246\347\261\215", nullptr));
        checkBoxGame->setText(QCoreApplication::translate("SettingsPage", "\346\270\270\346\210\217", nullptr));
        btnDownloadPublic->setText(QCoreApplication::translate("SettingsPage", "\344\270\213\350\275\275", nullptr));
        btnDownloadMirror->setText(QCoreApplication::translate("SettingsPage", "\351\225\234\345\203\217", nullptr));
        labelDownloadStatus->setText(QString());
        labelSearchSection->setText(QCoreApplication::translate("SettingsPage", "Bangumi \346\220\234\347\264\242", nullptr));
        checkBoxNsfw->setText(QCoreApplication::translate("SettingsPage", "NSFW \345\206\205\345\256\271", nullptr));
        labelPikPakTitle->setText(QCoreApplication::translate("SettingsPage", "PikPak \347\231\273\345\275\225", nullptr));
        labelPikPakUsername->setText(QString());
        labelPikPakPassword->setText(QString());
        labelPikPakAccessToken->setText(QString());
        labelPikPakRefreshToken->setText(QString());
        btnPikPakLogin->setText(QCoreApplication::translate("SettingsPage", "\345\274\200\345\247\213\347\231\273\345\275\225", nullptr));
        labelDownloadTitle->setText(QCoreApplication::translate("SettingsPage", "\344\270\213\350\275\275\350\267\257\345\276\204", nullptr));
        btnSelectDownloadPath->setText(QCoreApplication::translate("SettingsPage", "\351\200\211\346\213\251\350\267\257\345\276\204", nullptr));
        (void)SettingsPage;
    } // retranslateUi

};

namespace Ui {
    class SettingsPage: public Ui_SettingsPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // SETTINGS_UI_H
