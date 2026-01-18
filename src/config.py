import os
import json


def get_config_path():
    """获取配置文件路径"""
    return os.path.join(os.path.dirname(os.path.dirname(__file__)), "config.json")

def create_config():
    """创建配置文件"""
    config_path = get_config_path()
    if not os.path.exists(config_path):
        with open(config_path, 'w', encoding='utf-8') as f:
            json.dump({
                # Bangumi
                "user_id": "",
                "client_id": "",
                "client_secret": "",
                "access_token": "",
                "redirect_uri": "",
                "refresh_token": "",
                "expires_at": 0,
                "bangumi_base_url": "https://bangumi.tv/",
                # RSS
                "rss_guid": "0",
                # Pikpak
                "username": "",
                "password": "",
                "encoded_token": "",
                # 下载
                "download_path": "data/download"
            }, f, ensure_ascii=False, indent=2)
    return True

def get_config_item(key, default=None):
    """获取配置文件中键的值"""
    with open(get_config_path(), 'r', encoding='utf-8') as f:
        return json.load(f).get(key, default)

def set_config_items(**kwargs):
    """设置配置文件中键的值"""
    config_path = get_config_path()
    with open(config_path, 'r', encoding='utf-8') as f:
        config = json.load(f)
    config.update(kwargs)
    with open(config_path, 'w', encoding='utf-8') as f:
        json.dump(config, f, ensure_ascii=False, indent=2)
    return True