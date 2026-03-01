#include "search_page.h"
#include <QLabel>
#include <QJsonArray>
#include <QMouseEvent>
#include "sql.h"
#include "api/bangumi_api.h"
#include "utils/image_util.h"

SearchPage::SearchPage(QWidget *parent) : QWidget(parent)
{
    // 初始化UI
    ui.setupUi(this);
    // 初始化连接
    setupConnections();
}

SearchPage::~SearchPage()
{
    delete statusLabel;
}

void SearchPage::setManagers(CacheImageUtil *cacheImage, BangumiAPI *api)
{   // 初始化实例
    cacheImageUtil = cacheImage;
    bangumiApi = api;
}

void SearchPage::setupConnections()
{   // 连接
    connect(ui.search_pushButton, &QPushButton::clicked, this, &SearchPage::onSearchLineEditReturnPressed);
    connect(ui.search_lineEdit, &QLineEdit::returnPressed, this, &SearchPage::onSearchLineEditReturnPressed);
    connect(ui.back_Button, &QPushButton::clicked, this, &SearchPage::onBackButtonClicked);
}

void SearchPage::updateComboBoxByType(int currentType) const
{   // 类型映射
    if (currentType == 7 || currentType == 8) ui.comboBox->setCurrentIndex(1);
    else if (currentType == 4) ui.comboBox->setCurrentIndex(2);
    else ui.comboBox->setCurrentIndex(0);
}

void SearchPage::searchByTag(const QString &tag)
{   // tag搜索
    if (tag.isEmpty()) return;
    ui.search_lineEdit->setText("tag:" + tag);
    doSearch("", tag);
}

void SearchPage::onSearchLineEditReturnPressed()
{   // keyword搜索
    QString keyword = ui.search_lineEdit->text().trimmed();
    if (keyword.isEmpty()) return;
    doSearch(keyword, "");
}

void SearchPage::doSearch(const QString &keyword, const QString &tag)
{   // 搜索
    QMap<int, int> typeMapping{{1, 1}, {2, 4}};
    int searchType = typeMapping.value(ui.comboBox->currentIndex(), 2);
    showSearchStatus("搜索中...");
    QJsonArray results = bangumiApi->searchSubjects(keyword, tag, searchType);
    clearSearchResults();
    if (!isVisible()) return;
    auto *layout = qobject_cast<QVBoxLayout*>(ui.scrollAreaWidgetContents->layout());
    const qsizetype resultCount = results.size();
    for (int i = 0; i < resultCount; ++i) layout->addWidget(createResultFrame(results[i].toObject().toVariantMap()));
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
    int subjectId = result["id"].toInt();
    QVariantMap resultData;
    resultData["subject_id"] = subjectId;
    resultData["subject_name"] = result["name"];
    resultData["subject_name_cn"] = result["name_cn"];
    resultData["subject_images_common"] = result["images"].toMap()["common"];
    resultData["subject_type"] = result["type"];
    resultData["type"] = 0;
    animationFrame->setProperty("resultData", resultData);
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
    QVariantMap images = result["images"].toMap();
    QString imageUrl = images.value("common").toString();
    if (!imageUrl.isEmpty()) ImageUtil::loadImageWithCache(cacheImageUtil, imageUrl, coverLabel, 15, false);
    else coverLabel->setText("暂无封面");
    horizontalLayout->addWidget(coverLabel);
    // 垂直布局
    auto *infoLayout = new QVBoxLayout();
    infoLayout->setContentsMargins(5, 0, 0, 0);
    // 标题
    auto *titleLabel = new QLabel(animationFrame);
    titleLabel->setFixedHeight(30);
    QString title = result.contains("name_cn") && !result["name_cn"].toString().isEmpty() ? result["name_cn"].toString() : result["name"].toString();
    titleLabel->setText(title);
    QFont font("微软雅黑", 13);
    titleLabel->setFont(font);
    infoLayout->addWidget(titleLabel);
    infoLayout->addStretch();
    horizontalLayout->addLayout(infoLayout);
    // 点击事件
    auto onClicked = [this, animationFrame, subjectId] {
        QVariantMap data = animationFrame->property("resultData").toMap();
        data["type"] = DatabaseManager::getCollectionBySubjectId(subjectId)["type"].toInt();
        emit showDetailPage(data);
    };
    animationFrame->installEventFilter(this);
    animationFrame->setProperty("clickHandler", QVariant::fromValue<void*>(new std::function(onClicked)));
    return animationFrame;
}

bool SearchPage::eventFilter(QObject *watched, QEvent *event)
{   // 点击事件
    if (event->type() == QEvent::MouseButtonPress) {
        auto *mouseEvent = dynamic_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            auto *frame = qobject_cast<QFrame*>(watched);
            if (auto *handler = static_cast<std::function<void()>*>(frame->property("clickHandler").value<void*>())) {
                (*handler)();
                return true;
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void SearchPage::clearSearchResults()
{   // 清理框架
    auto *layout = qobject_cast<QVBoxLayout*>(ui.scrollAreaWidgetContents->layout());
    delete statusLabel;
    statusLabel = nullptr;
    while (layout->count() > 0) {
        QLayoutItem *item = layout->takeAt(0);
        if (QWidget *widget = item->widget()) {
            if (auto *frame = qobject_cast<QFrame*>(widget)) delete static_cast<std::function<void()>*>(frame->property("clickHandler").value<void*>());
            delete widget;
        }
        delete item;
    }
}

void SearchPage::showSearchStatus(const QString &text)
{   // 显示搜索状态
    clearSearchResults();
    auto *layout = qobject_cast<QVBoxLayout*>(ui.scrollAreaWidgetContents->layout());
    statusLabel = new QLabel(text);
    statusLabel->setAlignment(Qt::AlignCenter);
    QFont font("微软雅黑", 14);
    statusLabel->setFont(font);
    statusLabel->setStyleSheet("color: #666; padding: 20px");
    layout->addWidget(statusLabel);
    layout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

void SearchPage::onBackButtonClicked()
{   // 返回
    clearSearchResults();
    emit backButtonClicked();;
}