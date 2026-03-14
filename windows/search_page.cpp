#include "search_page.h"
#include <QJsonArray>
#include <QMouseEvent>
#include "config.h"
#include "sql/sql.h"
#include "detail_page.h"
#include "api/bangumi_api.h"
#include "utils/image_util.h"
#include "utils/progress_util.h"
#include "utils/game_monitor_util.h"

SearchPage::SearchPage(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    applyTheme();
    setupConnections();
}

SearchPage::~SearchPage()
{
    delete statusLabel;
}

void SearchPage::setManagers(CacheImageUtil *cacheImage, BangumiAPI *api, DatabaseManager *db, GameMonitorUtil *gameMonitor)
{   // 初始化实例
    cacheImageUtil = cacheImage;
    bangumiApi = api;
    dbManager = db;
    gameMonitorUtil = gameMonitor;
    // 详情页
    if (!detailPage) {
        detailPage = new DetailPage(this);
        detailPage->setManagers(cacheImageUtil, bangumiApi, dbManager, gameMonitorUtil);
        ui.stackedWidget->addWidget(detailPage);
        connect(detailPage, &DetailPage::backButtonClicked, this, [this] {ui.stackedWidget->setCurrentIndex(0);});
        connect(detailPage, &DetailPage::showEpisodePageRequested, this, &SearchPage::showEpisodePageRequested);
        connect(detailPage, &DetailPage::tagClicked, this, [this](const QString &tag, const int subjectType) {searchByTag(tag, subjectType);});
    }
}

void SearchPage::applyTheme() const
{   // 主题
    const QColor color1 = getColor("color1", QColor("#fdf7ff"));
    ui.search_frame->setStyleSheet(QString("QFrame {background-color: %1}").arg(color1.name()));
}

void SearchPage::setupConnections()
{   // 连接
    connect(ui.search_pushButton, &QPushButton::clicked, this, &SearchPage::onSearchLineEditReturnPressed);
    connect(ui.search_lineEdit, &QLineEdit::returnPressed, this, &SearchPage::onSearchLineEditReturnPressed);
    connect(ui.back_Button, &QPushButton::clicked, this, &SearchPage::onBackButtonClicked);
}

void SearchPage::updateComboBoxByType(const int currentType) const
{   // 类型映射
    if (currentType == 7 || currentType == 8) ui.comboBox->setCurrentIndex(1);
    else if (currentType == 4) ui.comboBox->setCurrentIndex(2);
    else ui.comboBox->setCurrentIndex(0);
}

void SearchPage::searchByTag(const QString &tag, const int subjectType)
{   // tag搜索
    if (tag.isEmpty()) return;
    updateComboBoxByType(subjectType);
    ui.checkBox->setChecked(true);
    ui.search_lineEdit->setText(tag);
    doSearch("", tag);
}

void SearchPage::onSearchLineEditReturnPressed()
{   // 搜索事件
    const QString input = ui.search_lineEdit->text().trimmed();
    if (input.isEmpty()) return;
    if (ui.checkBox->isChecked()) doSearch("", input);
    else doSearch(input, "");
}

void SearchPage::doSearch(const QString &keyword, const QString &tag)
{   // 搜索
    ui.stackedWidget->setCurrentIndex(0);
    const QMap<int, int> typeMapping{{1, 1}, {2, 4}};
    const int searchType = typeMapping.value(ui.comboBox->currentIndex(), 2);
    showSearchStatus("搜索中...");
    QJsonArray results = bangumiApi->searchSubjects(keyword, tag, searchType, 3);
    clearSearchResults();
    if (!isVisible()) return;
    auto *layout = qobject_cast<QVBoxLayout*>(ui.scrollAreaWidgetContents->layout());
    const qsizetype resultCount = results.size();
    for (int i = 0; i < resultCount; ++i) {
        QVariantMap result = results[i].toObject().toVariantMap();
        layout->addWidget(createResultFrame(result));
    }
    if (resultCount == 0) showSearchStatus("未找到结果");
    layout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

QFrame *SearchPage::createResultFrame(const QVariantMap &result)
{
    // 主框架
    auto *animationFrame = new QFrame();
    animationFrame->setFixedSize(450, 130);
    animationFrame->setStyleSheet("QFrame{background-color: rgb(242, 236, 244)}");
    animationFrame->setCursor(Qt::PointingHandCursor);
    // 获取数据
    const int subjectId = result["id"].toInt();
    resultDataMap[subjectId] = result;
    animationFrame->setProperty("subjectId", subjectId);
    // 水平布局
    auto *horizontalLayout = new QHBoxLayout(animationFrame);
    horizontalLayout->setSpacing(0);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);
    // 封面
    auto *coverLabel = new QLabel(animationFrame);
    coverLabel->setFixedSize(100, 130);
    coverLabel->setStyleSheet("background-color: rgb(242, 236, 244);border-radius: 15px 0 0 15px");
    coverLabel->setAlignment(Qt::AlignCenter);
    coverLabel->setText("加载中...");
    coverLabel->setCursor(Qt::PointingHandCursor);
    const QVariantMap images = result["images"].toMap();
    const QString imageUrl = images.value("common").toString();
    if (!imageUrl.isEmpty()) ImageUtil::loadImageWithCache(cacheImageUtil, imageUrl, coverLabel, 15, false, true, QString("s%1.jpg").arg(subjectId));
    else coverLabel->setText("暂无封面");
    horizontalLayout->addWidget(coverLabel);
    // 垂直布局
    auto *infoLayout = new QVBoxLayout();
    infoLayout->setContentsMargins(5, 0, 0, 0);
    // 标题
    auto *titleLabel = new QLabel(animationFrame);
    titleLabel->setFixedHeight(30);
    const QString title = result.contains("name_cn") && !result["name_cn"].toString().isEmpty() ? result["name_cn"].toString() : result["name"].toString();
    titleLabel->setText(title);
    const QFont font("微软雅黑", 13);
    titleLabel->setFont(font);
    infoLayout->addWidget(titleLabel);
    infoLayout->addStretch();
    horizontalLayout->addLayout(infoLayout);
    animationFrame->installEventFilter(this);
    return animationFrame;
}

bool SearchPage::eventFilter(QObject *watched, QEvent *event)
{   // 点击事件
    if (event->type() == QEvent::MouseButtonPress) {
        if (const auto *mouseEvent = dynamic_cast<QMouseEvent*>(event); mouseEvent->button() == Qt::LeftButton) {
            if (const auto *frame = qobject_cast<QFrame*>(watched)) {
                if (int subjectId = frame->property("subjectId").toInt(); subjectId > 0 && resultDataMap.contains(subjectId)) {
                    QVariantMap original = resultDataMap[subjectId];
                    const CollectionData collectionData = DatabaseManager::getCollectionBySubjectId(subjectId);
                    CollectionData data = collectionData;
                    if (collectionData.type == 0) {
                        data.subject_id = subjectId;
                        data.subject_name = original["name"].toString();
                        data.subject_name_cn = original["name_cn"].toString();
                        data.subject_type = original["type"].toInt();
                        data.subject_volumes = original["volumes"].toInt();
                        data.subject_eps = original["eps"].toInt();
                        data.subject_date = original["date"].toString();
                    }
                    const QString progressText = computeProgressText(data, dbManager->getEpisodeAirdates({subjectId}));
                    detailPage->setCollectionData(data, progressText);
                    ui.stackedWidget->setCurrentWidget(detailPage);
                    return true;
                }
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void SearchPage::clearSearchResults()
{   // 清理框架
    resultDataMap.clear();
    auto *layout = qobject_cast<QVBoxLayout*>(ui.scrollAreaWidgetContents->layout());
    delete statusLabel;
    statusLabel = nullptr;
    while (layout->count() > 0) {
        const QLayoutItem *item = layout->takeAt(0);
        delete item->widget();
        delete item;
    }
}

void SearchPage::showSearchStatus(const QString &text)
{   // 显示搜索状态
    clearSearchResults();
    auto *layout = qobject_cast<QVBoxLayout*>(ui.scrollAreaWidgetContents->layout());
    statusLabel = new QLabel(text);
    statusLabel->setAlignment(Qt::AlignCenter);
    const QFont font("微软雅黑", 14);
    statusLabel->setFont(font);
    statusLabel->setStyleSheet("color: #666; padding: 20px");
    layout->addWidget(statusLabel);
    layout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

void SearchPage::onBackButtonClicked()
{   // 返回
    ui.stackedWidget->setCurrentIndex(0);
    clearSearchResults();
    if (detailPage) detailPage->resetUI();
    ui.checkBox->setChecked(false);
    emit backButtonClicked();
}