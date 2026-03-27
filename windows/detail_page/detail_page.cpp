#include "detail_page.h"
#include <QTimer>
#include <QJsonArray>
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
    const QColor color1 = getColor("color1", QColor("#fdf7ff"));
    m_color2 = getColor("color2", QColor("#f2ecf4"));
    m_color3 = getColor("color3", QColor("#e1dbe4"));
    const QColor color4 = getColor("color4", QColor("#f2ecf4"));
    const QColor color5 = getColor("color5", QColor("#e1dbe4"));
    ui.frame_6->setStyleSheet(QString("QFrame {background-color: %1}").arg(color1.name()));
    ui.pushButton_26->setStyleSheet(QString("QPushButton {background-color: %1; border-radius:15px}"
                                            "QPushButton:hover {background-color: %2}").arg(color4.name(), color5.name()));
    ui.pushButton_27->setStyleSheet(QString("QPushButton {background-color: %1; border-radius:15px}"
                                            "QPushButton:hover {background-color: %2}").arg(color4.name(), color5.name()));
    ui.pushButton->setStyleSheet(QString("QPushButton {background-color: %1; border-radius:15px; padding: 2px 10px}"
                                         "QPushButton:hover {background-color: %2}").arg(color4.name(), color5.name()));
}

void DetailPage::setupConnections()
{   // 连接
    connect(ui.pushButton_19, &QPushButton::clicked, this, &DetailPage::onBackButtonClicked);
    connect(ui.pushButton_27, &QPushButton::clicked, this, &DetailPage::onEpisodeClicked);
    connect(ui.pushButton_20, &QPushButton::clicked, this, &DetailPage::onOpenBangumiPage);
    connect(ui.pushButton_26, &QPushButton::clicked, this, &DetailPage::onStatusButtonClicked);
    connect(ui.pushButton, &QPushButton::clicked, this, &DetailPage::onRatingButtonClicked);
    connect(ui.tabWidget, &QTabWidget::currentChanged, this, &DetailPage::clickOnTab);
    setupLineEditCustomContextMenu(ui.lineEdit, CMO_Copy | CMO_SelectAll);
    setupTextEditCustomContextMenu(ui.textEdit_2, CMO_Default);
    ui.lineEdit->setCursor(Qt::IBeamCursor);
}

void DetailPage::setCollectionData(const int subjectId, const QString &progressText)
{   // 显示传入数据
    ui.tabWidget->setCurrentIndex(0);
    resetUI();
    currentData = DatabaseManager::getCollectionBySubjectId(subjectId);
    const QString imageUrl = QString("https://api.bgm.tv/v0/subjects/%1/image?type=large").arg(subjectId);
    ImageUtil::loadImageWithCache(cacheImageUtil, imageUrl, ui.cover_label_3, 15, true, true, QString("s%1.jpg").arg(subjectId));
    ui.pushButton->setText(currentData.rate > 0 ? QString("我的评价: %1 分").arg(currentData.rate) : "未评价");
    loadData(subjectId, progressText);
}

void DetailPage::loadData(int subjectId, const QString &progressText)
{   // 加载数据
    subjectData = dbManager->getSubjectById(subjectId);
    if (subjectData.subject_id == 0) {
        bangumiAPI->getSubjectInfo(subjectId, 3, [this, subjectId](const QJsonObject &subjectInfo, const QString &error) {
            if (!error.isEmpty() || subjectInfo.isEmpty()) return;
            if (!dbManager->insertOrUpdateSubject(subjectInfo)) qDebug() << subjectId << "失败";
            if (!isVisible()) return;
            subjectData = dbManager->getSubjectById(subjectId);
        });
    }
    updateDetailPage(progressText);
    QTimer::singleShot(50, this, [this, subjectId] {
        m_characters = dbManager->getCharacters(subjectId);
        m_relations = dbManager->getSubjectRelations(subjectId);
        m_persons = dbManager->getPersons(subjectId);
    });
}

void DetailPage::onEpisodeClicked()
{   // 打开选集页
    if (subjectData.subject_type == 4) gameMonitorUtil->startGame(subjectData.subject_id);
    else emit showEpisodePageRequested(currentData);
}

void DetailPage::onOpenBangumiPage() const
{   // 跳转Bangumi
    const QString baseUrl = getConfig("Bangumi/bangumi_base_url").toString();
    QDesktopServices::openUrl(QString("%1subject/%2").arg(baseUrl).arg(subjectData.subject_id));
}

void DetailPage::onStatusButtonClicked()
{   // 改变状态
    int subjectType = subjectData.subject_type;
    const int currentStatus = currentData.type;
    StatusSelector::showStatusSelector(ui.pushButton_26, subjectType, currentStatus, subjectData.subject_id, bangumiAPI, dbManager,[this, subjectType](const int selectedStatus) {
        currentData.type = selectedStatus;
        ui.pushButton_26->setText(statusNamesMap.value(subjectType).value(selectedStatus));
        emit refresh();
    });
}

void DetailPage::updateDetailPage(const QString &progressText)
{   // 显示数据
    ui.lineEdit->setText(subjectData.name_cn.isEmpty() ? subjectData.name : subjectData.name_cn);
    QLabel *targetButton = subjectData.subject_type == 4 ? ui.pushButton_23 : ui.pushButton_24;
    CollectionData cData;
    cData.subject_id = subjectData.subject_id;
    cData.subject_type = subjectData.subject_type;
    cData.subject_date = subjectData.date;
    cData.ep_status = currentData.ep_status;
    cData.subject_volumes = currentData.subject_volumes;
    cData.subject_eps = currentData.subject_eps;
    targetButton->setText(progressText.isEmpty() ? computeProgressText(cData, dbManager->getEpisodeAirdates({subjectData.subject_id})) : progressText);
    ui.pushButton_26->setText(statusNamesMap.value(subjectData.subject_type).value(currentData.type));
    ui.pushButton_27->setText(subjectData.subject_type == 2 ? "选集" : subjectData.subject_type == 4 ? "启动" : "进度");
    const int total = std::reduce(subjectData.score_details.begin(), subjectData.score_details.end(), 0);
    ui.pushButton_21->setText(QString("%1 | %2人评 | #%3").arg(subjectData.rating_score).arg(total).arg(subjectData.rating_rank));
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
    m_currentTagPairs = allTagPairs;
    QTimer::singleShot(0, this, [this] {tagsDisplay(m_currentTagPairs);});
    const QString timeTag = getTimeInfo(tagPairs, subjectData.date);
    if (subjectData.subject_type == 4) {
        QVector<GameData> gameDataList = DatabaseManager::getGameData({subjectData.subject_id});
        if (!gameDataList.isEmpty()) ui.pushButton_24->setText(QString("已玩 %1 小时").arg(gameDataList.first().play_duration));
        else ui.pushButton_24->setText("0 小时");
    } else ui.pushButton_23->setText(timeTag);
    setupScoreChart(subjectData.score_details, total);
}

void DetailPage::setupScoreChart(const QVector<int> &scoreDetails, const int total)
{   // 设置评分分布表
    auto *chart = new ScoreChartWidget(this);
    chart->setData(scoreDetails, total);
    auto *layout = qobject_cast<QHBoxLayout*>(ui.frame_8->layout());
    layout->insertWidget(layout->count(), chart);
    m_scoreChartWidget = chart;
}

void DetailPage::tagsDisplay(const QList<QPair<QString, int>> &tagPairs)
{   // tag显示
    clearLayout();
    auto *mainLayout = new QVBoxLayout(ui.frame_5);
    QFont font = ui.frame_5->font();
    font.setPixelSize(14);
    const QFontMetrics fm(font);
    const int maxWidth = ui.frame_5->width();
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
        connect(tagLabel, &ClickableLabel::clicked, this, [this, tagName] {emit tagClicked(tagName, subjectData.subject_type);});
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
        bangumiAPI->updateCollection(subjectData.subject_id, {{"rate", rate}}, 3, [this, rate](const bool success, const QString &error) {
            if (success && error.isEmpty()) {
                DatabaseManager::updateCollectionFields(subjectData.subject_id, {{"rate", rate}}, false);
                if (!isVisible()) return;
                ui.pushButton->setText(QString("我的评价: %1 分").arg(rate));
                emit refresh();
            } else ui.pushButton->setText("更改失败");
        });
    });
    connect(m_starRating, &QObject::destroyed, this, [this] {m_starRating = nullptr;});
    const QPoint btnTopRight = ui.pushButton->mapToGlobal(QPoint(ui.pushButton->width(), 0));
    const QPoint btnCenter = ui.pushButton->mapToGlobal(ui.pushButton->rect().center());
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
    setupTabTemplate(ui.scrollAreaWidgetContents, m_characters, [this](const CharacterData& characterData) -> QWidget *{
        auto *card = new QWidget();
        card->setFixedWidth(400);
        card->setStyleSheet(QString("QWidget {background-color: %1; border-radius: 8px}").arg(m_color2.name()));
        auto *layout = new QHBoxLayout(card);
        auto *imageLabel = new QLabel(card);
        imageLabel->setFixedSize(60, 60);
        layout->addWidget(imageLabel);
        const QString imageUrl = QString("https://api.bgm.tv/v0/characters/%1/image?type=grid").arg(characterData.character_id);
        ImageUtil::loadImageWithCache(cacheImageUtil, imageUrl, imageLabel, 10, true, true, QString("c%1.jpg").arg(characterData.character_id));
        auto *textLayout = new QVBoxLayout();
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
    setupTabTemplate(ui.scrollAreaWidgetContents_3, m_relations, [this](const SubjectRelationData& relation) -> QWidget* {
        auto *card = new QPushButton();
        card->setFixedSize(400, 100);
        card->setStyleSheet(QString("QPushButton {background-color: %1; border-radius: 8px}"
                                    "QPushButton:hover {background-color: %2}").arg(m_color2.name(), m_color3.name()));
        auto *cardLayout = new QHBoxLayout(card);
        cardLayout->setContentsMargins(0, 0, 0, 0);
        auto *imageLabel = new QLabel(card);
        imageLabel->setFixedSize(75, 100);
        cardLayout->addWidget(imageLabel);
        const QString imageUrl = QString("https://api.bgm.tv/v0/subjects/%1/image?type=grid").arg(relation.subject.subject_id);
        ImageUtil::loadImageWithCache(cacheImageUtil, imageUrl, imageLabel, 10, false, true, QString("c%1.jpg").arg(relation.subject.subject_id));
        auto *textLayout = new QVBoxLayout();
        const QString title = relation.subject.name_cn.isEmpty() ? relation.subject.name : relation.subject.name_cn;
        auto *titleLabel = new QLabel(title, card);
        QFont titleFont = titleLabel->font();
        titleFont.setBold(true);
        titleLabel->setFont(titleFont);
        titleLabel->setWordWrap(true);
        textLayout->addWidget(titleLabel);
        const QString relText = relationMap.value(relation.relation_type, QString::number(relation.relation_type));
        auto *typeLabel = new QLabel(relText, card);
        textLayout->addWidget(typeLabel);
        cardLayout->addLayout(textLayout);
        cardLayout->addStretch();
        connect(card, &QPushButton::clicked, this, [this, subjectId = relation.subject.subject_id] {
            m_historyStack.push(subjectData.subject_id);
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
    QWidget *content = ui.scrollAreaWidgetContents_2;
    QMap<int, QVector<QPair<int, QString>>> grouped;
    for (const auto &[person_id, position, name, name_cn] : m_persons) grouped[position].append({person_id, name_cn.isEmpty() ? name : name_cn});
    QList<int> displayOrder;
    displayOrder << 67;
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
        ui.lineEdit->deselect();
        QTextCursor cursor = ui.textEdit_2->textCursor();
        cursor.clearSelection();
        ui.textEdit_2->setTextCursor(cursor);
    }
    return QWidget::eventFilter(watched, event);
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
    clearTab(ui.scrollAreaWidgetContents);
    clearTab(ui.scrollAreaWidgetContents_2);
    clearTab(ui.scrollAreaWidgetContents_3);
    ui.lineEdit->clear();
    ui.textEdit_2->clear();
    ui.cover_label_3->clear();
    ui.pushButton_24->setText("全 - 话");
    ui.pushButton_23->setText("TBA");
    ui.pushButton_21->setText(" | 人评 | #");
    ui.pushButton_25->setText("收藏 / 在看 / 抛弃");
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