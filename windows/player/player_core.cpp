#include "player_core.h"
#include <QPainter>

VLCPlayer::VLCPlayer(QWidget* parent) : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    const char* vlc_args[] = {"--quiet", "--no-xlib", "--avcodec-fast", "--avcodec-threads=1"};
    vlc_instance = libvlc_new(std::size(vlc_args), vlc_args);
    media_player = libvlc_media_player_new(vlc_instance);
}

VLCPlayer::~VLCPlayer()
{
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
    else  media = libvlc_media_new_path(vlc_instance, source.toUtf8().constData());
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
        QPixmap pixmap = QPixmap::fromImage(current_frame);
        QPixmap scaled = pixmap.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap((width() - scaled.width()) / 2, (height() - scaled.height()) / 2, scaled);
    }
}

void VLCPlayer::setPosition(float position) const
{   // 设置播放位置 (0.0 ~ 1.0)
    if (media_player) libvlc_media_player_set_position(media_player, position);
}

float VLCPlayer::getPosition() const
{   // 获取当前播放位置
    return media_player ? libvlc_media_player_get_position(media_player) : 0.0f;
}

std::pair<int, int> VLCPlayer::getTimeInfo() const
{    // 当前时间ms, 总时长ms
    if (!media_player) return {0, 0};
    return {static_cast<int>(libvlc_media_player_get_time(media_player)), static_cast<int>(libvlc_media_player_get_length(media_player))};
}

void VLCPlayer::setVolume(int volume) const
{   // 设置音量 (0 ~ 100)
    if (media_player) libvlc_audio_set_volume(media_player, volume);
}

int VLCPlayer::getVolume() const
{   // 获取当前音量
    return media_player ? libvlc_audio_get_volume(media_player) : 50;
}

void VLCPlayer::setPlaybackRate(float rate) const
{   // 设置播放速度
    if (media_player) libvlc_media_player_set_rate(media_player, rate);
}

bool VLCPlayer::isPlaying() const
{   // 播放状态
    return media_player ? libvlc_media_player_is_playing(media_player) : false;
}

void VLCPlayer::togglePlayPause() const
{   // 切换播放/暂停
    if (media_player) libvlc_media_player_pause(media_player);
}