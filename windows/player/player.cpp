#include "player.h"
#include <QMenu>
#include <QTimer>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMouseEvent>
#include <QRegularExpression>

ClickableSlider::ClickableSlider(Qt::Orientation orientation, int thresholdPercent, QWidget *parent): QSlider(orientation, parent), thresholdPercent(thresholdPercent) {setMouseTracking(true);}

double ClickableSlider::calculateValueFromPos(const QPoint &pos) const
{   // 计算滑块对应值
    const int available = (orientation() == Qt::Horizontal ? width() : height()) - 20;
    if (available <= 0) return 0;
    double value;
    if (orientation() == Qt::Horizontal) value = (pos.x() - 10) * static_cast<double>(maximum()) / available;
    else value = (height() - pos.y() - 10) * static_cast<double>(maximum()) / available;
    return qBound(0.0, value, static_cast<double>(maximum()));
}

void ClickableSlider::mousePressEvent(QMouseEvent *event)
{   // 点击滑块直接跳转
    double value = calculateValueFromPos(event->pos());
    int threshold = maximum() * thresholdPercent / 100;
    if (qAbs(value - this->value()) <= threshold) QSlider::mousePressEvent(event);
    else {
        int intValue = qRound(value);
        setValue(intValue);
        emit clicked(intValue);
        event->accept();
    }
}

void ClickableSlider::mouseMoveEvent(QMouseEvent *event)
{   // 保持滑块默认的鼠标移动行为
    QSlider::mouseMoveEvent(event);
    double value = calculateValueFromPos(event->pos());
    double fraction = value / maximum();
    emit hoverTime(fraction, event->pos().x());
}

void ClickableSlider::leaveEvent(QEvent *event)
{   // 鼠标离开
    QSlider::leaveEvent(event);
    emit hoverTime(-1, -1);
}

const QList<double> ControlOverlay::PLAYBACK_RATES = {3.0, 2.0, 1.5, 1.25, 1.0, 0.75, 0.5};

static const QString SLIDER_STYLE =
    "QSlider { background: transparent}"
    "QSlider::groove:horizontal { height: 4px; background: rgba(100, 100, 100, 150); border-radius: 2px}"
    "QSlider::sub-page:horizontal { height: 4px; background: #dc3232; border-radius: 2px}"
    "QSlider::handle:horizontal { background: rgba(255, 255, 255, 220); width: 12px; height: 12px; margin: -4px 0; border-radius: 6px}";

static const QString PROGRESS_SLIDER_STYLE =
    "QSlider { background-color: transparent}"
    "QSlider::groove:horizontal { height: 8px; background: rgba(100, 100, 100, 150); border-radius: 4px}"
    "QSlider::sub-page:horizontal { height: 8px; background: #dc3232; border-radius: 4px}"
    "QSlider::handle:horizontal { background: rgba(255, 255, 255, 220); width: 16px; height: 16px; margin: -4px 0; border-radius: 8px}";

ControlOverlay::ControlOverlay(QWidget *parent): QWidget(parent)
{   // 初始化控制面板
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setMouseTracking(true);
    setupUi();
    setupTimers();
    updatePlayButtonIcon();
    updatePlaybackRateButtonText();
}

void ControlOverlay::setupUi()
{   // 控制面板 UI
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    // 顶部区域
    topWidget = new QWidget(this);
    topWidget->setFixedHeight(60);
    topWidget->setStyleSheet("background-color: transparent");
    auto *topLayout = new QHBoxLayout(topWidget);
    backButton = createControlButton("icons/back2.png", 20, &ControlOverlay::backRequested);
    topLayout->addWidget(backButton);
    topLayout->addStretch();
    mainLayout->addWidget(topWidget);
    mainLayout->addStretch();
    // 底部区域
    bottomWidget = new QWidget(this);
    bottomWidget->setFixedHeight(80);
    bottomWidget->setStyleSheet("background-color: rgba(0, 0, 0, 100); border: none");
    auto *bottomLayout = new QVBoxLayout(bottomWidget);
    // 进度条
    auto *progressLayout = new QHBoxLayout();
    timeLabel = new QLabel("00:00 / 00:00", bottomWidget);
    timeLabel->setStyleSheet("color: rgba(255, 255, 255, 220); font-size: 11px; background-color: transparent");
    progressLayout->addWidget(timeLabel);
    timeTooltip = new QLabel(this);
    timeTooltip->setStyleSheet("background-color: rgba(0, 0, 0, 100); color: white; padding: 4px 8px; font-size: 12px");
    timeTooltip->setAlignment(Qt::AlignCenter);
    timeTooltip->hide();
    progressSlider = new ClickableSlider(Qt::Horizontal, 2, bottomWidget);
    progressSlider->setFixedHeight(30);
    progressSlider->setRange(0, PROGRESS_MAX_VALUE);
    progressSlider->setStyleSheet(PROGRESS_SLIDER_STYLE);
    connect(progressSlider, &ClickableSlider::sliderPressed, this, &ControlOverlay::onProgressPressed);
    connect(progressSlider, &ClickableSlider::sliderReleased, this, &ControlOverlay::onProgressReleased);
    connect(progressSlider, &ClickableSlider::clicked, this, &ControlOverlay::onProgressClicked);
    connect(progressSlider, &ClickableSlider::hoverTime, this, &ControlOverlay::onProgressHover);
    progressLayout->addWidget(progressSlider, 1);
    bottomLayout->addLayout(progressLayout);
    // 控制按钮
    auto *controlsLayout = new QHBoxLayout();
    controlsLayout->setSpacing(10);
    playButton = createControlButton("icons/play.png", 25, &ControlOverlay::playPauseRequested);
    controlsLayout->addWidget(playButton);
    // 音量
    auto *volumeLayout = new QHBoxLayout();
    volumeLayout->setSpacing(8);
    volumeLayout->setContentsMargins(0, 0, 0, 0);
    volumeIconLabel = new QLabel(bottomWidget);
    volumeIconLabel->setFixedSize(20, 20);
    volumeIconLabel->setPixmap(QPixmap("icons/sound.png").scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    volumeIconLabel->setStyleSheet("background-color: transparent;");
    volumeSlider = new ClickableSlider(Qt::Horizontal, 10, bottomWidget);
    volumeSlider->setFixedSize(80, 20);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(100);
    volumeSlider->setStyleSheet(SLIDER_STYLE);
    connect(volumeSlider, &QSlider::valueChanged, this, &ControlOverlay::volumeChanged);
    connect(volumeSlider, &ClickableSlider::sliderPressed, this, &ControlOverlay::onVolumePressed);
    connect(volumeSlider, &ClickableSlider::sliderReleased, this, &ControlOverlay::onVolumeReleased);
    connect(volumeSlider, &ClickableSlider::clicked, this, &ControlOverlay::volumeChanged);
    volumeLayout->addWidget(volumeIconLabel);
    volumeLayout->addWidget(volumeSlider);
    controlsLayout->addLayout(volumeLayout);
    controlsLayout->addStretch();
    // 播放速度
    playbackRateButton = new QPushButton(bottomWidget);
    playbackRateButton->setFixedSize(40, 28);
    playbackRateButton->setStyleSheet("QPushButton { border: 1px solid rgba(255, 255, 255, 150); border-radius: 4px; background-color: rgba(0, 0, 0, 120); color: rgba(255, 255, 255, 220); font-size: 12px; padding: 2px}"
                                      "QPushButton:hover { background-color: rgba(50, 50, 50, 150)}");
    connect(playbackRateButton, &QPushButton::clicked, [this]() {
        QMenu menu(this);
        menu.setStyleSheet("QMenu { background-color: rgba(40, 40, 40, 240); border: 1px solid rgba(100, 100, 100, 200)}"
                           "QMenu::item { color: white; padding: 6px 16px; font-size: 12px}"
                           "QMenu::item:selected { background-color: rgba(80, 80, 80, 255)}");
        for (int i = 0; i < PLAYBACK_RATES.size(); ++i) {
            double rate = PLAYBACK_RATES[i];
            QString rateStr = QString::number(rate, 'f', 2);
            rateStr.replace(QRegularExpression("0+$"), "").replace(QRegularExpression("\\.$"), "");
            QAction *action = menu.addAction(rateStr + "x");
            action->setCheckable(true);
            action->setChecked(i == currentPlaybackRateIndex);
            connect(action, &QAction::triggered, [this, i]() { onPlaybackRateSelected(i); });
        }
        QPoint pos = playbackRateButton->mapToGlobal(QPoint(0, 0));
        pos.setY(pos.y() - menu.sizeHint().height());
        menu.exec(pos);
    });
    controlsLayout->addWidget(playbackRateButton);
    fullscreenButton = createControlButton("icons/full.png", 20, &ControlOverlay::fullscreenRequested);
    controlsLayout->addWidget(fullscreenButton);
    bottomLayout->addLayout(controlsLayout);
    mainLayout->addWidget(bottomWidget);
}

void ControlOverlay::setupTimers()
{   // 隐藏控制面板定时器
    hideTimer = new QTimer(this);
    hideTimer->setSingleShot(true);
    connect(hideTimer, &QTimer::timeout, this, &ControlOverlay::hideControls);
    clickTimer = new QTimer(this);
    clickTimer->setSingleShot(true);
    connect(clickTimer, &QTimer::timeout, this, &ControlOverlay::playPauseRequested);
}

QPushButton* ControlOverlay::createControlButton(const QString &iconPath, int iconSize, void (ControlOverlay::*signal)())
{   // 创建控制按钮
    auto *btn = new QPushButton(this);
    btn->setFixedSize(28, 28);
    btn->setStyleSheet("QPushButton { border: none; border-radius: 14px; background-color: transparent}");
    btn->setIcon(QPixmap(iconPath));
    btn->setIconSize(QSize(iconSize, iconSize));
    connect(btn, &QPushButton::clicked, this, signal);
    return btn;
}

void ControlOverlay::updatePlaybackRateButtonText() const
{   // 播放速度按钮文本
    double rate = PLAYBACK_RATES[currentPlaybackRateIndex];
    QString rateStr = QString::number(rate, 'f', 2);
    rateStr.replace(QRegularExpression("0+$"), "").replace(QRegularExpression("\\.$"), "");
    playbackRateButton->setText(rateStr + "x");
}

void ControlOverlay::onPlaybackRateSelected(int index)
{   // 选择播放速度
    if (index == currentPlaybackRateIndex) return;
    currentPlaybackRateIndex = index;
    updatePlaybackRateButtonText();
    emit playbackRateChanged(PLAYBACK_RATES[index]);
    showControls();
}

void ControlOverlay::setPlayState(bool playing)
{   // 播放状态
    if (isPlaying == playing) return;
    isPlaying = playing;
    updatePlayButtonIcon();
    hideTimer->stop();
    if (isPlaying) showControls();
    else showControls(false);
}

void ControlOverlay::setProgress(double position) const
{   // 设置进度条
    if (!draggingProgress) progressSlider->setValue(static_cast<int>(position * PROGRESS_MAX_VALUE));
}

void ControlOverlay::setTime(int currentSeconds, int totalSeconds)
{   // 时间
    if (!draggingProgress) {
        currentTime = currentSeconds;
        totalTime = totalSeconds;
        timeLabel->setText(formatTime(currentSeconds) + " / " + formatTime(totalSeconds));
    }
}

void ControlOverlay::setVolume(int volume) const
{   // 设置音量
    if (!draggingVolume) volumeSlider->setValue(volume);
}

void ControlOverlay::showControls(bool startTimer)
{   // 显示控制面板
    if (!isVisible) {
        isVisible = true;
        topWidget->show();
        bottomWidget->show();
    }
    if (startTimer && isPlaying) hideTimer->start(HIDE_DELAY);
    setCursor(Qt::ArrowCursor);
}

void ControlOverlay::hideControls()
{   // 隐藏控制面板
    if (isVisible && isPlaying && !draggingProgress && !draggingVolume) {
        isVisible = false;
        topWidget->hide();
        bottomWidget->hide();
        setCursor(Qt::BlankCursor);
    }
}

void ControlOverlay::mouseMoveEvent(QMouseEvent *event)
{   // 鼠标移动事件
    QWidget::mouseMoveEvent(event);
    QPoint pos = event->pos();
    if (!isInControlArea(pos)) {
        if (!isVisible) showControls();
        else if (isPlaying && !draggingProgress && !draggingVolume) hideTimer->start(HIDE_DELAY);
    } else {
        hideTimer->stop();
        showControls(false);
    }
}

void ControlOverlay::mousePressEvent(QMouseEvent *event)
{   // 鼠标点击事件
    QWidget::mousePressEvent(event);
    setCursor(Qt::ArrowCursor);
    QPoint pos = event->pos();
    if (isInControlArea(pos)) showControls(false);
    else if (event->button() == Qt::LeftButton) {
        quint64 clickTimestamp = event->timestamp();
        if (clickTimestamp - lastClickTime < DOUBLE_CLICK_INTERVAL) {
            clickTimer->stop();
            emit fullscreenRequested();
        } else {
            lastClickTime = clickTimestamp;
            clickTimer->start(DOUBLE_CLICK_INTERVAL);
        }
    }
}

void ControlOverlay::mouseDoubleClickEvent(QMouseEvent *event)
{   // 双击事件
    if (event->button() == Qt::LeftButton) {
        clickTimer->stop();
        emit fullscreenRequested();
    }
}

bool ControlOverlay::isInControlArea(const QPoint &pos) const
{   // 是否在控制面板区域
    if (bottomWidget->geometry().contains(pos) || topWidget->geometry().contains(pos)) return true;
    return false;
}

void ControlOverlay::updatePlayButtonIcon() const
{   // 暂停.播放图标
    QString iconName = isPlaying ? "icons/stop.png" : "icons/play.png";
    playButton->setIcon(QPixmap(iconName));
    playButton->setIconSize(QSize(25, 25));
}

QString ControlOverlay::formatTime(int seconds)
{   // 格式化时间
    int hrs = seconds / 3600;
    int mins = seconds % 3600 / 60;
    int secs = seconds % 60;
    if (hrs > 0) return QString("%1:%2:%3").arg(hrs, 2, 10, QChar('0')) .arg(mins, 2, 10, QChar('0')) .arg(secs, 2, 10, QChar('0'));
    return QString("%1:%2").arg(mins, 2, 10, QChar('0')) .arg(secs, 2, 10, QChar('0'));
}

void ControlOverlay::updateTimeTooltipPosition(double fraction, int mouseX) const
{   // 时间提示
    if (fraction < 0 || totalTime <= 0) {
        timeTooltip->hide();
        return;
    }
    int hoverSeconds = qRound(fraction * totalTime);
    timeTooltip->setText(formatTime(hoverSeconds));
    timeTooltip->adjustSize();
    QPoint progressPos = progressSlider->mapTo(this, QPoint(0, 0));
    int x = progressPos.x() + mouseX - timeTooltip->width() / 2;
    int y = progressPos.y() - timeTooltip->height();
    x = qBound(10, x, width() - timeTooltip->width() - 10);
    y = qMax(10, y);
    timeTooltip->move(x, y);
    timeTooltip->show();
    hideTimer->stop();
}

void ControlOverlay::onProgressPressed()
{   // 拖动进度条
    draggingProgress = true;
    hideTimer->stop();
    setCursor(Qt::ArrowCursor);
}

void ControlOverlay::onProgressReleased()
{   // 拖动进度条结束
    draggingProgress = false;
    double position = static_cast<double>(progressSlider->value()) / PROGRESS_MAX_VALUE;
    emit seekRequested(position);
    if (isPlaying) showControls();
    else showControls(false);
}

void ControlOverlay::onProgressClicked(int value)
{   // 点击
    double position = static_cast<double>(value) / PROGRESS_MAX_VALUE;
    emit seekRequested(position);
    int currentSecs = qRound(totalTime * position);
    timeLabel->setText(formatTime(currentSecs) + " / " + formatTime(totalTime));
    showControls();
}

void ControlOverlay::onProgressHover(double fraction, int mouseX) const
{   // 显示时间提示
    updateTimeTooltipPosition(fraction, mouseX);
    if (fraction < 0 && isPlaying && !isInControlArea(mapFromGlobal(QCursor::pos()))) hideTimer->start(HIDE_DELAY);
}

void ControlOverlay::onVolumePressed()
{   // 拖动音量
    draggingVolume = true;
    hideTimer->stop();
    setCursor(Qt::ArrowCursor);
}

void ControlOverlay::onVolumeReleased()
{   // 拖动音量结束
    draggingVolume = false;
    if (isPlaying) showControls();
    else showControls(false);
}