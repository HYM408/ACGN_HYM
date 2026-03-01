#include "detail_page.h"
#include <QTimer>
#include <QJsonObject>
#include <QDesktopServices>
#include "sql.h"
#include "config.h"
#include "utils/menu_util.h"
#include "api/bangumi_api.h"
#include "utils/image_util.h"

ClickableLabel::ClickableLabel(const QString &text, QWidget *parent): QLabel(text, parent)
{   // tag组件
    setTextFormat(Qt::RichText);
}

void ClickableLabel::mousePressEvent(QMouseEvent *event)
{   // 鼠标事件实现
    if (event->button() == Qt::LeftButton) emit clicked();
    QLabel::mousePressEvent(event);
}

DetailPage::DetailPage(QWidget *parent) : QWidget(parent)
{
    // 初始化UI
    ui.setupUi(this);
    // 初始化状态映射
    statusNamesMap = {
        {1, {{0, "收藏"}, {1, "想读"}, {2, "读过"}, {3, "在读"}, {4, "搁置"}, {5, "抛弃"}}},
        {2, {{0, "追番"}, {1, "想看"}, {2, "看过"}, {3, "在看"}, {4, "搁置"}, {5, "抛弃"}}},
        {4, {{0, "收藏"}, {1, "想玩"}, {2, "玩过"}, {3, "在玩"}, {4, "搁置"}, {5, "抛弃"}}},
        {7, {{0, "收藏"}, {1, "想读"}, {2, "读过"}, {3, "在读"}, {4, "搁置"}, {5, "抛弃"}}},
        {8, {{0, "收藏"}, {1, "想读"}, {2, "读过"}, {3, "在读"}, {4, "搁置"}, {5, "抛弃"}}}};
    // 初始化连接
    setupConnections();
}

void DetailPage::setManagers(DatabaseManager *db, CacheImageUtil *cacheImage, BangumiAPI *api)
{   // 初始化实例
    dbManager = db;
    cacheImageUtil = cacheImage;
    bangumiAPI = api;
}

void DetailPage::showEvent(QShowEvent *event)
{   // 显示
    QWidget::showEvent(event);
    loadData();
}

void DetailPage::setupConnections()
{   // 连接
    connect(ui.pushButton_19, &QPushButton::clicked, this, &DetailPage::onBackButtonClicked);
    connect(ui.pushButton_27, &QPushButton::clicked, this, &DetailPage::onEpisodeClicked);
    connect(ui.pushButton_20, &QPushButton::clicked, this, &DetailPage::onOpenBangumiPage);
    connect(ui.pushButton_26, &QPushButton::clicked, this, &DetailPage::onStatusButtonClicked);
}

void DetailPage::setCollectionData(const CollectionData &data)
{   // 显示传入数据
    currentData = data;
    if (currentData.subject_images_common.isEmpty()) {
        ui.cover_label_3->setText("暂无图片");
        ui.cover_label_3->setStyleSheet("QLabel {color: gray}");
    } else ImageUtil::loadImageWithCache(cacheImageUtil, currentData.subject_images_common, ui.cover_label_3, 15, true, true);
    ui.textEdit->setText(currentData.subject_name_cn.isEmpty() ? currentData.subject_name : currentData.subject_name_cn);
    ui.pushButton_24->setText(QString("全%1话").arg(currentData.subject_eps > 0 ? QString::number(currentData.subject_eps) : "-"));
    ui.pushButton_26->setText(statusNamesMap.value(currentData.subject_type).value(currentData.type));
    if (currentData.subject_type == 2) {
        ui.pushButton_27->setText("选集");
        ui.pushButton_27->setEnabled(true);
    }
    else if (currentData.subject_type == 4) {
        ui.pushButton_27->setText("");
        ui.pushButton_27->setEnabled(false);
    }
    else {
        ui.pushButton_27->setText("进度");
        ui.pushButton_27->setEnabled(true);
    }
}

void DetailPage::setCollectionDataFromMap(const QVariantMap &data)
{   // 数据加工
    CollectionData collectionData;
    collectionData.subject_id = data["subject_id"].toInt();
    collectionData.subject_name = data["subject_name"].toString();
    collectionData.subject_name_cn = data["subject_name_cn"].toString();
    collectionData.subject_images_common = data["subject_images_common"].toString();
    collectionData.subject_type = data["subject_type"].toInt();
    collectionData.type = data["type"].toInt();
    setCollectionData(collectionData);
}

void DetailPage::loadData()
{   // 加载数据
    QJsonObject subjectData = DatabaseManager::getSubjectById(currentData.subject_id);
    if (!subjectData.isEmpty()) updateDetailPage(subjectData);
    else {
        QTimer::singleShot(0, this, [this] {
            QJsonObject subjectInfo = bangumiAPI->getSubjectInfo(currentData.subject_id);
            DatabaseManager::insertOrUpdateSubject(subjectInfo);
            if (!isVisible()) return;
            updateDetailPage(DatabaseManager::getSubjectById(currentData.subject_id));
        });
    }
}

void DetailPage::onEpisodeClicked()
{   // 打开选集页
    emit showEpisodePageRequested(currentData);
}

void DetailPage::onOpenBangumiPage() const
{   // 跳转Bangumi
    QString baseUrl = getConfig("Bangumi/bangumi_base_url").toString();
    QDesktopServices::openUrl(QUrl(QString("%1subject/%2").arg(baseUrl).arg(currentData.subject_id)));
}

void DetailPage::onStatusButtonClicked()
{   // 改变状态
    int subjectType = currentData.subject_type;
    int currentStatus = currentData.type;
    StatusSelector::showStatusSelector(ui.pushButton_26, subjectType, currentStatus, currentData.subject_id, bangumiAPI, [this, subjectType](int selectedStatus) {
        currentData.type = selectedStatus;
        ui.pushButton_26->setText(statusNamesMap.value(subjectType).value(selectedStatus));
    }, -20);
}

void DetailPage::updateDetailPage(const QJsonObject &subjectData)
{   // 显示数据
    QString score = QString::number(subjectData.value("rating_score").toDouble());
    QString total = QString::number(subjectData.value("rating_total").toInt());
    QString rank = QString::number(subjectData.value("rating_rank").toInt());
    ui.pushButton_21->setText(QString("%1|%2人评|#%3").arg(score, total, rank));
    int collect = subjectData.value("collect").toInt();
    int onHold = subjectData.value("on_hold").toInt();
    int dropped = subjectData.value("dropped").toInt();
    int wish = subjectData.value("wish").toInt();
    int doing = subjectData.value("doing").toInt();
    ui.pushButton_25->setText(QString("%1收藏/%2在看/%3抛弃").arg(collect + onHold + dropped + wish + doing).arg(doing).arg(dropped));
    ui.textEdit_2->setText(subjectData.value("summary").toString());
    QList<QPair<QString, int>> tagPairs;
    QJsonObject tagsObject = subjectData.value("tags").toObject();
    for (auto it = tagsObject.begin(); it != tagsObject.end(); ++it) {tagPairs.append(qMakePair(it.key(), it.value().toInt()));}
    std::sort(tagPairs.begin(), tagPairs.end(), [](const QPair<QString, int> &a, const QPair<QString, int> &b) {return b.second < a.second;});
    tagsDisplay(tagPairs);
    QString timeTag = getTimeInfo(tagPairs, subjectData.value("date").toString());
    ui.pushButton_23->setText(timeTag);
}

void DetailPage::tagsDisplay(const QList<QPair<QString, int>> &tagPairs)
{   // tag显示
    clearLayout();
    auto *mainLayout = new QVBoxLayout(ui.frame_5);
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    QFont font = ui.frame_5->font();
    QFontMetrics fm(font);
    int maxWidth = ui.frame_5->width();
    QHBoxLayout *currentLayout = nullptr;
    QWidget *currentWidget = nullptr;
    int currentWidth = 0;
    QVector<QPair<QString, int>> labelData;
    for (const auto &pair : tagPairs) {
        labelData.append(qMakePair(
            QString("<span style='color:black'>%1</span> <span style='color:gray'>%2</span>").arg(pair.first).arg(pair.second),
            fm.horizontalAdvance(pair.first + " " + QString::number(pair.second)) + 25
        ));
    }
    for (int i = 0; i < labelData.size(); ++i) {
        const auto &data = labelData[i];
        if (!currentLayout || currentWidth + data.second > maxWidth) {
            currentWidget = new QWidget();
            currentLayout = new QHBoxLayout(currentWidget);
            currentLayout->setSpacing(8);
            currentLayout->setContentsMargins(0, 0, 0, 0);
            currentLayout->addStretch(1);
            mainLayout->addWidget(currentWidget);
            currentWidth = 0;
        }
        auto *tagLabel = new ClickableLabel(data.first, currentWidget);
        tagLabel->setFixedSize(data.second, fm.height() + 10);
        tagLabel->setStyleSheet("QLabel {border: 1px solid gray; border-radius: 10px; padding: 2px 8px; background-color: white}"
                                "QLabel:hover {background-color: #f0f0f0}");
        QString tagName = tagPairs[i].first;
        tagLabel->setProperty("tagName", tagName);
        connect(tagLabel, &ClickableLabel::clicked, this, [this, tagName]() {emit tagClicked(tagName);});
        currentLayout->insertWidget(currentLayout->count() - 1, tagLabel);
        currentWidth += data.second + 10;
    }
}

QString DetailPage::getTimeInfo(const QList<QPair<QString, int>> &tagPairs, const QString &dateStr)
{   // 时间
    QRegularExpression timePattern(R"(\d{4}年\d{1,2}月)");
    for (const auto &pair : tagPairs) {
        QRegularExpressionMatch match = timePattern.match(pair.first);
        if (match.hasMatch()) return match.captured();
    }
    if (dateStr.isEmpty()) return "TBA";
    if (dateStr.length() >= 7) return QString("%1年%2月").arg(dateStr.left(4)).arg(dateStr.sliced(5, 2).toInt());
    return "TBA";
}

void DetailPage::clearLayout() const
{   // 清空组件
    if (QLayout *layout = ui.frame_5->layout()) {
        QLayoutItem *item;
        while ((item = layout->takeAt(0))) {
            if (item->widget()) item->widget()->deleteLater();
            delete item;
        }
        delete layout;
    }
}

void DetailPage::onBackButtonClicked()
{   // 返回
    clearLayout();
    ui.textEdit_2->clear();
    ui.pushButton_23->setText("TBA");
    ui.pushButton_21->setText("|人评|#");
    ui.pushButton_25->setText("收藏/在看/抛弃");
    emit backButtonClicked();
}