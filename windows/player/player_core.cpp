#include "player_core.h"
#include <QPainter>
#include <QKeyEvent>

VLCPlayer::VLCPlayer(QWidget* parent) : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setFocusPolicy(Qt::StrongFocus);
    const char* vlc_args[] = {"--quiet", "--no-xlib", "--avcodec-fast", "--avcodec-threads=1"};
    vlc_instance = libvlc_new(std::size(vlc_args), vlc_args);
    media_player = libvlc_media_player_new(vlc_instance);
    eventManager = libvlc_media_player_event_manager(media_player);
    libvlc_event_attach(eventManager, libvlc_MediaPlayerPlaying, onVlcEvent, this);
    libvlc_event_attach(eventManager, libvlc_MediaPlayerPaused, onVlcEvent, this);
    libvlc_event_attach(eventManager, libvlc_MediaPlayerStopped, onVlcEvent, this);
    libvlc_event_attach(eventManager, libvlc_MediaPlayerTimeChanged, onVlcEvent, this);
    libvlc_event_attach(eventManager, libvlc_MediaPlayerPositionChanged, onVlcEvent, this);
    libvlc_event_attach(eventManager, libvlc_MediaPlayerAudioVolume, onVlcEvent, this);
}

VLCPlayer::~VLCPlayer()
{
    libvlc_event_detach(eventManager, libvlc_MediaPlayerPlaying, onVlcEvent, this);
    libvlc_event_detach(eventManager, libvlc_MediaPlayerPaused, onVlcEvent, this);
    libvlc_event_detach(eventManager, libvlc_MediaPlayerStopped, onVlcEvent, this);
    libvlc_event_detach(eventManager, libvlc_MediaPlayerTimeChanged, onVlcEvent, this);
    libvlc_event_detach(eventManager, libvlc_MediaPlayerPositionChanged, onVlcEvent, this);
    libvlc_event_detach(eventManager, libvlc_MediaPlayerAudioVolume, onVlcEvent, this);
    if (media_player) {
        libvlc_media_player_stop(media_player);
        libvlc_media_player_release(media_player);
    }
    if (vlc_instance) libvlc_release(vlc_instance);
}

void VLCPlayer::playVideo(const QString& source)
{   // 播放
    stop();
    libvlc_media_t* media = nullptr;
    if (source.contains("http")) media = libvlc_media_new_location(vlc_instance, source.toUtf8().constData());
    else media = libvlc_media_new_path(vlc_instance, source.toUtf8().constData());
    libvlc_media_add_option(media, ":avcodec-hw=dxva2");
    libvlc_media_player_set_media(media_player, media);
    libvlc_media_release(media);
    libvlc_video_set_callbacks(media_player, lock, unlock, nullptr, this);
    libvlc_video_set_format(media_player, "RV32", video_width, video_height, video_width * 4);
    libvlc_media_player_play(media_player);
}

void VLCPlayer::stop()
{   // 结束播放
    if (media_player) libvlc_media_player_stop(media_player);
    current_frame = QImage();
}

void* VLCPlayer::lock(void* data, void** pixels)
{
    auto* player = static_cast<VLCPlayer*>(data);
    auto* buffer = new uchar[player->video_height * player->video_width * 4];
    *pixels = buffer;
    player->current_buffer.store(buffer, std::memory_order_release);
    return nullptr;
}

void VLCPlayer::unlock(void* data, [[maybe_unused]] void* id, [[maybe_unused]] void* const* pixels)
{
    auto* player = static_cast<VLCPlayer*>(data);
    uchar* buffer = player->current_buffer.exchange(nullptr, std::memory_order_acq_rel);
    if (!buffer) return;
    QImage image(buffer, player->video_width, player->video_height, QImage::Format_ARGB32, [](void* ptr){delete[] static_cast<uchar*>(ptr);}, buffer);
    QMetaObject::invokeMethod(player, [player, image] {
        player->current_frame = image;
        player->update();
    }, Qt::QueuedConnection);
}

void VLCPlayer::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.fillRect(rect(), Qt::black);
    if (!current_frame.isNull()) {
        const QPixmap pixmap = QPixmap::fromImage(current_frame);
        const QPixmap scaled = pixmap.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap((width() - scaled.width()) / 2, (height() - scaled.height()) / 2, scaled);
    }
}

void VLCPlayer::keyPressEvent(QKeyEvent* event)
{   // 键盘事件
    bool handled = true;
    if (event->key() == Qt::Key_Space) togglePlayPause();
    else if (event->key() == Qt::Key_Escape) emit exitFullscreenRequested();
    else if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Right) {
        auto [currentMs, totalMs] = getTimeInfo();
        if (totalMs > 0) setPosition(static_cast<float>(qBound(0, currentMs + (event->key() == Qt::Key_Right ? 5000 : -5000), totalMs)) / static_cast<float>(totalMs));
    } else if (event->key() == Qt::Key_Up) setVolume(qMin(getVolume() + 5, 100));
    else if (event->key() == Qt::Key_Down) setVolume(qMax(getVolume() - 5, 0));
    else handled = false;
    if (handled) event->accept();
    else QWidget::keyPressEvent(event);
}

void VLCPlayer::onVlcEvent(const libvlc_event_t* event, void* userData)
{   // vlc事件处理
    auto* player = static_cast<VLCPlayer*>(userData);
    if (event->type == libvlc_MediaPlayerPlaying) emit player->playStateChanged(true);
    else if (event->type == libvlc_MediaPlayerPaused || event->type == libvlc_MediaPlayerStopped) emit player->playStateChanged(false);
    else if (event->type == libvlc_MediaPlayerTimeChanged) {
        libvlc_time_t new_time = event->u.media_player_time_changed.new_time;
        libvlc_time_t length = libvlc_media_player_get_length(player->media_player);
        QMetaObject::invokeMethod(player, [player, new_time, length] {emit player->timeChanged(static_cast<int>(new_time), static_cast<int>(length));}, Qt::QueuedConnection);
    }
    else if (event->type == libvlc_MediaPlayerPositionChanged) emit player->positionChanged(event->u.media_player_position_changed.new_position);
    else if (event->type == libvlc_MediaPlayerAudioVolume) emit player->volumeChanged(qRound(event->u.media_player_audio_volume.volume * 100.0f));
}

void VLCPlayer::setPosition(const float position) const
{   // 设置播放位置 (0.0 ~ 1.0)
    if (media_player) libvlc_media_player_set_position(media_player, position);
}

std::pair<int, int> VLCPlayer::getTimeInfo() const
{    // 当前时间ms, 总时长ms
    if (!media_player) return {0, 0};
    return {static_cast<int>(libvlc_media_player_get_time(media_player)), static_cast<int>(libvlc_media_player_get_length(media_player))};
}

void VLCPlayer::setVolume(const int volume) const
{   // 设置音量 (0 ~ 100)
    if (media_player) libvlc_audio_set_volume(media_player, volume);
}

int VLCPlayer::getVolume() const
{   // 获取当前音量
    return media_player ? libvlc_audio_get_volume(media_player) : 50;
}

void VLCPlayer::setPlaybackRate(const float rate) const
{   // 设置播放速度
    if (media_player) libvlc_media_player_set_rate(media_player, rate);
}

void VLCPlayer::togglePlayPause() const
{   // 切换播放/暂停
    if (media_player) libvlc_media_player_pause(media_player);
}