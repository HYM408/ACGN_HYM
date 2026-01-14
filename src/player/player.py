import os
vlc_dir = os.path.dirname(r".\vlc\libvlc.dll")
os.environ["PATH"] = vlc_dir + os.pathsep + os.environ["PATH"]
import vlc
import ctypes


class VlcPlayer:
    """VLC播放器"""

    def __init__(self):
        vlc_args = ["--no-xlib", "--quiet", "--avcodec-fast", "--avcodec-threads=1"]
        self.instance = vlc.Instance(vlc_args)
        self.player = self.instance.media_player_new()
        self.width, self.height = 1920, 1080
        self.stride = self.width * 4
        self.buffer = None
        self.frame_ready = False
        self.current_frame = None
        self.setup_callbacks()

    def setup_callbacks(self):
        """设置VLC回调函数"""
        VideoLockCb = ctypes.CFUNCTYPE(ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p))
        VideoUnlockCb = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p))
        VideoDisplayCb = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_void_p)

        def lock_cb(opaque, planes):
            if not self.buffer:
                self.buffer = (ctypes.c_uint8 * (self.width * self.height * 4))()
                self.current_frame = None
            planes[0] = ctypes.addressof(self.buffer)
            return None

        def unlock_cb(opaque, picture, planes):
            self.frame_ready = True

        def display_cb(opaque, picture):
            pass

        self._lock_cb = VideoLockCb(lock_cb)
        self._unlock_cb = VideoUnlockCb(unlock_cb)
        self._display_cb = VideoDisplayCb(display_cb)
        self.player.video_set_callbacks(self._lock_cb, self._unlock_cb, self._display_cb, None)
        self.player.video_set_format("RV32", self.width, self.height, self.stride)

    def play(self, video_url):
        """播放视频"""
        try:
            media = self.instance.media_new(video_url)
            media.add_option(":avcodec-hw=dxva2")
            media.add_option(":network-caching=100")
            media.add_option(":file-caching=100")
            media.add_option(":disc-caching=100")
            self.player.set_media(media)
            self.player.play()
            print(f"开始播放: {video_url}")
            return True
        except Exception as e:
            print(f"播放失败: {e}")
            return False

    def cleanup(self):
        """清理资源"""
        if self.player:
            self.player.stop()
            self.player.release()
        self.buffer = None
        self.current_frame = None
        self.frame_ready = False
        # 清理回调函数
        self._lock_cb = None
        self._unlock_cb = None
        self._display_cb = None
        if self.instance:
            self.instance.release()
        print("VLC已清理")

    def get_frame_data(self):
        """获取帧数据"""
        if not self.frame_ready:
            return None
        self.frame_ready = False
        if self.current_frame is None and self.buffer:
            self.current_frame = memoryview(self.buffer)
        return self.current_frame

    def is_playing(self):
        return self.player.is_playing() if self.player else False

    def get_position(self):
        return self.player.get_position() if self.player else 0

    def set_position(self, pos):
        if self.player:
            self.player.set_position(pos)