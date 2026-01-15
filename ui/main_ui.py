from PySide6.QtGui import QFont, QIcon
from PySide6.QtCore import QCoreApplication, QMetaObject, QSize, Qt
from PySide6.QtWidgets import QButtonGroup, QFrame, QGridLayout,QHBoxLayout, QLabel, QLineEdit, QPushButton, QSizePolicy, QSpacerItem, QStackedWidget,QVBoxLayout, QWidget

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(1525, 942)
        MainWindow.setWindowFlags(Qt.FramelessWindowHint)
        self.centralwidget = QWidget(MainWindow)
        self.centralwidget.setObjectName(u"centralwidget")
        self.verticalLayout = QVBoxLayout(self.centralwidget)
        self.verticalLayout.setSpacing(0)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.verticalLayout.setContentsMargins(0, 0, 0, 0)
        self.titlebar_frame = QFrame(self.centralwidget)
        self.titlebar_frame.setObjectName(u"titlebar_frame")
        self.titlebar_frame.setMinimumSize(QSize(0, 25))
        self.titlebar_frame.setMaximumSize(QSize(16777215, 25))
        self.titlebar_frame.setStyleSheet(u"background-color: rgb(255, 255, 255);")
        self.titlebar_frame.setFrameShape(QFrame.StyledPanel)
        self.titlebar_frame.setFrameShadow(QFrame.Raised)
        self.horizontalLayout = QHBoxLayout(self.titlebar_frame)
        self.horizontalLayout.setSpacing(0)
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)
        self.horizontalSpacer_5 = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)
        self.horizontalLayout.addItem(self.horizontalSpacer_5)
        self.pushButton = QPushButton(self.titlebar_frame)
        self.pushButton.setObjectName(u"pushButton")
        self.pushButton.setMinimumSize(QSize(45, 25))
        self.pushButton.setMaximumSize(QSize(45, 25))
        self.pushButton.setStyleSheet(u"QPushButton {\n"
"    border: none\n"
"}\n"
"QPushButton:hover {\n"
"    background-color:rgb(229, 229, 229)\n"
"}")
        icon = QIcon()
        icon.addFile(u"icons/title3.png", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        self.pushButton.setIcon(icon)
        self.horizontalLayout.addWidget(self.pushButton)
        self.pushButton_8 = QPushButton(self.titlebar_frame)
        self.pushButton_8.setObjectName(u"pushButton_8")
        self.pushButton_8.setMinimumSize(QSize(45, 25))
        self.pushButton_8.setMaximumSize(QSize(45, 25))
        self.pushButton_8.setStyleSheet(u"QPushButton {\n"
"    border: none\n"
"}\n"
"QPushButton:hover {\n"
"    background-color:rgb(229, 229, 229)\n"
"}")
        icon1 = QIcon()
        icon1.addFile(u"icons/title2.png", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        self.pushButton_8.setIcon(icon1)
        self.pushButton_8.setIconSize(QSize(13, 13))
        self.horizontalLayout.addWidget(self.pushButton_8, 0, Qt.AlignRight)
        self.pushButton_2 = QPushButton(self.titlebar_frame)
        self.pushButton_2.setObjectName(u"pushButton_2")
        self.pushButton_2.setMinimumSize(QSize(45, 25))
        self.pushButton_2.setMaximumSize(QSize(45, 25))
        self.pushButton_2.setStyleSheet(u"QPushButton {\n"
"    border: none\n"
"}\n"
"QPushButton:hover {\n"
"    background-color:rgb(232, 17, 35)\n"
"}")
        icon2 = QIcon()
        icon2.addFile(u"icons/title1.png", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        self.pushButton_2.setIcon(icon2)
        self.pushButton_2.setIconSize(QSize(14, 14))
        self.horizontalLayout.addWidget(self.pushButton_2, 0, Qt.AlignRight)
        self.verticalLayout.addWidget(self.titlebar_frame)
        self.main_stackedWidget = QStackedWidget(self.centralwidget)
        self.main_stackedWidget.setObjectName(u"main_stackedWidget")
        self.main_stackedWidget.setStyleSheet(u"")
        self.showmain_page = QWidget()
        self.showmain_page.setObjectName(u"showmain_page")
        self.horizontalLayout_6 = QHBoxLayout(self.showmain_page)
        self.horizontalLayout_6.setSpacing(0)
        self.horizontalLayout_6.setObjectName(u"horizontalLayout_6")
        self.horizontalLayout_6.setContentsMargins(0, 0, 0, 0)
        self.toolbar_frame = QFrame(self.showmain_page)
        self.toolbar_frame.setObjectName(u"toolbar_frame")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.toolbar_frame.sizePolicy().hasHeightForWidth())
        self.toolbar_frame.setSizePolicy(sizePolicy)
        self.toolbar_frame.setMinimumSize(QSize(80, 0))
        self.toolbar_frame.setMaximumSize(QSize(80, 16777215))
        self.toolbar_frame.setStyleSheet(u"background-color: rgb(242, 236, 244);")
        self.toolbar_frame.setFrameShape(QFrame.NoFrame)
        self.verticalLayout_3 = QVBoxLayout(self.toolbar_frame)
        self.verticalLayout_3.setSpacing(0)
        self.verticalLayout_3.setObjectName(u"verticalLayout_3")
        self.verticalLayout_3.setContentsMargins(0, 9, 0, 9)
        self.searchinto_frame = QFrame(self.toolbar_frame)
        self.searchinto_frame.setObjectName(u"searchinto_frame")
        self.searchinto_frame.setMinimumSize(QSize(80, 100))
        self.searchinto_frame.setMaximumSize(QSize(80, 80))
        self.searchinto_frame.setFrameShape(QFrame.NoFrame)
        self.verticalLayout_5 = QVBoxLayout(self.searchinto_frame)
        self.verticalLayout_5.setSpacing(6)
        self.verticalLayout_5.setObjectName(u"verticalLayout_5")
        self.verticalLayout_5.setContentsMargins(-1, 9, 9, 0)
        self.search_Button = QPushButton(self.searchinto_frame)
        self.search_Button.setObjectName(u"search_Button")
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Minimum)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.search_Button.sizePolicy().hasHeightForWidth())
        self.search_Button.setSizePolicy(sizePolicy1)
        self.search_Button.setMinimumSize(QSize(50, 50))
        self.search_Button.setMaximumSize(QSize(50, 50))
        self.search_Button.setStyleSheet(u"QPushButton {\n"
"	background-color: rgb(233, 221, 255);\n"
"    border: none;\n"
"	border-radius:15px\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgb(221, 208, 246);\n"
"}")
        icon3 = QIcon()
        icon3.addFile(u"icons/search.png", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        self.search_Button.setIcon(icon3)
        self.search_Button.setIconSize(QSize(20, 20))
        self.search_Button.setCheckable(True)
        self.verticalLayout_5.addWidget(self.search_Button, 0, Qt.AlignHCenter)
        self.verticalLayout_3.addWidget(self.searchinto_frame)
        self.acgnselect_frame = QFrame(self.toolbar_frame)
        self.acgnselect_frame.setObjectName(u"acgnselect_frame")
        self.acgnselect_frame.setMinimumSize(QSize(80, 0))
        self.acgnselect_frame.setStyleSheet(u"background-color: rgb(242, 236, 244);")
        self.acgnselect_frame.setFrameShape(QFrame.NoFrame)
        self.verticalLayout_4 = QVBoxLayout(self.acgnselect_frame)
        self.verticalLayout_4.setSpacing(40)
        self.verticalLayout_4.setObjectName(u"verticalLayout_4")
        self.verticalLayout_4.setContentsMargins(-1, 9, -1, 9)
        self.animation_Button = QPushButton(self.acgnselect_frame)
        self.buttonGroup = QButtonGroup(MainWindow)
        self.buttonGroup.setObjectName(u"buttonGroup")
        self.buttonGroup.addButton(self.animation_Button)
        self.animation_Button.setObjectName(u"animation_Button")
        self.animation_Button.setMinimumSize(QSize(0, 50))
        self.animation_Button.setMaximumSize(QSize(80, 50))
        font = QFont()
        font.setFamilies([u"\u5fae\u8f6f\u96c5\u9ed1"])
        font.setPointSize(15)
        font.setBold(True)
        self.animation_Button.setFont(font)
        self.animation_Button.setStyleSheet(u"QPushButton {\n"
"	background-color: rgb(242, 236, 244);\n"
"    border: none;\n"
"	border-radius:15px\n"
"}\n"
"\n"
"QPushButton:checked {\n"
"    background-color: rgb(233, 221, 255);\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgb(225, 219, 228);\n"
"}\n"
"\n"
"QPushButton:checked:hover {\n"
"    background-color: rgb(216, 207, 232);\n"
"}")
        self.animation_Button.setCheckable(True)
        self.verticalLayout_4.addWidget(self.animation_Button)
        self.novel_Button = QPushButton(self.acgnselect_frame)
        self.buttonGroup.addButton(self.novel_Button)
        self.novel_Button.setObjectName(u"novel_Button")
        self.novel_Button.setMinimumSize(QSize(0, 50))
        self.novel_Button.setMaximumSize(QSize(80, 50))
        self.novel_Button.setFont(font)
        self.novel_Button.setStyleSheet(u"QPushButton {\n"
"	background-color: rgb(242, 236, 244);\n"
"    border: none;\n"
"	border-radius:15px\n"
"}\n"
"\n"
"QPushButton:checked {\n"
"    background-color: rgb(233, 221, 255);\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgb(225, 219, 228);\n"
"}\n"
"\n"
"QPushButton:checked:hover {\n"
"    background-color: rgb(216, 207, 232);\n"
"}")
        self.novel_Button.setCheckable(True)
        self.verticalLayout_4.addWidget(self.novel_Button)
        self.game_Button = QPushButton(self.acgnselect_frame)
        self.buttonGroup.addButton(self.game_Button)
        self.game_Button.setObjectName(u"game_Button")
        self.game_Button.setMinimumSize(QSize(0, 50))
        self.game_Button.setMaximumSize(QSize(80, 50))
        self.game_Button.setFont(font)
        self.game_Button.setStyleSheet(u"QPushButton {\n"
"	background-color: rgb(242, 236, 244);\n"
"    border: none;\n"
"	border-radius:15px\n"
"}\n"
"\n"
"QPushButton:checked {\n"
"    background-color: rgb(233, 221, 255);\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgb(225, 219, 228);\n"
"}\n"
"\n"
"QPushButton:checked:hover {\n"
"    background-color: rgb(216, 207, 232);\n"
"}")
        self.game_Button.setCheckable(True)
        self.verticalLayout_4.addWidget(self.game_Button)
        self.comic_Button = QPushButton(self.acgnselect_frame)
        self.buttonGroup.addButton(self.comic_Button)
        self.comic_Button.setObjectName(u"comic_Button")
        self.comic_Button.setMinimumSize(QSize(0, 50))
        self.comic_Button.setMaximumSize(QSize(80, 50))
        self.comic_Button.setFont(font)
        self.comic_Button.setStyleSheet(u"QPushButton {\n"
"	background-color: rgb(242, 236, 244);\n"
"    border: none;\n"
"	border-radius:15px\n"
"}\n"
"\n"
"QPushButton:checked {\n"
"    background-color: rgb(233, 221, 255);\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgb(225, 219, 228);\n"
"}\n"
"\n"
"QPushButton:checked:hover {\n"
"    background-color: rgb(216, 207, 232);\n"
"}")
        self.comic_Button.setCheckable(True)
        self.verticalLayout_4.addWidget(self.comic_Button)
        self.pushButton_9 = QPushButton(self.acgnselect_frame)
        self.buttonGroup.addButton(self.pushButton_9)
        self.pushButton_9.setObjectName(u"pushButton_9")
        self.pushButton_9.setMinimumSize(QSize(0, 50))
        self.pushButton_9.setMaximumSize(QSize(80, 50))
        self.pushButton_9.setFont(font)
        self.pushButton_9.setStyleSheet(u"QPushButton {\n"
"	background-color: rgb(242, 236, 244);\n"
"    border: none;\n"
"	border-radius:15px\n"
"}\n"
"\n"
"QPushButton:checked {\n"
"    background-color: rgb(233, 221, 255);\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgb(225, 219, 228);\n"
"}\n"
"\n"
"QPushButton:checked:hover {\n"
"    background-color: rgb(216, 207, 232);\n"
"}")
        self.pushButton_9.setCheckable(True)
        self.verticalLayout_4.addWidget(self.pushButton_9)
        self.verticalLayout_3.addWidget(self.acgnselect_frame)
        self.verticalSpacer = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)
        self.verticalLayout_3.addItem(self.verticalSpacer)
        self.settings_Button = QPushButton(self.toolbar_frame)
        self.settings_Button.setObjectName(u"settings_Button")
        self.settings_Button.setMinimumSize(QSize(50, 50))
        self.settings_Button.setMaximumSize(QSize(50, 50))
        self.settings_Button.setStyleSheet(u"QPushButton {\n"
"	background-color: rgb(242, 236, 244);\n"
"    border: none;\n"
"	border-radius:15px\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgb(225, 219, 228);\n"
"}")
        icon4 = QIcon()
        icon4.addFile(u"icons/settings.png", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        self.settings_Button.setIcon(icon4)
        self.settings_Button.setCheckable(False)
        self.verticalLayout_3.addWidget(self.settings_Button, 0, Qt.AlignHCenter)
        self.horizontalLayout_6.addWidget(self.toolbar_frame)
        self.showmain_stackedWidget = QStackedWidget(self.showmain_page)
        self.showmain_stackedWidget.setObjectName(u"showmain_stackedWidget")
        self.showmain_stackedWidgetPage1 = QWidget()
        self.showmain_stackedWidgetPage1.setObjectName(u"showmain_stackedWidgetPage1")
        self.verticalLayout_2 = QVBoxLayout(self.showmain_stackedWidgetPage1)
        self.verticalLayout_2.setSpacing(0)
        self.verticalLayout_2.setObjectName(u"verticalLayout_2")
        self.verticalLayout_2.setContentsMargins(0, 0, 0, 0)
        self.header_frame = QFrame(self.showmain_stackedWidgetPage1)
        self.header_frame.setObjectName(u"header_frame")
        self.header_frame.setMinimumSize(QSize(0, 100))
        self.header_frame.setMaximumSize(QSize(16777215, 100))
        self.header_frame.setStyleSheet(u"background-color: white;")
        self.header_frame.setFrameShape(QFrame.NoFrame)
        self.gridLayout = QGridLayout(self.header_frame)
        self.gridLayout.setObjectName(u"gridLayout")
        self.statu_frame = QFrame(self.header_frame)
        self.statu_frame.setObjectName(u"statu_frame")
        self.statu_frame.setMinimumSize(QSize(0, 60))
        self.statu_frame.setMaximumSize(QSize(16777215, 50))
        self.statu_frame.setFrameShape(QFrame.NoFrame)
        self.horizontalLayout_2 = QHBoxLayout(self.statu_frame)
        self.horizontalLayout_2.setSpacing(0)
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.horizontalLayout_2.setContentsMargins(0, 0, 20, 0)
        self.pushButton_3 = QPushButton(self.statu_frame)
        self.buttonGroup_2 = QButtonGroup(MainWindow)
        self.buttonGroup_2.setObjectName(u"buttonGroup_2")
        self.buttonGroup_2.addButton(self.pushButton_3)
        self.pushButton_3.setObjectName(u"pushButton_3")
        self.pushButton_3.setMinimumSize(QSize(80, 40))
        self.pushButton_3.setMaximumSize(QSize(80, 40))
        self.pushButton_3.setFont(font)
        self.pushButton_3.setStyleSheet(u"QPushButton {\n"
"	border-bottom-color: rgb(24, 59, 255);\n"
"	background-color: white;\n"
"	border:none;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgb(237, 237, 238);\n"
"}\n"
"\n"
"QPushButton:checked {\n"
"	background-color: #ffffff;\n"
"	border-bottom: 3px solid rgb(103, 79, 165); \n"
"}\n"
"\n"
"QPushButton:checked:hover {\n"
"    background-color: rgb(237, 237, 238);\n"
"}")
        self.pushButton_3.setCheckable(True)
        self.horizontalLayout_2.addWidget(self.pushButton_3)
        self.pushButton_4 = QPushButton(self.statu_frame)
        self.buttonGroup_2.addButton(self.pushButton_4)
        self.pushButton_4.setObjectName(u"pushButton_4")
        self.pushButton_4.setMinimumSize(QSize(80, 40))
        self.pushButton_4.setMaximumSize(QSize(80, 40))
        self.pushButton_4.setFont(font)
        self.pushButton_4.setStyleSheet(u"QPushButton {\n"
"	border-bottom-color: rgb(24, 59, 255);\n"
"	background-color: white;\n"
"	border:none;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgb(237, 237, 238);\n"
"}\n"
"\n"
"QPushButton:checked {\n"
"	background-color: #ffffff;\n"
"	border-bottom: 3px solid rgb(103, 79, 165); \n"
"}\n"
"\n"
"QPushButton:checked:hover {\n"
"    background-color: rgb(237, 237, 238);\n"
"}")
        self.pushButton_4.setCheckable(True)
        self.horizontalLayout_2.addWidget(self.pushButton_4)
        self.pushButton_5 = QPushButton(self.statu_frame)
        self.buttonGroup_2.addButton(self.pushButton_5)
        self.pushButton_5.setObjectName(u"pushButton_5")
        self.pushButton_5.setMinimumSize(QSize(80, 40))
        self.pushButton_5.setMaximumSize(QSize(80, 40))
        self.pushButton_5.setFont(font)
        self.pushButton_5.setStyleSheet(u"QPushButton {\n"
"	border-bottom-color: rgb(24, 59, 255);\n"
"	background-color: white;\n"
"	border:none;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgb(237, 237, 238);\n"
"}\n"
"\n"
"QPushButton:checked {\n"
"	background-color: #ffffff;\n"
"	border-bottom: 3px solid rgb(103, 79, 165); \n"
"}\n"
"\n"
"QPushButton:checked:hover {\n"
"    background-color: rgb(237, 237, 238);\n"
"}")
        self.pushButton_5.setCheckable(True)
        self.horizontalLayout_2.addWidget(self.pushButton_5)
        self.pushButton_6 = QPushButton(self.statu_frame)
        self.buttonGroup_2.addButton(self.pushButton_6)
        self.pushButton_6.setObjectName(u"pushButton_6")
        self.pushButton_6.setMinimumSize(QSize(80, 40))
        self.pushButton_6.setMaximumSize(QSize(80, 40))
        self.pushButton_6.setFont(font)
        self.pushButton_6.setStyleSheet(u"QPushButton {\n"
"	border-bottom-color: rgb(24, 59, 255);\n"
"	background-color: white;\n"
"	border:none;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgb(237, 237, 238);\n"
"}\n"
"\n"
"QPushButton:checked {\n"
"	background-color: #ffffff;\n"
"	border-bottom: 3px solid rgb(103, 79, 165); \n"
"}\n"
"\n"
"QPushButton:checked:hover {\n"
"    background-color: rgb(237, 237, 238);\n"
"}")
        self.pushButton_6.setCheckable(True)
        self.horizontalLayout_2.addWidget(self.pushButton_6)
        self.pushButton_7 = QPushButton(self.statu_frame)
        self.buttonGroup_2.addButton(self.pushButton_7)
        self.pushButton_7.setObjectName(u"pushButton_7")
        self.pushButton_7.setMinimumSize(QSize(80, 40))
        self.pushButton_7.setMaximumSize(QSize(80, 40))
        self.pushButton_7.setFont(font)
        self.pushButton_7.setStyleSheet(u"QPushButton {\n"
"	border-bottom-color: rgb(24, 59, 255);\n"
"	background-color: white;\n"
"	border:none;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgb(237, 237, 238);\n"
"}\n"
"\n"
"QPushButton:checked {\n"
"	background-color: #ffffff;\n"
"	border-bottom: 3px solid rgb(103, 79, 165); \n"
"}\n"
"\n"
"QPushButton:checked:hover {\n"
"    background-color: rgb(237, 237, 238);\n"
"}")
        self.pushButton_7.setCheckable(True)
        self.horizontalLayout_2.addWidget(self.pushButton_7, 0, Qt.AlignVCenter)
        self.gridLayout.addWidget(self.statu_frame, 2, 0, 4, 1)
        self.project_Button = QPushButton(self.header_frame)
        self.project_Button.setObjectName(u"project_Button")
        self.project_Button.setMinimumSize(QSize(105, 35))
        self.project_Button.setMaximumSize(QSize(16777215, 25))
        font1 = QFont()
        font1.setFamilies([u"\u5fae\u8f6f\u96c5\u9ed1"])
        font1.setPointSize(25)
        font1.setBold(True)
        self.project_Button.setFont(font1)
        self.project_Button.setStyleSheet(u"QPushButton {\n"
"    border: none;\n"
"}\n"
"")
        self.gridLayout.addWidget(self.project_Button, 0, 0, 1, 1, Qt.AlignLeft)
        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)
        self.gridLayout.addItem(self.horizontalSpacer, 4, 2, 1, 1)
        self.searchlist_lineEdit = QLineEdit(self.header_frame)
        self.searchlist_lineEdit.setObjectName(u"searchlist_lineEdit")
        self.searchlist_lineEdit.setMinimumSize(QSize(200, 25))
        self.searchlist_lineEdit.setMaximumSize(QSize(200, 25))
        self.searchlist_lineEdit.setContextMenuPolicy(Qt.NoContextMenu)
        self.searchlist_lineEdit.setStyleSheet(u"")
        self.gridLayout.addWidget(self.searchlist_lineEdit, 4, 4, 1, 1)
        self.refresh_Button = QPushButton(self.header_frame)
        self.refresh_Button.setObjectName(u"refresh_Button")
        self.refresh_Button.setMinimumSize(QSize(50, 50))
        self.refresh_Button.setMaximumSize(QSize(50, 50))
        self.refresh_Button.setStyleSheet(u"QPushButton {\n"
"	border-bottom-color: rgb(24, 59, 255);\n"
"	background-color: white;\n"
"	border:none;\n"
"	border-radius:25px\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgb(237, 237, 238);\n"
"}\n"
"\n"
"QPushButton:checked:hover {\n"
"    background-color: rgb(237, 237, 238);\n"
"}")
        icon5 = QIcon()
        icon5.addFile(u"icons/refresh.png", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        self.refresh_Button.setIcon(icon5)
        self.refresh_Button.setIconSize(QSize(20, 20))
        self.gridLayout.addWidget(self.refresh_Button, 3, 1, 1, 1)
        self.verticalLayout_2.addWidget(self.header_frame)
        self.animationdata_frame = QFrame(self.showmain_stackedWidgetPage1)
        self.animationdata_frame.setObjectName(u"animationdata_frame")
        self.animationdata_frame.setStyleSheet(u"background-color: white;")
        self.animationdata_frame.setFrameShape(QFrame.NoFrame)
        self.gridLayout_2 = QGridLayout(self.animationdata_frame)
        self.gridLayout_2.setObjectName(u"gridLayout_2")
        self.gridLayout_2.setHorizontalSpacing(35)
        self.gridLayout_2.setVerticalSpacing(18)
        self.gridLayout_2.setContentsMargins(0, 10, 0, 10)
        self.animationdata_frame_1 = QFrame(self.animationdata_frame)
        self.animationdata_frame_1.setObjectName(u"animationdata_frame_1")
        self.animationdata_frame_1.setMinimumSize(QSize(420, 170))
        self.animationdata_frame_1.setMaximumSize(QSize(420, 170))
        self.animationdata_frame_1.setStyleSheet(u"QFrame{\n"
"	background-color: rgb(242, 236, 244);\n"
"	border-radius: 15px;\n"
"}")
        self.animationdata_frame_1.setFrameShape(QFrame.StyledPanel)
        self.animationdata_frame_1.setFrameShadow(QFrame.Raised)
        self.horizontalLayout_frame_1 = QHBoxLayout(self.animationdata_frame_1)
        self.horizontalLayout_frame_1.setSpacing(0)
        self.horizontalLayout_frame_1.setObjectName(u"horizontalLayout_frame_1")
        self.horizontalLayout_frame_1.setContentsMargins(0, 0, 0, 0)
        self.cover_label_1 = QLabel(self.animationdata_frame_1)
        self.cover_label_1.setObjectName(u"cover_label_1")
        self.cover_label_1.setMinimumSize(QSize(125, 170))
        self.cover_label_1.setMaximumSize(QSize(80, 170))
        self.cover_label_1.setStyleSheet(u"background-color: rgb(242, 236, 244);\n"
"border-top-left-radius: 15px;\n"
"border-bottom-left-radius: 15px;")
        self.cover_label_1.setAlignment(Qt.AlignCenter)
        self.horizontalLayout_frame_1.addWidget(self.cover_label_1)
        self.verticalLayout_info_1 = QVBoxLayout()
        self.verticalLayout_info_1.setSpacing(8)
        self.verticalLayout_info_1.setObjectName(u"verticalLayout_info_1")
        self.verticalLayout_info_1.setContentsMargins(5, 5, -1, -1)
        self.title_label_1 = QLabel(self.animationdata_frame_1)
        self.title_label_1.setObjectName(u"title_label_1")
        self.title_label_1.setMinimumSize(QSize(0, 30))
        self.title_label_1.setMaximumSize(QSize(16777215, 30))
        font2 = QFont()
        font2.setFamilies([u"\u5fae\u8f6f\u96c5\u9ed1"])
        font2.setPointSize(13)
        font2.setBold(False)
        self.title_label_1.setFont(font2)
        self.title_label_1.setStyleSheet(u"")
        self.title_label_1.setWordWrap(False)
        self.verticalLayout_info_1.addWidget(self.title_label_1)
        self.progress_label_1 = QLabel(self.animationdata_frame_1)
        self.progress_label_1.setObjectName(u"progress_label_1")
        font3 = QFont()
        font3.setFamilies([u"\u5fae\u8f6f\u96c5\u9ed1"])
        font3.setPointSize(10)
        font3.setBold(False)
        self.progress_label_1.setFont(font3)
        self.progress_label_1.setStyleSheet(u"")
        self.verticalLayout_info_1.addWidget(self.progress_label_1)
        self.verticalSpacer_info_1 = QSpacerItem(20, 13, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)
        self.verticalLayout_info_1.addItem(self.verticalSpacer_info_1)
        self.episode_horizontalLayout = QHBoxLayout()
        self.episode_horizontalLayout.setSpacing(0)
        self.episode_horizontalLayout.setObjectName(u"episode_horizontalLayout")
        self.episode_horizontalLayout.setContentsMargins(-1, 0, 15, 15)
        self.horizontalSpacer_4 = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)
        self.episode_horizontalLayout.addItem(self.horizontalSpacer_4)
        self.pushButton_16 = QPushButton(self.animationdata_frame_1)
        self.pushButton_16.setObjectName(u"pushButton_16")
        self.pushButton_16.setMinimumSize(QSize(40, 40))
        self.pushButton_16.setMaximumSize(QSize(40, 25))
        self.pushButton_16.setStyleSheet(u"QPushButton {\n"
"	background-color: rgb(242, 236, 244);\n"
"	border:none;\n"
"	border-radius:20px\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgb(216, 207, 232);\n"
"	border:none;\n"
"	border-radius:20px\n"
"}")
        icon6 = QIcon()
        icon6.addFile(u"icons/more.png", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        self.pushButton_16.setIcon(icon6)
        self.episode_horizontalLayout.addWidget(self.pushButton_16)
        self.episode_pushButton_1 = QPushButton(self.animationdata_frame_1)
        self.episode_pushButton_1.setObjectName(u"episode_pushButton_1")
        self.episode_pushButton_1.setMinimumSize(QSize(60, 40))
        self.episode_pushButton_1.setMaximumSize(QSize(60, 25))
        font4 = QFont()
        font4.setFamilies([u"\u5fae\u8f6f\u96c5\u9ed1"])
        font4.setPointSize(10)
        self.episode_pushButton_1.setFont(font4)
        self.episode_pushButton_1.setStyleSheet(u"QPushButton {\n"
"	background-color: rgb(242, 236, 244);\n"
"	border:none;\n"
"	border-radius:20px\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgb(216, 207, 232);\n"
"	border:none;\n"
"	border-radius:20px\n"
"}")
        self.episode_horizontalLayout.addWidget(self.episode_pushButton_1, 0, Qt.AlignRight)
        self.verticalLayout_info_1.addLayout(self.episode_horizontalLayout)
        self.horizontalLayout_frame_1.addLayout(self.verticalLayout_info_1)
        self.gridLayout_2.addWidget(self.animationdata_frame_1, 0, 0, 1, 1)
        self.verticalLayout_2.addWidget(self.animationdata_frame)
        self.page_frame = QFrame(self.showmain_stackedWidgetPage1)
        self.page_frame.setObjectName(u"page_frame")
        self.page_frame.setMinimumSize(QSize(0, 45))
        self.page_frame.setMaximumSize(QSize(16777215, 45))
        self.page_frame.setStyleSheet(u"background-color: white;")
        self.page_frame.setFrameShape(QFrame.NoFrame)
        self.horizontalLayout_3 = QHBoxLayout(self.page_frame)
        self.horizontalLayout_3.setObjectName(u"horizontalLayout_3")
        self.horizontalSpacer_3 = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)
        self.horizontalLayout_3.addItem(self.horizontalSpacer_3)
        self.previous_Button = QPushButton(self.page_frame)
        self.previous_Button.setObjectName(u"previous_Button")
        self.previous_Button.setStyleSheet(u"")
        self.horizontalLayout_3.addWidget(self.previous_Button)
        self.pages_Button = QPushButton(self.page_frame)
        self.pages_Button.setObjectName(u"pages_Button")
        self.horizontalLayout_3.addWidget(self.pages_Button)
        self.next_Button = QPushButton(self.page_frame)
        self.next_Button.setObjectName(u"next_Button")
        self.horizontalLayout_3.addWidget(self.next_Button)
        self.horizontalSpacer_2 = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)
        self.horizontalLayout_3.addItem(self.horizontalSpacer_2)
        self.verticalLayout_2.addWidget(self.page_frame)
        self.showmain_stackedWidget.addWidget(self.showmain_stackedWidgetPage1)
        self.horizontalLayout_6.addWidget(self.showmain_stackedWidget)
        self.main_stackedWidget.addWidget(self.showmain_page)
        self.verticalLayout.addWidget(self.main_stackedWidget)
        MainWindow.setCentralWidget(self.centralwidget)
        self.retranslateUi(MainWindow)
        self.main_stackedWidget.setCurrentIndex(0)
        QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        self.pushButton.setText("")
        self.pushButton_8.setText("")
        self.pushButton_2.setText("")
        self.animation_Button.setText(QCoreApplication.translate("MainWindow", u"\u52a8\u753b", None))
        self.novel_Button.setText(QCoreApplication.translate("MainWindow", u"\u5c0f\u8bf4", None))
        self.game_Button.setText(QCoreApplication.translate("MainWindow", u"\u6e38\u620f", None))
        self.comic_Button.setText(QCoreApplication.translate("MainWindow", u"\u6f2b\u753b", None))
        self.pushButton_9.setText(QCoreApplication.translate("MainWindow", u"\u4e0b\u8f7d", None))
        self.settings_Button.setText("")
        self.pushButton_3.setText(QCoreApplication.translate("MainWindow", u"\u60f3\u770b", None))
        self.pushButton_4.setText(QCoreApplication.translate("MainWindow", u"\u5728\u770b", None))
        self.pushButton_5.setText(QCoreApplication.translate("MainWindow", u"\u770b\u8fc7", None))
        self.pushButton_6.setText(QCoreApplication.translate("MainWindow", u"\u6401\u7f6e", None))
        self.pushButton_7.setText(QCoreApplication.translate("MainWindow", u"\u629b\u5f03", None))
        self.project_Button.setText(QCoreApplication.translate("MainWindow", u"\u52a8\u753b", None))
        self.searchlist_lineEdit.setPlaceholderText(QCoreApplication.translate("MainWindow", u"\u641c\u7d22\u5f53\u524d\u5217\u8868", None))
        self.refresh_Button.setText("")
        self.cover_label_1.setText(QCoreApplication.translate("MainWindow", u"\u5c01\u9762", None))
        self.title_label_1.setText(QCoreApplication.translate("MainWindow", u"\u6807\u9898", None))
        self.progress_label_1.setText(QCoreApplication.translate("MainWindow", u"\u5168--\u8bdd", None))
        self.pushButton_16.setText("")
        self.episode_pushButton_1.setText(QCoreApplication.translate("MainWindow", u"\u9009\u96c6", None))
        self.previous_Button.setText(QCoreApplication.translate("MainWindow", u"\u4e0a\u4e00\u9875", None))
        self.pages_Button.setText(QCoreApplication.translate("MainWindow", u"--/--", None))
        self.next_Button.setText(QCoreApplication.translate("MainWindow", u"\u4e0b\u4e00\u9875", None))
        pass