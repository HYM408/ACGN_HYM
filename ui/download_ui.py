from PySide6.QtCore import QCoreApplication, QMetaObject

class Ui_DownloaderPage(object):
    def setupUi(self, DownloaderPage):
        if not DownloaderPage.objectName():
            DownloaderPage.setObjectName(u"DownloaderPage")
        DownloaderPage.resize(1525, 942)
        self.retranslateUi(DownloaderPage)
        QMetaObject.connectSlotsByName(DownloaderPage)

    def retranslateUi(self, DownloaderPage):
        DownloaderPage.setWindowTitle(QCoreApplication.translate("DownloaderPage", u"Form", None))