#ifndef PLAYER_H
#define PLAYER_H

#include <QSlider>

class QLabel;
class QPushButton;

class ClickableSlider : public QSlider
{
    Q_OBJECT
public:
    explicit ClickableSlider(Qt::Orientation orientation, int thresholdPercent = 2, QWidget *parent = nullptr);
    [[nodiscard]] double calculateValueFromPos(const QPoint &pos) const;

signals:
    void clicked(int value);
    void hoverTime(double fraction, int mouseX);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    const int thresholdPercent;
};

class ControlOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit ControlOverlay(QWidget *parent = nullptr);
    void setPlayState(bool playing);
    void setProgress(double position) const;
    void setTime(int currentSeconds, int totalSeconds);
    void setVolume(int volume) const;
    void showControls(bool startTimer);
    void hideControls();

signals:
    void playPauseRequested();
    void fullscreenRequested();
    void backRequested();
    void volumeChanged(int volume);
    void seekRequested(double position);
    void playbackRateChanged(double rate);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private slots:
    void onProgressPressed();
    void onProgressReleased();
    void onProgressClicked(int value);
    void onProgressHover(double fraction, int mouseX) const;
    void onVolumePressed();
    void onVolumeReleased();
    void onPlaybackRateSelected(int index);
    void updatePlayButtonIcon() const;

private:
    void setupUi();
    void setupTimers();
    QPushButton* createControlButton(const QString &iconPath, int iconSize, void (ControlOverlay::*signal)());
    void updatePlaybackRateButtonText() const;
    void updateTimeTooltipPosition(double fraction, int mouseX) const;
    [[nodiscard]] bool isInControlArea(const QPoint &pos) const;
    static QString formatTime(int seconds);
    QWidget *topWidget = nullptr;
    QWidget *bottomWidget = nullptr;
    QPushButton *backButton = nullptr;
    QLabel *timeLabel = nullptr;
    QLabel *timeTooltip = nullptr;
    ClickableSlider *progressSlider = nullptr;
    QPushButton *playButton = nullptr;
    QLabel *volumeIconLabel = nullptr;
    ClickableSlider *volumeSlider = nullptr;
    QPushButton *playbackRateButton = nullptr;
    QPushButton *fullscreenButton = nullptr;
    bool isPlaying = false;
    bool isVisible = true;
    bool draggingProgress = false;
    bool draggingVolume = false;
    int currentTime = 0;
    int totalTime = 0;
    quint64 lastClickTime = 0;
    static constexpr int PROGRESS_MAX_VALUE = 10000;
    static constexpr int HIDE_DELAY = 3000;
    static constexpr int DOUBLE_CLICK_INTERVAL = 200;
    static const QList<double> PLAYBACK_RATES;
    int currentPlaybackRateIndex = 4;
    QTimer *hideTimer = nullptr;
    QTimer *clickTimer = nullptr;
};

#endif // PLAYER_H