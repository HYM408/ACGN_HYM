#ifndef DOWNLOAD_PAGE_H
#define DOWNLOAD_PAGE_H

#include "download_ui.h"

class PikPakApi;
class ChunkDownload;

class DownloadPage : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadPage(QWidget *parent = nullptr);
    void setManagers(PikPakApi *pikpakapi);
    void applyTheme() const;
    void loadRecentFiles();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void selectDownloadPath();

private:
    void setupDownloadPathUI();
    void setupDownloadList() const;
    void displayItems(const QJsonArray &items, const QString &emptyMessage);
    void loadFolderContent(const QString &folderId);
    void fetchAndDownload(const QString& fileId, const QString& fileName);
    QFrame* createDownloadItem(const QString &fileName);
    static void updateDownloadProgress(const QFrame *widget, int percent, qint64 downloaded, qint64 total);
    static void updateDownloadStatus(QFrame *widget, const QString &status);
    static void clearLayout(QLayout *layout);
    Ui::DownloaderPage ui{};
    PikPakApi *pikpakApi = nullptr;
    QString downloadPath;
    QString currentFolderId;
    QMap<QString, ChunkDownload*> downloadTasks;
    QMap<QString, QFrame*> taskWidgets;
};

#endif // DOWNLOAD_PAGE_H