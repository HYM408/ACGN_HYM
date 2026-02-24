#ifndef CHUNK_DOWNLOAD_H
#define CHUNK_DOWNLOAD_H

#include <QNetworkReply>

class ChunkDownload : public QObject
{
    Q_OBJECT

public:
    explicit ChunkDownload(const QString &url, QString savePath, int maxWorkers = 4, QObject *parent = nullptr);
    ~ChunkDownload() override;
    void start();
    void stop();

signals:
    void progressChanged(int percent, qint64 downloaded, qint64 total);
    void statusChanged(const QString &status);

private slots:
    void onHeadFinished();
    void onChunkReadyRead();
    void onChunkFinished();
    void onChunkError(QNetworkReply::NetworkError code);

private:
    void startMultiPartDownload();
    void mergeChunks() const;
    void cleanupTempFiles() const;
    void setStatus(const QString &status);
    QNetworkAccessManager *m_manager;
    QUrl m_url;
    QString m_savePath;
    int m_maxWorkers;
    qint64 m_totalSize;
    qint64 m_downloaded;
    QString m_status;
    bool m_isRunning;
    QList<QNetworkReply*> m_replies;
    QList<qint64> m_chunkDownloaded;
    int m_chunksFinished;
    bool m_hasError;
};

#endif // CHUNK_DOWNLOAD_H