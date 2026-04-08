#include "search_page.h"
#include <QJsonArray>
#include "config.h"
#include "sql/sql.h"
#include "api/bangumi_api.h"
#include "utils/image_util.h"
#include "utils/context_menu_util.h"
#include "detail_page/detail_page.h"
#include "utils/game/game_monitor_util.h"

SearchPage::SearchPage(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    applyTheme();
    setupConnections();
}

void SearchPage::setManagers(CacheImageUtil *cacheImage, BangumiAPI *api, DatabaseManager *db, GameMonitorUtil *gameMonitor)
{
    cacheImageUtil = cacheImage;
    bangumiApi = api;
    dbManager = db;
    gameMonitorUtil = gameMonitor;
    if (!detailPage) {
        detailPage = new DetailPage(this);
        detailPage->setManagers(cacheImageUtil, bangumiApi, dbManager, gameMonitorUtil);
        ui.stackedWidget->addWidget(detailPage);
        connect(detailPage, &DetailPage::backButtonClicked, this, [this] {ui.stackedWidget->setCurrentIndex(0);});
        connect(detailPage, &DetailPage::showEpisodePageRequested, this, &SearchPage::showEpisodePageRequested);
        connect(detailPage, &DetailPage::tagClicked, this, [this](const QString &tag, const int subjectType) {searchByTag(tag, subjectType);});
    }
}

void SearchPage::applyTheme()
{   // 主题
    m_color2 = getConfig("Theme/color2", 0xf2ecf4).toString();
    m_color3 = getConfig("Theme/color3", 0xe1dbe4).toString();
}

void SearchPage::setupConnections()
{   // 连接
    updateNsfwCheckBox(getConfig("Bangumi/nsfw").toBool());
    connect(ui.btnSearch, &QPushButton::clicked, this, &SearchPage::onSearchLineEditReturnPressed);
    connect(ui.lineEditSearch, &QLineEdit::returnPressed, this, &SearchPage::onSearchLineEditReturnPressed);
    connect(ui.btnBack, &QPushButton::clicked, this, &SearchPage::onBackButtonClicked);
    setupLineEditCustomContextMenu(ui.lineEditSearch, CMO_Default);
}

void SearchPage::updateNsfwCheckBox(const bool checked) const
{   // 更新NSFW
    ui.checkBoxNsfw->setChecked(checked);
}

void SearchPage::updateComboBoxByType(const int currentType) const
{   // 类型映射
    ui.lineEditSearch->setFocus();
    if (currentType == 7 || currentType == 8) ui.comboSearchType->setCurrentIndex(1);
    else if (currentType == 4) ui.comboSearchType->setCurrentIndex(2);
    else ui.comboSearchType->setCurrentIndex(0);
}

void SearchPage::searchByTag(const QString &tag, const int subjectType)
{   // tag搜索
    updateComboBoxByType(subjectType);
    ui.checkBoxTag->setChecked(true);
    ui.lineEditSearch->setText(tag);
    searchSubject("", tag);
}

void SearchPage::onSearchLineEditReturnPressed()
{   // 搜索事件
    const QString input = ui.lineEditSearch->text().trimmed();
    if (input.isEmpty()) return;
    if (ui.checkBoxTag->isChecked()) searchSubject("", input);
    else searchSubject(input, "");
}

void SearchPage::searchSubject(const QString &keyword, const QString &tag)
{   // 搜索
    ui.stackedWidget->setCurrentIndex(0);
    static const QMap<int, int> typeMapping{{0, 2}, {1, 1}, {2, 4}};
    showSearchStatus("搜索中...");
    bangumiApi->searchSubjectsWithPost(keyword, tag,typeMapping.value(ui.comboSearchType->currentIndex()), ui.checkBoxNsfw->isChecked(), 3, [this](const QJsonArray &results, const QString &error) {
        if (!error.isEmpty()) return showSearchStatus(error);
        clearSearchResults();
        if (!isVisible()) return;
        auto *layout = qobject_cast<QVBoxLayout*>(ui.scrollAreaWidgetContents->layout());
        const qsizetype resultCount = results.size();
        for (int i = 0; i < resultCount; ++i) layout->addWidget(createResult(results[i].toObject().toVariantMap()));
        if (resultCount == 0) showSearchStatus("未找到结果");
        else layout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    });
}

QPushButton *SearchPage::createResult(const QVariantMap &result)
{   // 创建搜索结果组件
    const int subjectId = result["id"].toInt();
    auto *button = new QPushButton();
    button->setFixedHeight(130);
    button->setStyleSheet(QString("QPushButton {background-color: %1; border: none; border-radius: 15px}"
                                  "QPushButton:hover {background-color: %2}").arg(m_color2.name(), m_color3.name()));
    auto *horizontalLayout = new QHBoxLayout(button);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);
    auto *coverLabel = new QLabel(button);
    coverLabel->setFixedSize(100, 130);
    ImageUtil::loadImageWithCache(cacheImageUtil, QString("https://api.bgm.tv/v0/subjects/%1/image?type=common").arg(subjectId), coverLabel, 15, false, true, QString("s%1.jpg").arg(subjectId));
    horizontalLayout->addWidget(coverLabel);
    auto *infoLayout = new QVBoxLayout();
    auto *titleLabel = new QLabel(button);
    titleLabel->setText(result["name_cn"].toString().isEmpty() ? result["name"].toString() : result["name_cn"].toString());
    const QFont font("微软雅黑", 13);
    titleLabel->setFont(font);
    infoLayout->addWidget(titleLabel);
    infoLayout->addStretch();
    horizontalLayout->addLayout(infoLayout);
    connect(button, &QPushButton::clicked, this, [this, subjectId] {
        detailPage->setCollectionData(subjectId, "");
        ui.stackedWidget->setCurrentWidget(detailPage);
    });
    return button;
}

void SearchPage::showSearchStatus(const QString &text)
{   // 显示搜索状态
    clearSearchResults();
    auto *layout = qobject_cast<QVBoxLayout*>(ui.scrollAreaWidgetContents->layout());
    statusLabel = new QLabel(text);
    statusLabel->setAlignment(Qt::AlignCenter);
    const QFont font("微软雅黑", 14);
    statusLabel->setFont(font);
    statusLabel->setStyleSheet("color: #666");
    layout->addWidget(statusLabel);
    layout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

void SearchPage::clearSearchResults()
{   // 清理框架
    auto *layout = qobject_cast<QVBoxLayout*>(ui.scrollAreaWidgetContents->layout());
    delete statusLabel;
    statusLabel = nullptr;
    while (layout->count() > 0) {
        const QLayoutItem *item = layout->takeAt(0);
        delete item->widget();
        delete item;
    }
}

void SearchPage::onBackButtonClicked()
{   // 返回
    ui.stackedWidget->setCurrentIndex(0);
    clearSearchResults();
    if (detailPage) detailPage->resetUI();
    ui.checkBoxTag->setChecked(false);
    emit backButtonClicked();
}