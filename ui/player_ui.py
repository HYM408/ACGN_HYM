from PySide6.QtGui import QFont, QIcon
from PySide6.QtCore import QCoreApplication, QMetaObject, QRect, QSize, Qt
from PySide6.QtWidgets import QFrame, QGridLayout, QHBoxLayout,QPushButton, QScrollArea, QSizePolicy, QSpacerItem,QVBoxLayout, QWidget

class Ui_EpisodePage(object):
    def setupUi(self, EpisodePage):
        if not EpisodePage.objectName():
            EpisodePage.setObjectName(u"EpisodePage")
        EpisodePage.resize(600, 600)
        EpisodePage.setMinimumSize(QSize(600, 0))
        EpisodePage.setMaximumSize(QSize(600, 16777215))
        self.horizontalLayout_5 = QHBoxLayout(EpisodePage)
        self.horizontalLayout_5.setSpacing(0)
        self.horizontalLayout_5.setObjectName(u"horizontalLayout_5")
        self.horizontalLayout_5.setContentsMargins(0, 0, 0, 0)
        self.frame_2 = QFrame(EpisodePage)
        self.frame_2.setObjectName(u"frame_2")
        self.frame_2.setMinimumSize(QSize(600, 0))
        self.frame_2.setMaximumSize(QSize(600, 16777215))
        self.frame_2.setStyleSheet(u"background-color:rgb(230, 224, 233);")
        self.frame_2.setFrameShape(QFrame.StyledPanel)
        self.frame_2.setFrameShadow(QFrame.Raised)
        self.verticalLayout_9 = QVBoxLayout(self.frame_2)
        self.verticalLayout_9.setObjectName(u"verticalLayout_9")
        self.verticalLayout_9.setContentsMargins(0, -1, 0, -1)
        self.horizontalLayout_9 = QHBoxLayout()
        self.horizontalLayout_9.setObjectName(u"horizontalLayout_9")
        self.pushButton_14 = QPushButton(self.frame_2)
        self.pushButton_14.setObjectName(u"pushButton_14")
        self.pushButton_14.setMinimumSize(QSize(44, 44))
        self.pushButton_14.setMaximumSize(QSize(44, 44))
        self.pushButton_14.setStyleSheet(u"QPushButton {\n"
"	background-color: rgb(230, 224, 233);\n"
"	border:none;\n"
"	border-radius:22px\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgb(225, 219, 228);\n"
"}")
        icon = QIcon()
        icon.addFile(u"icons/back.png", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        self.pushButton_14.setIcon(icon)
        self.pushButton_14.setIconSize(QSize(25, 25))
        self.pushButton_14.setCheckable(True)

        self.horizontalLayout_9.addWidget(self.pushButton_14)

        self.pushButton_12 = QPushButton(self.frame_2)
        self.pushButton_12.setObjectName(u"pushButton_12")
        self.pushButton_12.setMinimumSize(QSize(500, 0))
        self.pushButton_12.setMaximumSize(QSize(300, 16777215))
        font = QFont()
        font.setFamilies([u"\u5fae\u8f6f\u96c5\u9ed1"])
        font.setPointSize(25)
        font.setBold(True)
        self.pushButton_12.setFont(font)
        self.pushButton_12.setStyleSheet(u"border:none;")

        self.horizontalLayout_9.addWidget(self.pushButton_12)

        self.horizontalSpacer_7 = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout_9.addItem(self.horizontalSpacer_7)

        self.pushButton_15 = QPushButton(self.frame_2)
        self.pushButton_15.setObjectName(u"pushButton_15")
        font1 = QFont()
        font1.setFamilies([u"\u5fae\u8f6f\u96c5\u9ed1"])
        font1.setPointSize(10)
        font1.setBold(True)
        self.pushButton_15.setFont(font1)

        self.horizontalLayout_9.addWidget(self.pushButton_15)


        self.verticalLayout_9.addLayout(self.horizontalLayout_9)

        self.pushButton_13 = QPushButton(self.frame_2)
        self.pushButton_13.setObjectName(u"pushButton_13")
        font2 = QFont()
        font2.setFamilies([u"\u5fae\u8f6f\u96c5\u9ed1"])
        font2.setPointSize(15)
        font2.setBold(True)
        self.pushButton_13.setFont(font2)
        self.pushButton_13.setStyleSheet(u"border:none;")

        self.verticalLayout_9.addWidget(self.pushButton_13)

        self.scrollArea = QScrollArea(self.frame_2)
        self.scrollArea.setObjectName(u"scrollArea")
        self.scrollArea.setStyleSheet(u"border:none")
        self.scrollArea.setWidgetResizable(True)
        self.scrollAreaWidgetContents_2 = QWidget()
        self.scrollAreaWidgetContents_2.setObjectName(u"scrollAreaWidgetContents_2")
        self.scrollAreaWidgetContents_2.setGeometry(QRect(0, 0, 598, 495))
        self.gridLayout_3 = QGridLayout(self.scrollAreaWidgetContents_2)
        self.gridLayout_3.setObjectName(u"gridLayout_3")
        self.verticalSpacer_5 = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.gridLayout_3.addItem(self.verticalSpacer_5, 0, 0, 1, 1)

        self.scrollArea.setWidget(self.scrollAreaWidgetContents_2)

        self.verticalLayout_9.addWidget(self.scrollArea)


        self.horizontalLayout_5.addWidget(self.frame_2, 0, Qt.AlignLeft)


        self.retranslateUi(EpisodePage)

        QMetaObject.connectSlotsByName(EpisodePage)
    # setupUi

    def retranslateUi(self, EpisodePage):
        self.pushButton_14.setText("")
        self.pushButton_12.setText(QCoreApplication.translate("EpisodePage", u"\u9009\u96c6", None))
        self.pushButton_15.setText(QCoreApplication.translate("EpisodePage", u"\u5168\u90e8\u5df2\u770b", None))
        self.pushButton_13.setText(QCoreApplication.translate("EpisodePage", u"\u6807\u9898", None))
        pass
    # retranslateUi

