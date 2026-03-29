#include "detail_page.h"
#include <QTimer>
#include <QJsonArray>
#include <QMouseEvent>
#include <QDesktopServices>
#include "../config.h"
#include "../sql/sql.h"
#include "clickable_label.h"
#include "../utils/menu_util.h"
#include "../api/bangumi_api.h"
#include "score_chart_widget.h"
#include "../utils/image_util.h"
#include "../utils/progress_util.h"
#include "../utils/star_rating_util.h"
#include "../utils/game_monitor_util.h"
#include "../utils/context_menu_util.h"

DetailPage::DetailPage(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    statusNamesMap = {
        {1, {{0, "收藏"}, {1, "想读"}, {2, "读过"}, {3, "在读"}, {4, "搁置"}, {5, "抛弃"}}},
        {2, {{0, "追番"}, {1, "想看"}, {2, "看过"}, {3, "在看"}, {4, "搁置"}, {5, "抛弃"}}},
        {4, {{0, "收藏"}, {1, "想玩"}, {2, "玩过"}, {3, "在玩"}, {4, "搁置"}, {5, "抛弃"}}},
        {7, {{0, "收藏"}, {1, "想读"}, {2, "读过"}, {3, "在读"}, {4, "搁置"}, {5, "抛弃"}}},
        {8, {{0, "收藏"}, {1, "想读"}, {2, "读过"}, {3, "在读"}, {4, "搁置"}, {5, "抛弃"}}}};
    applyTheme();
    setupConnections();
    installEventFilter(this);
}

void DetailPage::setManagers(CacheImageUtil *cacheImage, BangumiAPI *api, DatabaseManager *db, GameMonitorUtil *gameMonitor)
{   // 初始化实例
    cacheImageUtil = cacheImage;
    bangumiAPI = api;
    dbManager = db;
    gameMonitorUtil = gameMonitor;
}

void DetailPage::applyTheme()
{   // 主题
    const QColor color1 = getColor("color1", 0xfdf7ff);
    m_color2 = getColor("color2", 0xf2ecf4);
    m_color3 = getColor("color3", 0xe1dbe4);
    const QColor color4 = getColor("color4", 0xf2ecf4);
    const QColor color5 = getColor("color5", 0xe1dbe4);
    ui.mainFrame->setStyleSheet(QString("QFrame {background-color: %1}").arg(color1.name()));
    ui.btnStatus->setStyleSheet(QString("QPushButton {background-color: %1; border-radius:15px}"
                                        "QPushButton:hover {background-color: %2}").arg(color4.name(), color5.name()));
    ui.btnAction->setStyleSheet(QString("QPushButton {background-color: %1; border-radius:15px}"
                                        "QPushButton:hover {background-color: %2}").arg(color4.name(), color5.name()));
    ui.btnRating->setStyleSheet(QString("QPushButton {background-color: %1; border-radius:15px; padding: 2px 10px}"
                                        "QPushButton:hover {background-color: %2}").arg(color4.name(), color5.name()));
}

void DetailPage::setupConnections()
{   // 连接
    connect(ui.btnBack, &QPushButton::clicked, this, &DetailPage::onBackButtonClicked);
    connect(ui.btnAction, &QPushButton::clicked, this, &DetailPage::onEpisodeClicked);
    connect(ui.btnOpenUrl, &QPushButton::clicked, this, &DetailPage::onOpenBangumiPage);
    connect(ui.btnStatus, &QPushButton::clicked, this, &DetailPage::onStatusButtonClicked);
    connect(ui.btnRating, &QPushButton::clicked, this, &DetailPage::onRatingButtonClicked);
    connect(ui.tabWidget, &QTabWidget::currentChanged, this, &DetailPage::clickOnTab);
    setupLineEditCustomContextMenu(ui.lineEditTitle, CMO_Copy | CMO_SelectAll);
    setupTextEditCustomContextMenu(ui.textEditSummary, CMO_Default);
    ui.lineEditTitle->setCursor(Qt::IBeamCursor);
}

void DetailPage::setCollectionData(int subjectId, const QString &progressText)
{   // 加载数据
    ui.tabWidget->setCurrentIndex(0);
    resetUI();
    m_subjectData = DatabaseManager::getCollectionBySubjectId(subjectId);
    const QString imageUrl = QString("https://api.bgm.tv/v0/subjects/%1/image?type=common").arg(subjectId);
    ImageUtil::loadImageWithCache(cacheImageUtil, imageUrl, ui.labelCover, 15, true, true, QString("s%1.jpg").arg(subjectId));
    ui.btnRating->setText(m_subjectData.rate > 0 ? QString("我的评价: %1 分").arg(m_subjectData.rate) : "未评价");
    const SubjectsData subjectData = dbManager->getSubjectById(subjectId);
    if (subjectData.subjectId == 0) {
        bangumiAPI->getSubjectInfo(subjectId, 3, [this, subjectId, progressText](const QJsonObject &subjectInfo, const QString &error) {
            if (!error.isEmpty() || subjectInfo.isEmpty()) return;
            if (!dbManager->insertOrUpdateSubject(subjectInfo)) qDebug() << subjectId << "失败";
            if (!isVisible()) return;
            mergeSubjectData(dbManager->getSubjectById(subjectId));
            updateDetailPage(progressText);
        });
    } else {
        mergeSubjectData(subjectData);
        updateDetailPage(progressText);
    }
    QTimer::singleShot(50, this, [this, subjectId] {
        m_characters = dbManager->getCharacters(subjectId);
        m_relations = dbManager->getSubjectRelations(subjectId);
        m_persons = dbManager->getPersons(subjectId);
    });
}

void DetailPage::mergeSubjectData(const SubjectsData &subjectData)
{   // 合并subjectData
    const int userType = m_subjectData.type;
    const int userRate = m_subjectData.rate;
    const int userEpStatus = m_subjectData.epStatus;
    const int userVolStatus = m_subjectData.volStatus;
    m_subjectData = subjectData;
    m_subjectData.type = userType;
    m_subjectData.rate = userRate;
    m_subjectData.epStatus = userEpStatus;
    m_subjectData.volStatus = userVolStatus;
}

void DetailPage::onEpisodeClicked()
{   // 打开选集页
    if (m_subjectData.subjectType == 4) gameMonitorUtil->startGame(m_subjectData.subjectId, m_gameData[m_subjectData.subjectId]);
    else emit showEpisodePageRequested(m_subjectData);
}

void DetailPage::onOpenBangumiPage() const
{   // 跳转Bangumi
    const QString baseUrl = getConfig("Bangumi/bangumi_base_url").toString();
    QDesktopServices::openUrl(QString("%1subject/%2").arg(baseUrl).arg(m_subjectData.subjectId));
}

void DetailPage::onStatusButtonClicked()
{   // 改变状态
    int subjectType = m_subjectData.subjectType;
    StatusSelector::showStatusSelector(ui.btnStatus, subjectType, m_subjectData.type, m_subjectData.subjectId, bangumiAPI, dbManager,[this, subjectType](const int selectedStatus) {
        m_subjectData.type = selectedStatus;
        ui.btnStatus->setText(statusNamesMap.value(subjectType).value(selectedStatus));
        emit refresh();
    });
}

void DetailPage::updateDetailPage(const QString &progressText)
{   // 显示数据
    ui.lineEditTitle->setText(m_subjectData.nameCn.isEmpty() ? m_subjectData.name : m_subjectData.nameCn);
    QLabel *targetLabel = m_subjectData.subjectType == 4 ? ui.labelStatus : ui.labelProgress;
    targetLabel->setText(progressText.isEmpty() ? computeProgressText(m_subjectData, dbManager->getEpisodeAirdates({m_subjectData.subjectId})) : progressText);
    ui.btnStatus->setText(statusNamesMap.value(m_subjectData.subjectType).value(m_subjectData.type));
    ui.btnAction->setText(m_subjectData.subjectType == 2 ? "选集" : m_subjectData.subjectType == 4 ? "启动" : "进度");
    const int total = std::reduce(m_subjectData.scoreDetails.begin(), m_subjectData.scoreDetails.end(), 0);
    ui.labelRatingInfo->setText(QString("%1 | %2人评 | #%3").arg(m_subjectData.ratingScore).arg(total).arg(m_subjectData.ratingRank));
    const int dropped = m_subjectData.dropped;
    const int doing = m_subjectData.doing;
    ui.labelCollectionStats->setText(QString("%1收藏 / %2在看 / %3抛弃").arg(m_subjectData.collect + m_subjectData.onHold + dropped + m_subjectData.wish + doing).arg(doing).arg(dropped));
    ui.textEditSummary->setText(m_subjectData.summary);
    QList<QPair<QString, int>> allTagPairs, tagPairs;
    if (!m_subjectData.metaTags.isEmpty()) for (const auto &value : QJsonDocument::fromJson(m_subjectData.metaTags.toUtf8()).array()) allTagPairs.append(qMakePair(value.toString().trimmed(), 0));
    QJsonObject tagsObject = m_subjectData.tags;
    for (auto it = tagsObject.begin(); it != tagsObject.end(); ++it) tagPairs.append(qMakePair(it.key(), it.value().toInt()));
    std::sort(tagPairs.begin(), tagPairs.end(), [](const QPair<QString, int> &a, const QPair<QString, int> &b) {return b.second < a.second;});
    allTagPairs.append(tagPairs);
    m_currentTagPairs = allTagPairs;
    QMetaObject::invokeMethod(this, [this] {tagsDisplay(m_currentTagPairs);}, Qt::QueuedConnection);
    if (m_subjectData.subjectType == 4) {
        m_gameData = DatabaseManager::getGameData({m_subjectData.subjectId});
        ui.labelProgress->setText(QString("已玩 %1 小时").arg(m_gameData[m_subjectData.subjectId].playDuration / 3600.0));
    } else ui.labelStatus->setText(getTimeInfo(tagPairs, m_subjectData.date));
    setupScoreChart(m_subjectData.scoreDetails, total);
}

void DetailPage::setupScoreChart(const QVector<int> &scoreDetails, const int total)
{   // 设置评分分布表
    auto *chart = new ScoreChartWidget(this);
    chart->setData(scoreDetails, total);
    auto *layout = qobject_cast<QHBoxLayout*>(ui.statsFrame->layout());
    layout->insertWidget(layout->count(), chart);
    m_scoreChartWidget = chart;
}

void DetailPage::tagsDisplay(const QList<QPair<QString, int>> &tagPairs)
{   // tag显示
    clearLayout();
    auto *mainLayout = new QVBoxLayout(ui.frameTags);
    QFont font = ui.frameTags->font();
    font.setPixelSize(14);
    const QFontMetrics fm(font);
    const int maxWidth = ui.frameTags->width();
    QHBoxLayout *currentLayout = nullptr;
    QWidget *currentWidget = nullptr;
    int currentWidth = 0;
    QVector<QPair<QString, int>> labelData;
    for (const auto &[fst, snd] : tagPairs) {
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
        const auto &[displayText, textWidth] = labelData[i];
        if (!currentLayout || currentWidth + textWidth > maxWidth) {
            currentWidget = new QWidget();
            currentLayout = new QHBoxLayout(currentWidget);
            currentLayout->setContentsMargins(0, 0, 0, 0);
            currentLayout->addStretch(1);
            mainLayout->addWidget(currentWidget);
            currentWidth = 0;
        }
        auto *tagLabel = new ClickableLabel(displayText, currentWidget);
        tagLabel->setFont(font);
        tagLabel->setFixedSize(textWidth, fm.height() + 10);
        tagLabel->setStyleSheet(QString("QLabel {background-color: %1; border-radius: 10px; padding: 2px 8px}"
                                        "QLabel:hover {background-color: %2}").arg(m_color2.name(), m_color3.name()));
        QString tagName = tagPairs[i].first;
        tagLabel->setProperty("tagName", tagName);
        connect(tagLabel, &ClickableLabel::clicked, this, [this, tagName] {emit tagClicked(tagName, m_subjectData.subjectType);});
        currentLayout->insertWidget(currentLayout->count() - 1, tagLabel);
        currentWidth += textWidth + 10;
    }
}

void DetailPage::resizeEvent(QResizeEvent *event)
{   // 窗口大小改变事件
    if (!m_currentTagPairs.isEmpty() && isVisible()) tagsDisplay(m_currentTagPairs);
}

QString DetailPage::getTimeInfo(const QList<QPair<QString, int>> &tagPairs, const QString &dateStr)
{   // 时间
    static const QRegularExpression timePattern(R"(\d{4}年\d{1,2}月)");
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
    m_starRating->setRate(m_subjectData.rate);
    connect(m_starRating, &StarRatingWidget::ratingSelected, this, [this](const int rate) {
        m_subjectData.rate = rate;
        ui.btnRating->setText(QString("我的评价: %1 分").arg(rate));
        m_starRating->close();
        ui.btnRating->setText("更改中...");
        bangumiAPI->updateCollection(m_subjectData.subjectId, {{"rate", rate}}, 3, [this, rate](const bool success, const QString &error) {
            if (success && error.isEmpty()) {
                DatabaseManager::updateCollectionFields(m_subjectData.subjectId, {{"rate", rate}}, false);
                if (!isVisible()) return;
                ui.btnRating->setText(QString("我的评价: %1 分").arg(rate));
                emit refresh();
            } else ui.btnRating->setText("更改失败");
        });
    });
    connect(m_starRating, &QObject::destroyed, this, [this] {m_starRating = nullptr;});
    const QPoint btnTopRight = ui.btnRating->mapToGlobal(QPoint(ui.btnRating->width(), 0));
    const QPoint btnCenter = ui.btnRating->mapToGlobal(ui.btnRating->rect().center());
    m_starRating->move(btnTopRight.x(), btnCenter.y() - m_starRating->height() / 2 - 3);
    m_starRating->show();
}

void DetailPage::clickOnTab(const int index)
{   // 点击Tab
    if (index == 1) {
        if (characterTabInitialized) return;
        onCharacterTab();
        characterTabInitialized = true;
    }
    else if (index == 2) {
        if (relationTabInitialized) return;
        onRelationTab();
        relationTabInitialized = true;
    }
    else if (index == 3) {
        if (staffTabInitialized) return;
        onStaffTab();
        staffTabInitialized = true;
    }
}

template<typename DataType, typename CreateFunc>
void DetailPage::setupTabTemplate(QWidget *content, const QVector<DataType> &data, CreateFunc createCardFunc)
{   // Tab模板
    const auto gridLayout = new QGridLayout();
    gridLayout->setSpacing(20);
    constexpr int maxCols = 2;
    int row = 0, col = 0;
    for (const auto& item : data) {
        QWidget *card = createCardFunc(item);
        gridLayout->addWidget(card, row, col);
        if (++col >= maxCols) {
            col = 0;
            ++row;
        }
    }
    if (row == 0 && col == 1) {
        const auto placeholder = new QWidget(content);
        placeholder->setFixedWidth(400);
        gridLayout->addWidget(placeholder, 0, 1);
    }
    auto *vSpacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
    gridLayout->addItem(vSpacer, row + 1, 0, 1, maxCols);
    content->setLayout(gridLayout);
}

void DetailPage::onCharacterTab()
{   // 角色Tab
    static const QMap<int, QString> roleTypeMap = {{1, "主角"}, {2, "配角"}, {3, "客串"}, {4, "闲角"}};
    setupTabTemplate(ui.charactersContent, m_characters, [this](const CharacterData& characterData) -> QWidget *{
        auto *card = new QWidget();
        card->setFixedWidth(400);
        card->setStyleSheet(QString("QWidget {background-color: %1; border-radius: 8px}").arg(m_color2.name()));
        auto *layout = new QHBoxLayout(card);
        auto *imageLabel = new QLabel(card);
        imageLabel->setFixedSize(60, 60);
        layout->addWidget(imageLabel);
        const QString imageUrl = QString("https://api.bgm.tv/v0/characters/%1/image?type=grid").arg(characterData.characterId);
        ImageUtil::loadImageWithCache(cacheImageUtil, imageUrl, imageLabel, 10, true, true, QString("c%1.jpg").arg(characterData.characterId));
        auto *textLayout = new QVBoxLayout();
        const QString name = characterData.characterNameCn.isEmpty() ? characterData.characterName : characterData.characterNameCn;
        auto *nameLabel = new QLabel(name, card);
        QFont nameFont = nameLabel->font();
        nameFont.setBold(true);
        nameFont.setPointSize(12);
        nameLabel->setFont(nameFont);
        textLayout->addWidget(nameLabel);
        QStringList allPersons;
        QString firstPerson;
        for (const auto &p : characterData.persons) {
            QString disp = p.personNameCn.isEmpty() ? p.personName : p.personNameCn;
            if (firstPerson.isEmpty()) firstPerson = disp;
            allPersons << disp;
        }
        QString typeAndPerson = roleTypeMap.value(characterData.type, QString::number(characterData.type));
        if (!firstPerson.isEmpty()) typeAndPerson += " · " + firstPerson;
        auto *personLabel = new QLabel(typeAndPerson, card);
        if (allPersons.size() > 1) personLabel->setToolTip(allPersons.join(" "));
        textLayout->addWidget(personLabel);
        layout->addLayout(textLayout);
        layout->addStretch();
        return card;
    });
}

void DetailPage::onRelationTab()
{   // 相关条目Tab
    static const QMap<int, QString> relationMap = {{2, "前传"}, {3, "续集"}, {4, "总集篇"}, {5, "全集"}, {6, "番外篇"}, {11, "衍生"}, {1003, "单行本"}};
    setupTabTemplate(ui.relationsContent, m_relations, [this](const SubjectRelationData& relation) -> QWidget* {
        auto *card = new QPushButton();
        card->setFixedSize(400, 100);
        card->setStyleSheet(QString("QPushButton {background-color: %1; border-radius: 8px}"
                                    "QPushButton:hover {background-color: %2}").arg(m_color2.name(), m_color3.name()));
        auto *cardLayout = new QHBoxLayout(card);
        cardLayout->setContentsMargins(0, 0, 0, 0);
        auto *imageLabel = new QLabel(card);
        imageLabel->setFixedSize(75, 100);
        cardLayout->addWidget(imageLabel);
        const QString imageUrl = QString("https://api.bgm.tv/v0/subjects/%1/image?type=common").arg(relation.subject.subjectId);
        ImageUtil::loadImageWithCache(cacheImageUtil, imageUrl, imageLabel, 10, false, true, QString("c%1.jpg").arg(relation.subject.subjectId));
        auto *textLayout = new QVBoxLayout();
        const QString title = relation.subject.nameCn.isEmpty() ? relation.subject.name : relation.subject.nameCn;
        auto *titleLabel = new QLabel(title, card);
        QFont titleFont = titleLabel->font();
        titleFont.setBold(true);
        titleLabel->setFont(titleFont);
        titleLabel->setWordWrap(true);
        textLayout->addWidget(titleLabel);
        const QString relText = relationMap.value(relation.relationType, QString::number(relation.relationType));
        auto *typeLabel = new QLabel(relText, card);
        textLayout->addWidget(typeLabel);
        cardLayout->addLayout(textLayout);
        cardLayout->addStretch();
        connect(card, &QPushButton::clicked, this, [this, subjectId = relation.subject.subjectId] {
            m_historyStack.push(m_subjectData.subjectId);
            setCollectionData(subjectId, "");
        });
        return card;
    });
}

void DetailPage::onStaffTab()
{   // 制作人员Tab
    static const QMap<int, QString> positionMap = {
        {1, "原作"}, {2, "导演"}, {3, "脚本"}, {4, "分镜"}, {5, "演出"}, {6, "音乐"}, {7, "人物原案"}, {8, "人物设定"}, {9, "构图"}, {10, "系列构成"},
        {11, "美术监督"}, {13, "色彩设计"}, {14, "总作画监督"}, {15, "作画监督"}, {16, "机械设定"}, {17, "摄影监督"}, {18, "监修"}, {19, "道具设计"}, {20, "原画"},
        {21, "第二原画"}, {22, "动画检查"}, {23, "助理制片人"}, {24, "制作助理"}, {25, "背景美术"}, {26, "色彩指定"}, {27, "数码绘图"}, {28, "剪辑"}, {29, "原案"}, {30, "主题歌编曲"},
        {31, "主题歌作曲"}, {32, "主题歌作词"}, {33, "主题歌演出"}, {34, "插入歌演出"}, {35, "企画"}, {36, "企划制作人"}, {37, "制作管理"}, {38, "宣传"}, {39, "录音"}, {40, "录音助理"},
        {41, "系列监督"}, {42, "製作"}, {43, "设定"}, {44, "音响监督"}, {45, "音响"}, {46, "音效"}, {47, "特效"}, {48, "配音监督"}, {49, "联合导演"}, {50, "背景设定"},
        {51, "补间动画"}, {52, "执行制片人"}, {53, "助理制片人"}, {54, "制片人"}, {55, "音乐助理"}, {56, "制作进行"}, {57, "演员监督"}, {58, "总制片人"}, {59, "联合制片人"}, {60, "台词编辑"},
        {61, "后期制片协调"}, {62, "制作助理"}, {63, "制作"}, {64, "制作协调"}, {65, "音乐制作"}, {66, "特别鸣谢"}, {67, "动画制作"}, {69, "CG 导演"}, {70, "机械作画监督"},
        {71, "美术设计"}, {72, "副导演"}, {73, "OP・ED 分镜"}, {74, "总导演"}, {75, "3DCG"}, {76, "制作协力"}, {77, "动作作画监督"}, {80, "监制"},
        {81, "协力"}, {82, "摄影"}, {83, "制作进行协力"}, {84, "设定制作"}, {85, "音乐制作人"}, {86, "3DCG 导演"}, {87, "动画制片人"}, {88, "特效作画监督"}, {89, "主演出"}, {90, "作画监督助理"},
        {91, "演出助理"}, {92, "主动画师"}, {93, "上色"}, {94, "上色检查"}, {95, "色彩检查"}, {96, "美术板"}, {97, "美术"}, {98, "印象板"}, {99, "2D 设计"}, {100, "3D 设计"},
        {101, "技术导演"}, {102, "特技导演"}, {103, "色彩脚本"}, {104, "分镜协力"}, {105, "分镜抄写"}, {106, "副人物设定"}, {107, "客座人物设定"}, {108, "构图监修"}, {109, "构图作画监督"}, {110, "总作画监督助理"},
        {111, "道具作画监督"}, {112, "概念设计"}, {113, "服装设计"}, {114, "标题设计"}, {115, "设定协力"}, {116, "音乐监督"}, {117, "选曲"}, {118, "插入歌作词"}, {119, "插入歌作曲"}, {120, "插入歌编曲"},
        {121, "创意制片人"}, {122, "副制片人"}, {123, "制作统括"}, {124, "现场制片人"}, {125, "文艺制作"}, {127, "企画协力"}, {128, "OP・ED 演出"}, {129, "Bank 分镜演出"}, {130, "Live 分镜演出"},
        {131, "剧中剧分镜演出"}, {132, "剧中剧人设"}, {133, "视觉导演"}, {134, "创意总监"}, {135, "特摄效果"}, {136, "视觉效果"}, {137, "动作导演"}, {138, "转场绘"}, {139, "插画"}, {140, "角色作画监督"},
        {141, "作画监修"}, {142, "机设原案"}, {143, "概念艺术"}, {144, "视觉概念"}, {145, "画面设计"}, {146, "怪物设计"}, {147, "故事概念"}, {148, "剧本协调"}, {149, "脚本协力"}, {150, "副系列构成"},
        {151, "构成协力"}, {152, "录音工作室"}, {153, "整音"}, {154, "音响制作担当"}, {155, "在线剪辑"}, {156, "离线剪辑"}, {157, "3D 动画师"}, {158, "CG 制作人"}, {159, "宣传制片人"}, {160, "美术制作人"},
        {161, "音响制作人"}, {162, "CG 制作进行"}, {163, "美术制作进行"}, {164, "美术监督助理"}, {165, "色彩设计助理"}, {166, "摄影监督助理"}, {167, "制作管理助理"}, {168, "设定制作助理"},
        {1001, "开发"}, {1002, "发行"}, {1003, "游戏设计师"}, {1004, "剧本"}, {1005, "美工"}, {1006, "音乐"}, {1007, "关卡设计"}, {1008, "人物设定"}, {1009, "主题歌作曲"}, {1010, "主题歌作词"},
        {1011, "主题歌演出"}, {1012, "插入歌演出"}, {1013, "原画"}, {1014, "动画制作"}, {1015, "原作"}, {1016, "导演"}, {1017, "动画监督"}, {1018, "制作总指挥"}, {1019, "QC"}, {1020, "动画剧本"},
        {1021, "程序"}, {1022, "协力"}, {1023, "CG 监修"}, {1024, "SD原画"}, {1025, "背景"}, {1026, "监修"}, {1027, "系列构成"}, {1028, "企画"}, {1029, "机械设定"}, {1030, "音响监督"},
        {1031, "作画监督"}, {1032, "制作人"}, {1033, "海报"},
        {2001, "作者"}, {2002, "作画"}, {2003, "插图"}, {2004, "出版社"}, {2005, "连载杂志"}, {2006, "译者"}, {2007, "原作"}, {2008, "客串"}, {2009, "人物原案"}, {2010, "脚本"},
        {2011, "书系"}, {2012, "出品方"}, {2013, "图书品牌"}
    };
    QWidget *content = ui.staffContent;
    QMap<int, QVector<QPair<int, QString>>> grouped;
    for (const auto &[person_id, position, name, name_cn] : m_persons) grouped[position].append({person_id, name_cn.isEmpty() ? name : name_cn});
    QList<int> displayOrder;
    if (grouped.contains(67)) displayOrder << 67;
    QList<int> remainingKeys = grouped.keys();
    std::sort(remainingKeys.begin(), remainingKeys.end());
    for (int key : remainingKeys) if (!displayOrder.contains(key)) displayOrder.append(key);
    auto *mainLayout = new QVBoxLayout(content);
    const QString bangumiBaseUrl = getConfig("Bangumi/bangumi_base_url").toString();
    for (int posKey : displayOrder) {
        const auto &persons = grouped[posKey];
        auto *row = new QFrame(content);
        row->setStyleSheet(QString("QFrame {background-color: %1; font-size: 15px}").arg(m_color2.name()));
        auto *layout = new QHBoxLayout(row);
        auto *posLabel = new QLabel(positionMap.value(posKey, QString::number(posKey)), row);
        posLabel->setFixedWidth(110);
        QFont font = posLabel->font();
        font.setBold(true);
        posLabel->setFont(font);
        auto *nameLabel = new QLabel(row);
        nameLabel->setWordWrap(true);
        nameLabel->setOpenExternalLinks(true);
        QStringList personLinks;
        for (const auto &[personId, personName] : persons) personLinks << QString("<a href=\"%1person/%2\">%3</a>").arg(bangumiBaseUrl).arg(personId).arg(personName);
        nameLabel->setText(personLinks.join("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"));
        layout->addWidget(posLabel);
        layout->addWidget(nameLabel, 1);
        mainLayout->addWidget(row);
    }
    mainLayout->addStretch();
}

bool DetailPage::eventFilter(QObject *watched, QEvent *event)
{   // 事件过滤器
    if (event->type() == QEvent::MouseButtonPress) {
        if (dynamic_cast<QMouseEvent*>(event)->button() == Qt::LeftButton) {
            ui.lineEditTitle->deselect();
            QTextCursor cursor = ui.textEditSummary->textCursor();
            cursor.clearSelection();
            ui.textEditSummary->setTextCursor(cursor);
        }
    }
    return QWidget::eventFilter(watched, event);
}

void DetailPage::clearLayout() const
{   // 清空组件
    if (QLayout *layout = ui.frameTags->layout()) {
        QLayoutItem *item;
        while ((item = layout->takeAt(0))) {
            if (item->widget()) item->widget()->deleteLater();
            delete item;
        }
        delete layout;
    }
}

void DetailPage::clearHistory()
{   // 清理历史
    m_historyStack.clear();
}

void DetailPage::clearTab(const QWidget *content)
{   // 清理Tab
    qDeleteAll(content->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly));
    if (content->layout()) delete content->layout();
}

void DetailPage::resetUI()
{   // 重置ui
    clearLayout();
    clearTab(ui.charactersContent);
    clearTab(ui.staffContent);
    clearTab(ui.relationsContent);
    ui.lineEditTitle->clear();
    ui.textEditSummary->clear();
    ui.labelCover->clear();
    ui.labelProgress->setText("全 - 话");
    ui.labelStatus->setText("TBA");
    ui.labelRatingInfo->setText(" | 人评 | #");
    ui.labelCollectionStats->setText("收藏 / 在看 / 抛弃");
    if (!m_scoreChartWidget) return;
    m_scoreChartWidget->parentWidget()->layout()->removeWidget(m_scoreChartWidget);
    m_scoreChartWidget->deleteLater();
    m_scoreChartWidget = nullptr;
}

void DetailPage::onBackButtonClicked()
{   // 返回
    resetUI();
    m_characters.clear();
    characterTabInitialized = false;
    m_persons.clear();
    staffTabInitialized = false;
    m_relations.clear();
    relationTabInitialized = false;
    if (!m_historyStack.isEmpty()) setCollectionData(m_historyStack.pop(), "");
    else emit backButtonClicked();
}