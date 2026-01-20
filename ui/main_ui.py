from PySide6.QtGui import QFont, QIcon
from PySide6.QtCore import QCoreApplication, QMetaObject, QSize, Qt
from PySide6.QtWidgets import QButtonGroup, QFrame, QGridLayout, QHBoxLayout, QLineEdit, QPushButton, QSizePolicy, QSpacerItem, QStackedWidget, QVBoxLayout, QWidget

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
        self.titlebar_frame.setFrameShape(QFrame.Shape.StyledPanel)
        self.titlebar_frame.setFrameShadow(QFrame.Shadow.Raised)
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
        self.horizontalLayout.addWidget(self.pushButton_8, 0, Qt.AlignmentFlag.AlignRight)
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
        self.horizontalLayout.addWidget(self.pushButton_2, 0, Qt.AlignmentFlag.AlignRight)
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
        self.toolbar_frame.setFrameShape(QFrame.Shape.NoFrame)
        self.verticalLayout_3 = QVBoxLayout(self.toolbar_frame)
        self.verticalLayout_3.setSpacing(0)
        self.verticalLayout_3.setObjectName(u"verticalLayout_3")
        self.verticalLayout_3.setContentsMargins(0, 40, 0, -1)
        self.search_Button = QPushButton(self.toolbar_frame)
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
        self.verticalLayout_3.addWidget(self.search_Button, 0, Qt.AlignmentFlag.AlignHCenter)
        self.acgnselect_frame = QFrame(self.toolbar_frame)
        self.acgnselect_frame.setObjectName(u"acgnselect_frame")
        self.acgnselect_frame.setMinimumSize(QSize(80, 0))
        self.acgnselect_frame.setStyleSheet(u"background-color: rgb(242, 236, 244);")
        self.acgnselect_frame.setFrameShape(QFrame.Shape.NoFrame)
        self.verticalLayout_4 = QVBoxLayout(self.acgnselect_frame)
        self.verticalLayout_4.setSpacing(40)
        self.verticalLayout_4.setObjectName(u"verticalLayout_4")
        self.verticalLayout_4.setContentsMargins(-1, 15, -1, -1)
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
        self.verticalLayout_3.addWidget(self.settings_Button, 0, Qt.AlignmentFlag.AlignHCenter)
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
        self.header_frame.setFrameShape(QFrame.Shape.NoFrame)
        self.verticalLayout_5 = QVBoxLayout(self.header_frame)
        self.verticalLayout_5.setSpacing(0)
        self.verticalLayout_5.setObjectName(u"verticalLayout_5")
        self.verticalLayout_5.setContentsMargins(-1, 0, -1, 0)
        self.frame = QFrame(self.header_frame)
        self.frame.setObjectName(u"frame")
        self.frame.setFrameShape(QFrame.Shape.StyledPanel)
        self.frame.setFrameShadow(QFrame.Shadow.Raised)
        self.horizontalLayout_4 = QHBoxLayout(self.frame)
        self.horizontalLayout_4.setSpacing(0)
        self.horizontalLayout_4.setObjectName(u"horizontalLayout_4")
        self.horizontalLayout_4.setContentsMargins(-1, 0, -1, 0)
        self.project_Button = QPushButton(self.frame)
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
        self.horizontalLayout_4.addWidget(self.project_Button, 0, Qt.AlignmentFlag.AlignLeft)
        self.verticalLayout_5.addWidget(self.frame)
        self.frame_2 = QFrame(self.header_frame)
        self.frame_2.setObjectName(u"frame_2")
        self.frame_2.setFrameShape(QFrame.Shape.StyledPanel)
        self.frame_2.setFrameShadow(QFrame.Shadow.Raised)
        self.horizontalLayout_5 = QHBoxLayout(self.frame_2)
        self.horizontalLayout_5.setSpacing(20)
        self.horizontalLayout_5.setObjectName(u"horizontalLayout_5")
        self.horizontalLayout_5.setContentsMargins(-1, 0, -1, 0)
        self.frame_3 = QFrame(self.frame_2)
        self.frame_3.setObjectName(u"frame_3")
        self.frame_3.setMinimumSize(QSize(90, 0))
        self.frame_3.setMaximumSize(QSize(90, 16777215))
        self.frame_3.setFrameShape(QFrame.Shape.StyledPanel)
        self.frame_3.setFrameShadow(QFrame.Shadow.Raised)
        self.horizontalLayout_7 = QHBoxLayout(self.frame_3)
        self.horizontalLayout_7.setSpacing(0)
        self.horizontalLayout_7.setObjectName(u"horizontalLayout_7")
        self.horizontalLayout_7.setContentsMargins(0, 0, 0, 0)
        self.pushButton_3 = QPushButton(self.frame_3)
        self.pushButton_3.setObjectName(u"pushButton_3")
        self.pushButton_3.setFont(font)
        self.pushButton_3.setStyleSheet(u"border:none")
        self.horizontalLayout_7.addWidget(self.pushButton_3)
        self.pushButton_10 = QPushButton(self.frame_3)
        self.pushButton_10.setObjectName(u"pushButton_10")
        self.pushButton_10.setMinimumSize(QSize(30, 30))
        self.pushButton_10.setMaximumSize(QSize(30, 30))
        font2 = QFont()
        font2.setFamilies([u"\u5fae\u8f6f\u96c5\u9ed1"])
        self.pushButton_10.setFont(font2)
        self.pushButton_10.setStyleSheet(u"border:none;\n"
"border-radius:15px;\n"
"background-color: rgb(242, 236, 244)")
        self.horizontalLayout_7.addWidget(self.pushButton_10)
        self.horizontalLayout_5.addWidget(self.frame_3)
        self.frame_5 = QFrame(self.frame_2)
        self.frame_5.setObjectName(u"frame_5")
        self.frame_5.setMinimumSize(QSize(90, 0))
        self.frame_5.setMaximumSize(QSize(90, 16777215))
        self.frame_5.setFrameShape(QFrame.Shape.StyledPanel)
        self.frame_5.setFrameShadow(QFrame.Shadow.Raised)
        self.horizontalLayout_8 = QHBoxLayout(self.frame_5)
        self.horizontalLayout_8.setSpacing(0)
        self.horizontalLayout_8.setObjectName(u"horizontalLayout_8")
        self.horizontalLayout_8.setContentsMargins(0, 0, 0, 0)
        self.pushButton_4 = QPushButton(self.frame_5)
        self.pushButton_4.setObjectName(u"pushButton_4")
        self.pushButton_4.setFont(font)
        self.pushButton_4.setStyleSheet(u"border:none")
        self.horizontalLayout_8.addWidget(self.pushButton_4)
        self.pushButton_11 = QPushButton(self.frame_5)
        self.pushButton_11.setObjectName(u"pushButton_11")
        self.pushButton_11.setMinimumSize(QSize(30, 30))
        self.pushButton_11.setMaximumSize(QSize(30, 30))
        self.pushButton_11.setFont(font2)
        self.pushButton_11.setStyleSheet(u"border:none;\n"
"border-radius:15px;\n"
"background-color: rgb(242, 236, 244)")
        self.horizontalLayout_8.addWidget(self.pushButton_11)
        self.horizontalLayout_5.addWidget(self.frame_5)
        self.frame_7 = QFrame(self.frame_2)
        self.frame_7.setObjectName(u"frame_7")
        self.frame_7.setMinimumSize(QSize(90, 0))
        self.frame_7.setMaximumSize(QSize(90, 16777215))
        self.frame_7.setFrameShape(QFrame.Shape.StyledPanel)
        self.frame_7.setFrameShadow(QFrame.Shadow.Raised)
        self.horizontalLayout_9 = QHBoxLayout(self.frame_7)
        self.horizontalLayout_9.setSpacing(0)
        self.horizontalLayout_9.setObjectName(u"horizontalLayout_9")
        self.horizontalLayout_9.setContentsMargins(0, 0, 0, 0)
        self.pushButton_5 = QPushButton(self.frame_7)
        self.pushButton_5.setObjectName(u"pushButton_5")
        self.pushButton_5.setFont(font)
        self.pushButton_5.setStyleSheet(u"border:none")
        self.horizontalLayout_9.addWidget(self.pushButton_5)
        self.pushButton_12 = QPushButton(self.frame_7)
        self.pushButton_12.setObjectName(u"pushButton_12")
        self.pushButton_12.setMinimumSize(QSize(30, 30))
        self.pushButton_12.setMaximumSize(QSize(30, 30))
        self.pushButton_12.setFont(font2)
        self.pushButton_12.setStyleSheet(u"border:none;\n"
"border-radius:15px;\n"
"background-color: rgb(242, 236, 244)")
        self.horizontalLayout_9.addWidget(self.pushButton_12)
        self.horizontalLayout_5.addWidget(self.frame_7)
        self.frame_9 = QFrame(self.frame_2)
        self.frame_9.setObjectName(u"frame_9")
        self.frame_9.setMinimumSize(QSize(90, 0))
        self.frame_9.setMaximumSize(QSize(90, 16777215))
        self.frame_9.setFrameShape(QFrame.Shape.StyledPanel)
        self.frame_9.setFrameShadow(QFrame.Shadow.Raised)
        self.horizontalLayout_10 = QHBoxLayout(self.frame_9)
        self.horizontalLayout_10.setSpacing(0)
        self.horizontalLayout_10.setObjectName(u"horizontalLayout_10")
        self.horizontalLayout_10.setContentsMargins(0, 0, 0, 0)
        self.pushButton_6 = QPushButton(self.frame_9)
        self.pushButton_6.setObjectName(u"pushButton_6")
        self.pushButton_6.setFont(font)
        self.pushButton_6.setStyleSheet(u"border:none")
        self.horizontalLayout_10.addWidget(self.pushButton_6)
        self.pushButton_13 = QPushButton(self.frame_9)
        self.pushButton_13.setObjectName(u"pushButton_13")
        self.pushButton_13.setMinimumSize(QSize(30, 30))
        self.pushButton_13.setMaximumSize(QSize(30, 30))
        self.pushButton_13.setFont(font2)
        self.pushButton_13.setStyleSheet(u"border:none;\n"
"border-radius:15px;\n"
"background-color: rgb(242, 236, 244)")
        self.horizontalLayout_10.addWidget(self.pushButton_13)
        self.horizontalLayout_5.addWidget(self.frame_9)
        self.frame_11 = QFrame(self.frame_2)
        self.frame_11.setObjectName(u"frame_11")
        self.frame_11.setMinimumSize(QSize(90, 0))
        self.frame_11.setMaximumSize(QSize(90, 16777215))
        self.frame_11.setStyleSheet(u"")
        self.frame_11.setFrameShape(QFrame.Shape.StyledPanel)
        self.frame_11.setFrameShadow(QFrame.Shadow.Raised)
        self.horizontalLayout_2 = QHBoxLayout(self.frame_11)
        self.horizontalLayout_2.setSpacing(0)
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.horizontalLayout_2.setContentsMargins(0, 0, 0, 0)
        self.pushButton_7 = QPushButton(self.frame_11)
        self.pushButton_7.setObjectName(u"pushButton_7")
        self.pushButton_7.setFont(font)
        self.pushButton_7.setStyleSheet(u"border:none")
        self.horizontalLayout_2.addWidget(self.pushButton_7)
        self.pushButton_14 = QPushButton(self.frame_11)
        self.pushButton_14.setObjectName(u"pushButton_14")
        self.pushButton_14.setMinimumSize(QSize(30, 30))
        self.pushButton_14.setMaximumSize(QSize(30, 30))
        self.pushButton_14.setFont(font2)
        self.pushButton_14.setStyleSheet(u"border:none;\n"
"border-radius:15px;\n"
"background-color: rgb(242, 236, 244)")
        self.horizontalLayout_2.addWidget(self.pushButton_14, 0, Qt.AlignmentFlag.AlignRight)
        self.horizontalLayout_5.addWidget(self.frame_11)
        self.refresh_Button = QPushButton(self.frame_2)
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
        self.horizontalLayout_5.addWidget(self.refresh_Button)
        self.horizontalSpacer = QSpacerItem(774, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)
        self.horizontalLayout_5.addItem(self.horizontalSpacer)
        self.searchlist_lineEdit = QLineEdit(self.frame_2)
        self.searchlist_lineEdit.setObjectName(u"searchlist_lineEdit")
        self.searchlist_lineEdit.setMinimumSize(QSize(200, 25))
        self.searchlist_lineEdit.setMaximumSize(QSize(200, 25))
        self.searchlist_lineEdit.setContextMenuPolicy(Qt.ContextMenuPolicy.NoContextMenu)
        self.searchlist_lineEdit.setStyleSheet(u"")
        self.horizontalLayout_5.addWidget(self.searchlist_lineEdit)
        self.verticalLayout_5.addWidget(self.frame_2)
        self.verticalLayout_2.addWidget(self.header_frame)
        self.animationdata_frame = QFrame(self.showmain_stackedWidgetPage1)
        self.animationdata_frame.setObjectName(u"animationdata_frame")
        self.animationdata_frame.setStyleSheet(u"background-color: white;")
        self.animationdata_frame.setFrameShape(QFrame.Shape.NoFrame)
        self.gridLayout_2 = QGridLayout(self.animationdata_frame)
        self.gridLayout_2.setObjectName(u"gridLayout_2")
        self.gridLayout_2.setHorizontalSpacing(35)
        self.gridLayout_2.setVerticalSpacing(18)
        self.gridLayout_2.setContentsMargins(0, 10, 0, 10)
        self.verticalLayout_2.addWidget(self.animationdata_frame)
        self.page_frame = QFrame(self.showmain_stackedWidgetPage1)
        self.page_frame.setObjectName(u"page_frame")
        self.page_frame.setMinimumSize(QSize(0, 45))
        self.page_frame.setMaximumSize(QSize(16777215, 45))
        self.page_frame.setStyleSheet(u"QFrame {\n"
"    background-color: white\n"
"}\n"
"\n"
"QPushButton {\n"
"    border-width: 1px;\n"
"    border-style: solid; \n"
"    border-color: rgba(0,0,0,100);\n"
"    border-radius: 4px;\n"
"}")
        self.page_frame.setFrameShape(QFrame.Shape.NoFrame)
        self.horizontalLayout_3 = QHBoxLayout(self.page_frame)
        self.horizontalLayout_3.setObjectName(u"horizontalLayout_3")
        self.horizontalSpacer_3 = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)
        self.horizontalLayout_3.addItem(self.horizontalSpacer_3)
        self.previous_Button = QPushButton(self.page_frame)
        self.previous_Button.setObjectName(u"previous_Button")
        self.previous_Button.setMinimumSize(QSize(75, 20))
        self.previous_Button.setMaximumSize(QSize(75, 20))
        self.previous_Button.setStyleSheet(u"QPushButton:pressed {\n"
"   background-color: rgba(0,0,0,20)\n"
"}")
        self.previous_Button.setCheckable(True)
        self.horizontalLayout_3.addWidget(self.previous_Button)
        self.pages_Button = QPushButton(self.page_frame)
        self.pages_Button.setObjectName(u"pages_Button")
        self.pages_Button.setMinimumSize(QSize(75, 20))
        self.pages_Button.setMaximumSize(QSize(75, 20))
        self.pages_Button.setStyleSheet(u"")
        self.horizontalLayout_3.addWidget(self.pages_Button)
        self.next_Button = QPushButton(self.page_frame)
        self.next_Button.setObjectName(u"next_Button")
        self.next_Button.setMinimumSize(QSize(75, 20))
        self.next_Button.setMaximumSize(QSize(75, 20))
        self.next_Button.setStyleSheet(u"QPushButton:pressed {\n"
"   background-color: rgba(0,0,0,20)\n"
"}")
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
        self.project_Button.setText(QCoreApplication.translate("MainWindow", u"\u52a8\u753b", None))
        self.pushButton_3.setText(QCoreApplication.translate("MainWindow", u"\u60f3\u770b", None))
        self.pushButton_10.setText("")
        self.pushButton_4.setText(QCoreApplication.translate("MainWindow", u"\u5728\u770b", None))
        self.pushButton_11.setText("")
        self.pushButton_5.setText(QCoreApplication.translate("MainWindow", u"\u770b\u8fc7", None))
        self.pushButton_12.setText("")
        self.pushButton_6.setText(QCoreApplication.translate("MainWindow", u"\u6401\u7f6e", None))
        self.pushButton_13.setText("")
        self.pushButton_7.setText(QCoreApplication.translate("MainWindow", u"\u629b\u5f03", None))
        self.pushButton_14.setText("")
        self.refresh_Button.setText("")
        self.searchlist_lineEdit.setPlaceholderText(QCoreApplication.translate("MainWindow", u"\u641c\u7d22\u5f53\u524d\u5217\u8868", None))
        self.previous_Button.setText(QCoreApplication.translate("MainWindow", u"\u4e0a\u4e00\u9875", None))
        self.pages_Button.setText(QCoreApplication.translate("MainWindow", u"--/--", None))
        self.next_Button.setText(QCoreApplication.translate("MainWindow", u"\u4e0b\u4e00\u9875", None))
        pass