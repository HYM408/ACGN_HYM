#ifndef PLAYER_CORE_H
#define PLAYER_CORE_H

#include <QWidget>
#include <vlc/vlc.h>

class VLCPlayer : public QWidget
{
    Q_OBJECT
public:
    explicit VLCPlayer(QWidget* parent = nullptr);
    ~VLCPlayer() override;
    void playVideo(const QString& source);
    void stop();
    void setPosition(float position) const;
    float getPosition() const;
    std::pair<int, int> getTimeInfo() const;
    void setVolume(int volume) const;
    int getVolume() const;
    void setPlaybackRate(float rate) const;
    bool isPlaying() const;
    void togglePlayPause() const;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    static void *lock(void* data, void** pixels);
    static void unlock(void* data, void* id, void* const* pixels);
    libvlc_instance_t *vlc_instance   = nullptr;
    libvlc_media_player_t *media_player  = nullptr;
    QImage current_frame;
    int video_width  = 1920;
    int video_height = 1080;
    std::atomic<uchar*> current_buffer{nullptr};

signals:
    void resized();
};

#endif // PLAYER_CORE_H