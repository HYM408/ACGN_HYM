#include "chunk_download.h"
#include <QDir>

ChunkDownload::ChunkDownload(const QString &url, QString savePath, int maxWorkers, QObject *parent): QObject(parent) , m_manager(new QNetworkAccessManager(this)) , m_url(url) , m_savePath(std::move(savePath)) , m_maxWorkers(maxWorkers) , m_totalSize(0) , m_downloaded(0) , m_isRunning(false) , m_chunksFinished(0) , m_hasError(false) {}

ChunkDownload::~ChunkDownload()
{
    stop();
}

void ChunkDownload::start()
{   // 开始下载
    stop();
    m_isRunning = true;
    m_downloaded = 0;
    m_chunksFinished = 0;
    m_hasError = false;
    m_chunkDownloaded.clear();
    setStatus("下载中");
    QNetworkRequest request(m_url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
    QNetworkReply *reply = m_manager->head(request);
    connect(reply, &QNetworkReply::finished, this, &ChunkDownload::onHeadFinished);
    connect(reply, &QNetworkReply::errorOccurred, this, [this, reply](QNetworkReply::NetworkError code) {
        if (code != QNetworkReply::NoError) {
            m_hasError = true;
            setStatus("错误");
            m_isRunning = false;
            reply->deleteLater();
        }
    });
}

void ChunkDownload::stop()
{   // 停止下载
    if (m_isRunning) {
        m_isRunning = false;
        for (QNetworkReply *reply : std::as_const(m_replies)) if (reply->isRunning()) reply->abort();
    }
    cleanupTempFiles();
    setStatus("已停止");
}

void ChunkDownload::onHeadFinished()
{   // 处理HEAD响应
    auto *reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();
    if (m_hasError || !m_isRunning) return;
    if (reply->error() != QNetworkReply::NoError) {
        setStatus("错误");
        m_isRunning = false;
        return;
    }
    m_totalSize = reply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
    if (m_totalSize <= 0) {
        setStatus("失败");
        m_isRunning = false;
        return;
    }
    startMultiPartDownload();
}

void ChunkDownload::startMultiPartDownload()
{   // 启动分块下载
    qint64 chunkSize = m_totalSize / m_maxWorkers;
    QList<QPair<qint64, qint64>> ranges;
    for (int i = 0; i < m_maxWorkers - 1; ++i) ranges.append({i * chunkSize, (i + 1) * chunkSize - 1});
    ranges.append({(m_maxWorkers - 1) * chunkSize, m_totalSize - 1});
    m_chunkDownloaded.resize(m_maxWorkers);
    m_chunkDownloaded.fill(0);
    m_replies.clear();

    QFileInfo fileInfo(m_savePath);
    QDir dir = fileInfo.absoluteDir();
    if (!dir.exists()) dir.mkpath(".");

    for (int i = 0; i < m_maxWorkers; ++i) {
        qint64 start = ranges[i].first;
        qint64 end = ranges[i].second;
        QNetworkRequest request(m_url);
        request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
        QString rangeHeader = QString("bytes=%1-%2").arg(start).arg(end);
        request.setRawHeader("Range", rangeHeader.toUtf8());
        QNetworkReply *reply = m_manager->get(request);
        m_replies.append(reply);
        connect(reply, &QNetworkReply::readyRead, this, &ChunkDownload::onChunkReadyRead);
        connect(reply, &QNetworkReply::finished, this, &ChunkDownload::onChunkFinished);
        connect(reply, &QNetworkReply::errorOccurred, this, &ChunkDownload::onChunkError);
        reply->setProperty("chunkIndex", i);
        QString tempFile = m_savePath + QString(".part%1").arg(i);
        auto *file = new QFile(tempFile, reply);
        if (!file->open(QIODevice::WriteOnly)) {
            reply->abort();
            delete file;
            continue;
        }
        reply->setProperty("tempFile", QVariant::fromValue(file));
    }
}

void ChunkDownload::onChunkReadyRead()
{   // 下载数据处理
    auto *reply = qobject_cast<QNetworkReply*>(sender());
    if (!m_isRunning) return;
    int index = reply->property("chunkIndex").toInt();
    auto *file = reply->property("tempFile").value<QFile*>();
    QByteArray data = reply->readAll();
    file->write(data);
    m_chunkDownloaded[index] += data.size();
    qint64 total = 0;
    for (qint64 size : std::as_const(m_chunkDownloaded)) total += size;
    m_downloaded = total;
    int percent = m_totalSize > 0 ? static_cast<int>((m_downloaded * 100) / m_totalSize) : 0;
    emit progressChanged(percent, m_downloaded, m_totalSize);
}

void ChunkDownload::onChunkFinished()
{   // 分块下载完成
    auto *reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();
    if (!m_isRunning || m_hasError) return;
    reply->property("tempFile").value<QFile*>()->close();
    if (reply->error() != QNetworkReply::NoError) {
        m_hasError = true;
        setStatus("错误");
        for (QNetworkReply *r : std::as_const(m_replies)) {
            if (r != reply && r->isRunning()) r->abort();
        }
        return;
    }
    m_chunksFinished++;
    if (m_chunksFinished == m_maxWorkers) {
        mergeChunks();
        setStatus("完成");
        m_isRunning = false;
    }
}

void ChunkDownload::onChunkError(QNetworkReply::NetworkError)
{   // 分块请求错误
    auto *reply = qobject_cast<QNetworkReply*>(sender());
    if (m_isRunning) {
        m_hasError = true;
        setStatus("错误");
        for (QNetworkReply *r : std::as_const(m_replies)) {
            if (r != reply && r->isRunning()) r->abort();
        }
    }
}

void ChunkDownload::mergeChunks() const
{   // 合并分块
    QFile finalFile(m_savePath);
    if (!finalFile.open(QIODevice::WriteOnly)) return;
    for (int i = 0; i < m_maxWorkers; ++i) {
        QString tempFile = m_savePath + QString(".part%1").arg(i);
        QFile partFile(tempFile);
        if (!partFile.open(QIODevice::ReadOnly)) return;
        finalFile.write(partFile.readAll());
        partFile.close();
        partFile.remove();
    }
    finalFile.close();
}

void ChunkDownload::cleanupTempFiles() const
{   // 清理临时文件
    for (int i = 0; i < m_maxWorkers; ++i) {
        QString tempFile = m_savePath + QString(".part%1").arg(i);
        QFile::remove(tempFile);
    }
}

void ChunkDownload::setStatus(const QString &status)
{   // 状态更新
    if (m_status != status) {
        m_status = status;
        emit statusChanged(status);
    }
}