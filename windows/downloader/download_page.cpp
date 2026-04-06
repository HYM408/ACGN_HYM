#include "download_page.h"
#include <QLabel>
#include <QPointer>
#include <QJsonArray>
#include <QJsonObject>
#include <QFileDialog>
#include "../config.h"
#include "chunk_download.h"
#include "../api/pikpak_api.h"

DownloadPage::DownloadPage(QWidget *parent): QWidget(parent)
{
    ui.setupUi(this);
    applyTheme();
    setupDownloadPathUI();
    setupDownloadList();
}

void DownloadPage::setManagers(PikPakApi *pikpakapi)
{   // 初始化实例
    pikpakApi = pikpakapi;
}

void DownloadPage::applyTheme() const
{   // 主题
    const QColor color1 = getConfig("Theme/color1", 0xfdf7ff).toString();
    ui.mainFrame->setStyleSheet(QString("QFrame {background-color: %1}").arg(color1.name()));
}

void DownloadPage::setupDownloadPathUI()
{   // 下载路径
    downloadPath = getConfig("Download/download_path").toString();
    ui.lineEditDownloadPath->setText(downloadPath);
    connect(ui.btnSelectPath, &QPushButton::clicked, this, &DownloadPage::selectDownloadPath);
}

void DownloadPage::selectDownloadPath()
{   // 设置下载路径
    const QString currentPath = ui.lineEditDownloadPath->text();
    if (const QString dirPath = QFileDialog::getExistingDirectory(this, "选择下载路径", currentPath); !dirPath.isEmpty()) {
        ui.lineEditDownloadPath->setText(dirPath);
        setConfig("Download/download_path", dirPath);
        downloadPath = dirPath;
    }
}

void DownloadPage::displayItems(const QJsonArray &items, const QString &emptyMessage)
{   // 显示组件
    clearLayout(ui.fileListLayout);
    if (items.isEmpty()) {
        auto *label = new QLabel(emptyMessage);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("font-size: 14px; color: #999; padding: 20px");
        ui.fileListLayout->addWidget(label);
        ui.fileListLayout->addStretch();
        return;
    }
    for (const auto &val : items) {
        QJsonObject obj = val.toObject();
        QString typeText = obj["kind"].toString() == "drive#folder" ? "文件夹" : "文件";
        QString sizeText;
        if (typeText == "文件") {
            const qint64 size = obj["size"].toString().toLongLong();
            sizeText = " " + QLocale().formattedDataSize(size);
        }
        auto *item = new QFrame(this);
        item->setStyleSheet("QFrame {border: 1px solid #e0e0e0}"
                            "QFrame:hover {background-color: #f0f0f0}");
        item->setProperty("type", typeText);
        item->setProperty("file_id", obj["id"].toString());
        item->setProperty("file_name", obj["name"].toString());
        item->installEventFilter(this);
        auto *layout = new QVBoxLayout(item);
        auto *nameLabel = new QLabel(obj["name"].toString());
        nameLabel->setStyleSheet("font-size: 14px; background: transparent; border: none");
        nameLabel->setWordWrap(true);
        nameLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
        layout->addWidget(nameLabel);
        auto *typeLabel = new QLabel(typeText + sizeText);
        typeLabel->setStyleSheet("font-size: 12px; background: transparent; border: none");
        typeLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
        layout->addWidget(typeLabel);
        ui.fileListLayout->addWidget(item);
    }
    ui.fileListLayout->addStretch();
}

void DownloadPage::loadRecentFiles()
{   // 显示最近添加
    QPointer guard(this);
    pikpakApi->getRecentFiles(3, [guard](const QJsonObject &result, const int statusCode, const QString &error) {
        if (!guard) return;
        if (statusCode == 200 && error.isEmpty()) {
            QJsonArray events = result["events"].toArray();
            QJsonArray items;
            for (const auto &val : events) {
                QJsonObject event = val.toObject();
                QJsonObject ref = event["reference_resource"].toObject();
                QJsonObject item;
                item["id"] = event["file_id"].toString();
                item["name"] = event["file_name"].toString();
                item["kind"] = ref["kind"].toString();
                item["size"] = ref["size"].toString();
                items.append(item);
            }
            guard->displayItems(items, "暂无最近添加的文件");
        } else guard->displayItems(QJsonArray(), "获取最近文件失败：" + error);
    });
}

void DownloadPage::fetchAndDownload(const QString &fileId, const QString &fileName)
{   // 获取链接并下载
    QFrame *widget = taskWidgets.value(fileName, nullptr);
    updateDownloadStatus(widget, "获取链接");
    const QPointer guard(this);
    pikpakApi->getDownloadUrl(fileId, 3, [=](const QJsonObject &fileInfo, const int statusCode, const QString &error) {
        if (!guard) return;
        if (statusCode != 200 || !error.isEmpty()) {
            updateDownloadStatus(widget, "获取链接失败");
            return;
        }
        QString downloadUrl;
        const auto medias = fileInfo["medias"].toArray();
        if (!medias.isEmpty()) {
            auto link = medias.first().toObject()["link"].toObject();
            auto mirrors = link["mirrors"].toArray();
            if (!mirrors.isEmpty()) downloadUrl = mirrors[mirrors.size() >= 2 ? 1 : 0].toString();
            else downloadUrl = link["url"].toString();
        }
        if (downloadUrl.isEmpty()) downloadUrl = fileInfo["web_content_link"].toString();
        const QString savePath = downloadPath + "/" + fileName;
        auto *task = new ChunkDownload(downloadUrl, savePath, 4, this);
        downloadTasks[fileName] = task;
        connect(task, &ChunkDownload::progressChanged, this, [this, fileName](const int percent, const qint64 downloaded, const qint64 total) {
            if (const QFrame *w = taskWidgets.value(fileName, nullptr)) updateDownloadProgress(w, percent, downloaded, total);});
        connect(task, &ChunkDownload::statusChanged, this, [this, fileName](const QString &status) {
            if (QFrame *w = taskWidgets.value(fileName, nullptr)) updateDownloadStatus(w, status);});
        task->start();
    });
}

void DownloadPage::loadFolderContent(const QString &folderId)
{   // 显示文件夹内容
    QPointer guard(this);
    pikpakApi->getFileList(folderId, 100, "", 3, [guard, folderId](const QJsonObject &result, const int statusCode, const QString &error) {
        if (!guard) return;
        if (statusCode == 200 && error.isEmpty()) {
            const QJsonArray files = result["files"].toArray();
            guard->displayItems(files, "文件夹为空");
            guard->currentFolderId = folderId;
        } else guard->displayItems(QJsonArray(), "获取文件夹内容失败：" + (error.isEmpty() ? QString::number(statusCode) : error));
    });
}

bool DownloadPage::eventFilter(QObject *obj, QEvent *event)
{   // 点击事件
    if (event->type() == QEvent::MouseButtonPress) {
        const auto *frame = qobject_cast<QFrame*>(obj);
        if (!frame) return QWidget::eventFilter(obj, event);
        const QString type = frame->property("type").toString();
        const QString fileId = frame->property("file_id").toString();
        const QString fileName = frame->property("file_name").toString();
        if (type == "文件") {
            if (taskWidgets.contains(fileName) || downloadTasks.contains(fileName)) return true;
            QFrame *widget = createDownloadItem(fileName);
            taskWidgets[fileName] = widget;
            ui.taskListLayout->insertWidget(0, widget);
            fetchAndDownload(fileId, fileName);
            return true;
        }
        loadFolderContent(fileId);
        return true;
    }
    return QWidget::eventFilter(obj, event);
}

void DownloadPage::setupDownloadList() const
{   // 设置下载列表
    ui.taskListLayout->setSpacing(5);
    ui.taskListLayout->setContentsMargins(5, 5, 5, 5);
    ui.taskListLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

QFrame* DownloadPage::createDownloadItem(const QString &fileName)
{   // 创建下载组件
    auto *widget = new QFrame(this);
    widget->setFixedHeight(50);
    widget->setStyleSheet("QFrame {border: 2px solid #9E9E9E; border-radius: 4px; background: transparent}");
    auto *progressFrame = new QFrame(widget);
    progressFrame->setGeometry(0, 0, 0, 50);
    progressFrame->setStyleSheet("background-color: rgba(76, 175, 80, 0.3); border: none; border-radius: 3px");
    progressFrame->lower();
    auto *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(10, 5, 10, 5);
    auto *statusLabel = new QLabel("获取链接");
    statusLabel->setStyleSheet("font-size: 13px; font-weight: bold; background: transparent; border: none; min-width: 40px");
    layout->addWidget(statusLabel);
    auto *nameLabel = new QLabel(fileName);
    nameLabel->setStyleSheet("font-size: 13px; font-weight: bold; background: transparent; border: none");
    nameLabel->setWordWrap(true);
    layout->addWidget(nameLabel, 1);
    auto *progressLabel = new QLabel("0B/0B");
    progressLabel->setStyleSheet("font-size: 13px; font-weight: bold; background: transparent; border: none");
    layout->addWidget(progressLabel);
    auto *removeBtn = new QPushButton("删除");
    removeBtn->setFixedSize(50, 25);
    removeBtn->setStyleSheet("QPushButton {font-size: 11px; color: #ff6b6b; background: transparent; border: 1px solid #ff6b6b; border-radius: 3px}"
                             "QPushButton:hover {background: rgba(255, 107, 107, 0.1)}");
    layout->addWidget(removeBtn);
    widget->setProperty("file_name", fileName);
    widget->setProperty("progress_frame", QVariant::fromValue(progressFrame));
    widget->setProperty("status_label", QVariant::fromValue(statusLabel));
    widget->setProperty("progress_label", QVariant::fromValue(progressLabel));
    widget->installEventFilter(this);
    connect(removeBtn, &QPushButton::clicked, this, [this, fileName] {
        ChunkDownload *task = downloadTasks.take(fileName);
        task->stop();
        task->deleteLater();
        taskWidgets.take(fileName)->deleteLater();
    });
    return widget;
}

void DownloadPage::updateDownloadProgress(const QFrame *widget, const int percent, const qint64 downloaded, const qint64 total)
{   // 更新下载进度
    const int width = widget->width() * percent / 100;
    widget->property("progress_frame").value<QFrame*>()->setFixedWidth(width);
    QString downloadedStr = QLocale().formattedDataSize(downloaded);
    QString totalStr = QLocale().formattedDataSize(total);
    widget->property("progress_label").value<QLabel*>()->setText(QString("%1/%2").arg(downloadedStr, totalStr));
}

void DownloadPage::updateDownloadStatus(QFrame *widget, const QString &status)
{   // 更新下载状态
    widget->property("status_label").value<QLabel*>()->setText(status);
    const QMap<QString, QString> colorMap = {
        {"下载中", "#4CAF50"},
        {"完成", "#2196F3"},
        {"错误", "#F44336"},
        {"获取链接", "#FF9800"}
    };
    const QString color = colorMap.value(status, "#9E9E9E");
    widget->setStyleSheet(QString("QFrame {border: 2px solid %1; border-radius: 4px; background: transparent}").arg(color));
}

void DownloadPage::clearLayout(QLayout *layout)
{   // 清理组件
    while (const QLayoutItem *item = layout->takeAt(0)) {
        if (QWidget *w = item->widget()) w->deleteLater();
        delete item;
    }
}