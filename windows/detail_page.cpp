#include "detail_page.h"
#include <QTimer>
#include <QJsonArray>
#include <QMouseEvent>
#include <QDesktopServices>
#include "config.h"
#include "sql/sql.h"
#include "utils/menu_util.h"
#include "api/bangumi_api.h"
#include "utils/image_util.h"
#include "utils/star_rating_util.h"

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

void DetailPage::setManagers(CacheImageUtil *cacheImage, BangumiAPI *api, DatabaseManager *db)
{   // 初始化实例
    cacheImageUtil = cacheImage;
    bangumiAPI = api;
    dbManager = db;
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
    connect(ui.pushButton, &QPushButton::clicked, this, &DetailPage::onRatingButtonClicked);
    connect(ui.tabWidget, &QTabWidget::currentChanged, this, &DetailPage::onCharacterTab);
}

void DetailPage::setCollectionData(const CollectionData &data, const QString &progressText)
{   // 显示传入数据
    ui.tabWidget->setCurrentIndex(0);
    resetUI();
    currentData = data;
    if (currentData.subject_images_common.isEmpty()) {
        ui.cover_label_3->setText("暂无图片");
        ui.cover_label_3->setStyleSheet("QLabel {color: gray}");
    } else ImageUtil::loadImageWithCache(cacheImageUtil, currentData.subject_images_common, ui.cover_label_3, 15, true, true, QString("s%1.jpg").arg(currentData.subject_id));
    ui.textEdit->setText(currentData.subject_name_cn.isEmpty() ? currentData.subject_name : currentData.subject_name_cn);
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
    ui.pushButton_24->setText(progressText);
    const QString rateText = currentData.rate > 0 ? QString("我的评价: %1 分").arg(currentData.rate) : "未评价";
    ui.pushButton->setText(rateText);
    loadData();
}

void DetailPage::loadData()
{   // 加载数据
    const SubjectsData subjectData = dbManager->getSubjectById(currentData.subject_id);
    if (subjectData.subject_id != 0) updateDetailPage(subjectData);
    else {
        QTimer::singleShot(0, this, [this] {
            const QJsonObject subjectInfo = bangumiAPI->getSubjectInfo(currentData.subject_id, 3);
            if (!dbManager->insertOrUpdateSubject(subjectInfo)) qDebug() << currentData.subject_id << "失败";
            if (!isVisible()) return;
            updateDetailPage(dbManager->getSubjectById(currentData.subject_id));
        });
    }
    QTimer::singleShot(10, this, [this] {m_characters = dbManager->getCharactersWithPersonsBySubjectId(currentData.subject_id);});
}

void DetailPage::onEpisodeClicked()
{   // 打开选集页
    emit showEpisodePageRequested(currentData);
}

void DetailPage::onOpenBangumiPage() const
{   // 跳转Bangumi
    const QString baseUrl = getConfig("Bangumi/bangumi_base_url").toString();
    QDesktopServices::openUrl(QString("%1subject/%2").arg(baseUrl).arg(currentData.subject_id));
}

void DetailPage::onStatusButtonClicked()
{   // 改变状态
    int subjectType = currentData.subject_type;
    const int currentStatus = currentData.type;
    StatusSelector::showStatusSelector(ui.pushButton_26, subjectType, currentStatus, currentData.subject_id, bangumiAPI, dbManager,[this, subjectType](const int selectedStatus) {
        currentData.type = selectedStatus;
        ui.pushButton_26->setText(statusNamesMap.value(subjectType).value(selectedStatus));
    }, -20);
}

void DetailPage::updateDetailPage(const SubjectsData &subjectData)
{   // 显示数据
    QString score = QString::number(subjectData.rating_score);
    QString total = QString::number(subjectData.rating_total);
    QString rank = QString::number(subjectData.rating_rank);
    ui.pushButton_21->setText(QString("%1 | %2人评 | #%3").arg(score, total, rank));
    const int dropped = subjectData.dropped;
    const int doing = subjectData.doing;
    ui.pushButton_25->setText(QString("%1收藏 / %2在看 / %3抛弃").arg(subjectData.collect + subjectData.on_hold + dropped + subjectData.wish + doing).arg(doing).arg(dropped));
    ui.textEdit_2->setText(subjectData.summary);
    QList<QPair<QString, int>> allTagPairs, tagPairs;
    if (!subjectData.meta_tags.isEmpty()) for (const auto &value : QJsonDocument::fromJson(subjectData.meta_tags.toUtf8()).array()) allTagPairs.append(qMakePair(value.toString().trimmed(), 0));
    QJsonObject tagsObject = subjectData.tags;
    for (auto it = tagsObject.begin(); it != tagsObject.end(); ++it) tagPairs.append(qMakePair(it.key(), it.value().toInt()));
    std::sort(tagPairs.begin(), tagPairs.end(), [](const QPair<QString, int> &a, const QPair<QString, int> &b) {return b.second < a.second;});
    allTagPairs.append(tagPairs);
    tagsDisplay(allTagPairs);
    const QString timeTag = getTimeInfo(tagPairs, subjectData.date);
    ui.pushButton_23->setText(timeTag);
}

void DetailPage::tagsDisplay(const QList<QPair<QString, int>> &tagPairs)
{   // tag显示
    clearLayout();
    auto *mainLayout = new QVBoxLayout(ui.frame_5);
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    const QFontMetrics fm(ui.frame_5->font());
    const int maxWidth = ui.frame_5->width();
    QHBoxLayout *currentLayout = nullptr;
    QWidget *currentWidget = nullptr;
    int currentWidth = 0;
    QVector<QPair<QString, int>> labelData;
    for (const auto & [fst, snd] : tagPairs) {
        QString displayText;
        int textWidth;
        if (snd > 0) {
            displayText = QString("<span style='color:black'>%1</span> <span style='color:gray'>%2</span>").arg(fst).arg(snd);
            textWidth = fm.horizontalAdvance(fst + " " + QString::number(snd)) + 25;
        } else {
            displayText = QString("<span style='color:black'>%1</span>").arg(fst);
            textWidth = fm.horizontalAdvance(fst) + 25;
        }
        labelData.append(qMakePair(displayText, textWidth));
    }
    for (int i = 0; i < labelData.size(); ++i) {
        const auto & [fst, snd] = labelData[i];
        if (!currentLayout || currentWidth + snd > maxWidth) {
            currentWidget = new QWidget();
            currentLayout = new QHBoxLayout(currentWidget);
            currentLayout->setSpacing(8);
            currentLayout->setContentsMargins(0, 0, 0, 0);
            currentLayout->addStretch(1);
            mainLayout->addWidget(currentWidget);
            currentWidth = 0;
        }
        auto *tagLabel = new ClickableLabel(fst, currentWidget);
        tagLabel->setFixedSize(snd, fm.height() + 10);
        tagLabel->setStyleSheet("QLabel {border: 1px solid gray; border-radius: 10px; padding: 2px 8px; background-color: white}"
                                "QLabel:hover {background-color: #f0f0f0}");
        QString tagName = tagPairs[i].first;
        tagLabel->setProperty("tagName", tagName);
        connect(tagLabel, &ClickableLabel::clicked, this, [this, tagName] {emit tagClicked(tagName, currentData.subject_type);});
        currentLayout->insertWidget(currentLayout->count() - 1, tagLabel);
        currentWidth += snd + 10;
    }
}

QString DetailPage::getTimeInfo(const QList<QPair<QString, int>> &tagPairs, const QString &dateStr)
{   // 时间
    const QRegularExpression timePattern(R"(\d{4}年\d{1,2}月)");
    for (const auto & [fst, snd] : tagPairs) if (QRegularExpressionMatch match = timePattern.match(fst); match.hasMatch()) return match.captured();
    if (dateStr.isEmpty()) return "TBA";
    if (dateStr.length() >= 7) return QString("%1年%2月").arg(dateStr.left(4)).arg(dateStr.sliced(5, 2).toInt());
    return "TBA";
}

void DetailPage::onRatingButtonClicked()
{   // 评分
    if (m_starRating) {
        m_starRating->close();
        return;
    }
    m_starRating = new StarRatingWidget(10, this);
    m_starRating->setRate(currentData.rate);
    connect(m_starRating, &StarRatingWidget::ratingSelected, this, [this](const int rate) {
        currentData.rate = rate;
        ui.pushButton->setText(QString("我的评价: %1 分").arg(rate));
        m_starRating->close();
        ui.pushButton->setText("更改中...");
        if (bangumiAPI->updateCollection(currentData.subject_id, {{"rate", rate}}, 3)) {
            DatabaseManager::updateCollectionFields(currentData.subject_id, {{"rate", rate}}, false);
            ui.pushButton->setText(QString("我的评价: %1 分").arg(rate));
        }
        else ui.pushButton->setText("更改失败");
    });
    connect(m_starRating, &QObject::destroyed, this, [this] {m_starRating = nullptr;});
    const QPoint btnTopRight = ui.pushButton->mapToGlobal(QPoint(ui.pushButton->width(), 0));
    const QPoint btnCenter = ui.pushButton->mapToGlobal(ui.pushButton->rect().center());
    m_starRating->move(btnTopRight.x(), btnCenter.y() - m_starRating->height() / 2);
    m_starRating->show();
}

void DetailPage::onCharacterTab(const int index)
{   // 角色Tab
    if (index != 1) return;
    clearTab2();
    QWidget *content = ui.scrollAreaWidgetContents;
    auto *gridLayout = new QGridLayout(content);
    gridLayout->setSpacing(20);
    gridLayout->setContentsMargins(0, 20, 20, 20);
    constexpr int maxCols = 2;
    constexpr int cardWidth = 400;
    constexpr int imageSize = 60;
    auto createCard = [this](const CharacterData &characterData) -> QWidget* {
        auto *card = new QWidget();
        card->setFixedWidth(cardWidth);
        card->setStyleSheet("QWidget {background-color: #f9f9f9; border-radius: 8px}");
        auto *layout = new QHBoxLayout(card);
        auto *imageLabel = new QLabel(card);
        imageLabel->setFixedSize(imageSize, imageSize);
        layout->addWidget(imageLabel);
        const QString imageUrl = QString("https://api.bgm.tv/v0/characters/%1/image?type=grid").arg(characterData.character_id);
        ImageUtil::loadImageWithCache(cacheImageUtil, imageUrl, imageLabel, 10, true, true, QString("c%1.jpg").arg(characterData.character_id));
        auto *textLayout = new QVBoxLayout();
        textLayout->setSpacing(5);
        const QString name = characterData.character_name_cn.isEmpty() ? characterData.character_name : characterData.character_name_cn;
        auto *nameLabel = new QLabel(name, card);
        QFont nameFont = nameLabel->font();
        nameFont.setBold(true);
        nameFont.setPointSize(12);
        nameLabel->setFont(nameFont);
        textLayout->addWidget(nameLabel);
        QStringList allPersons;
        QString firstPerson;
        for (const auto &p : characterData.persons) {
            QString disp = p.person_name_cn.isEmpty() ? p.person_name : p.person_name_cn;
            if (firstPerson.isEmpty()) firstPerson = disp;
            allPersons << disp;
        }
        static QMap<int, QString> roleTypeMap = {{1, "主角"}, {2, "配角"}, {3, "客串"}, {4, "闲角"}};
        QString typeAndPerson = roleTypeMap.value(characterData.type, QString::number(characterData.type));
        if (!firstPerson.isEmpty()) typeAndPerson += " · " + firstPerson;
        auto *personLabel = new QLabel(typeAndPerson, card);
        if (allPersons.size() > 1) personLabel->setToolTip(allPersons.join(" "));
        textLayout->addWidget(personLabel);
        layout->addLayout(textLayout);
        layout->addStretch();
        return card;
    };
    int row = 0, col = 0;
    for (const auto &characterData : m_characters) {
        QWidget *card = createCard(characterData);
        gridLayout->addWidget(card, row, col);
        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }
    if (col != 0) gridLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum), row, col);
    auto *vSpacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
    gridLayout->addItem(vSpacer, row + 1, 0, 1, maxCols);
    content->setLayout(gridLayout);
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

void DetailPage::clearTab2() const
{   // 清理角色Tab
    const QWidget *content = ui.scrollAreaWidgetContents;
    qDeleteAll(content->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly));
    if (content->layout()) delete content->layout();
}

void DetailPage::resetUI() const
{   // 重置ui
    clearLayout();
    ui.textEdit->clear();
    ui.textEdit_2->clear();
    ui.cover_label_3->clear();
    ui.pushButton_24->setText("全 - 话");
    ui.pushButton_23->setText("TBA");
    ui.pushButton_21->setText(" | 人评 | #");
    ui.pushButton_25->setText("收藏 / 在看 / 抛弃");
}

void DetailPage::onBackButtonClicked()
{   // 返回
    m_characters.clear();
    clearTab2();
    resetUI();
    emit backButtonClicked();
}