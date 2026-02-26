#include "download_page.h"
#include <QLabel>
#include <QJsonArray>
#include <QFileDialog>
#include "../config.h"
#include "chunk_download.h"
#include "../api/pikpak_api.h"

DownloadPage::DownloadPage(QWidget *parent): QWidget(parent)
{
    ui.setupUi(this);
    setupDownloadPathUI();
    setupDownloadList();
}

void DownloadPage::setManagers(PikPakApi *pikpakapi)
{   // 初始化实例
    pikpakApi = pikpakapi;
}

void DownloadPage::setupDownloadPathUI()
{   // 下载路径
    downloadPath = getConfig("Download/download_path").toString();
    ui.lineEdit->setText(downloadPath);
    connect(ui.pushButton, &QPushButton::clicked, this, &DownloadPage::selectDownloadPath);
}

void DownloadPage::selectDownloadPath()
{   // 设置下载路径
    QString dir = QFileDialog::getExistingDirectory(this, "选择下载路径", ui.lineEdit->text());
    ui.lineEdit->setText(dir);
    setConfig("Download/download_path", dir);
    downloadPath = dir;
}

void DownloadPage::displayItems(const QJsonArray &items, const QString &emptyMessage)
{   // 显示组件
    clearLayout(ui.verticalLayout_2);
    if (items.isEmpty()) {
        auto *label = new QLabel(emptyMessage);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("font-size: 14px; color: #999; padding: 20px");
        ui.verticalLayout_2->addWidget(label);
        ui.verticalLayout_2->addStretch();
        return;
    }
    for (const auto &val : items) {
        QJsonObject obj = val.toObject();
        QString typeText = obj["kind"].toString() == "drive#folder" ? "文件夹" : "文件";
        QString sizeText;
        if (typeText == "文件") {
            qint64 size = obj["size"].toString().toLongLong();
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
        ui.verticalLayout_2->addWidget(item);
    }
    ui.verticalLayout_2->addStretch();
}

void DownloadPage::loadRecentFiles()
{   // 显示最近添加
    QJsonObject result = pikpakApi->getRecentFiles();
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
    displayItems(items, "暂无最近添加的文件");
}

void DownloadPage::loadFolderContent(const QString &folderId)
{   // 显示文件夹内容
    QJsonObject result = pikpakApi->getFileList(folderId, 100, "");
    QJsonArray files = result["files"].toArray();
    displayItems(files, "文件夹为空");
    currentFolderId = folderId;
}

bool DownloadPage::eventFilter(QObject *obj, QEvent *event)
{   // 点击事件
    if (event->type() == QEvent::MouseButtonPress) {
        auto *frame = qobject_cast<QFrame*>(obj);
        if (!frame) return QWidget::eventFilter(obj, event);
        QString type = frame->property("type").toString();
        QString fileId = frame->property("file_id").toString();
        QString fileName = frame->property("file_name").toString();
        if (type == "文件") {
            QJsonObject fileInfo = pikpakApi->getDownloadUrl(fileId);
            if (fileInfo.isEmpty()) return true;
            QString downloadUrl = fileInfo["web_content_link"].toString();
            if (downloadUrl.isEmpty()) downloadUrl = fileInfo["medias"].toArray().first().toObject()["link"].toObject()["url"].toString();
            qDebug() << "下载链接:" << downloadUrl;
            addDownloadTask(downloadUrl, fileName);
        } else if (type == "文件夹") loadFolderContent(fileId);
        return true;
    }
    return QWidget::eventFilter(obj, event);
}

void DownloadPage::setupDownloadList() const
{   // 设置下载列表
    ui.verticalLayout_4->setSpacing(5);
    ui.verticalLayout_4->setContentsMargins(5, 5, 5, 5);
    ui.verticalLayout_4->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
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
    auto *statusLabel = new QLabel("等待");
    statusLabel->setStyleSheet("font-size: 13px; font-weight: bold; background: transparent; border: none; min-width: 40px");
    statusLabel->setFixedWidth(40);
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

void DownloadPage::addDownloadTask(const QString &url, const QString &fileName)
{   // 添加下载任务
    if (downloadTasks.contains(fileName)) return;
    QString savePath = downloadPath + "/" + fileName;
    auto *task = new ChunkDownload(url, savePath, 4, this);
    downloadTasks[fileName] = task;
    QFrame *widget = createDownloadItem(fileName);
    taskWidgets[fileName] = widget;
    connect(task, &ChunkDownload::progressChanged, this, [this, fileName](int percent, qint64 downloaded, qint64 total) {
        if (QFrame *widget = taskWidgets.value(fileName, nullptr)) updateDownloadProgress(widget, percent, downloaded, total);});
    connect(task, &ChunkDownload::statusChanged, this, [this, fileName](const QString &status) {
        if (QFrame *widget = taskWidgets.value(fileName, nullptr)) updateDownloadStatus(widget, status);});
    ui.verticalLayout_4->insertWidget(0, widget);
    task->start();
}

void DownloadPage::updateDownloadProgress(const QFrame *widget, int percent, qint64 downloaded, qint64 total)
{   // 更新下载进度
    int width = widget->width() * percent / 100;
    widget->property("progress_frame").value<QFrame*>()->setFixedWidth(width);
    QString downloadedStr = QLocale().formattedDataSize(downloaded);
    QString totalStr = QLocale().formattedDataSize(total);
    widget->property("progress_label").value<QLabel*>()->setText(QString("%1/%2").arg(downloadedStr, totalStr));
}

void DownloadPage::updateDownloadStatus(QFrame *widget, const QString &status)
{   // 更新下载状态
    widget->property("status_label").value<QLabel*>()->setText(status);
    QMap<QString, QString> colorMap = {
        {"下载中", "#4CAF50"},
        {"完成", "#2196F3"},
        {"错误", "#F44336"},
        {"等待", "#9E9E9E"}
    };
    QString color = colorMap.value(status, "#9E9E9E");
    widget->setStyleSheet(QString("QFrame {border: 2px solid %1; border-radius: 4px; background: transparent}").arg(color));
}

void DownloadPage::clearLayout(QLayout *layout)
{   // 清理组件
    while (QLayoutItem *item = layout->takeAt(0)) {
        if (QWidget *w = item->widget()) w->deleteLater();
        delete item;
    }
}