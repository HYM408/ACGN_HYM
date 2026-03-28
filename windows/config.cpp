#include "config.h"
#include <QColor>

void initConfig()
{   // 初始化配置文件
    QSettings settings("./config.ini", QSettings::IniFormat);
    // 关闭动作
    settings.beginGroup("CloseAction");
    setDefaultIfMissing(settings, "remember", false);
    setDefaultIfMissing(settings, "action", 1);
    settings.endGroup();
    // 清理缓存
    settings.beginGroup("Cache");
    setDefaultIfMissing(settings, "next_cleanup_timestamp", 0);
    settings.endGroup();
    // 主题
    settings.beginGroup("Theme");
    setDefaultIfMissing(settings, "color1", "#fdf7ff");
    setDefaultIfMissing(settings, "color2", "#f2ecf4");
    setDefaultIfMissing(settings, "color3", "#e1dbe4");
    setDefaultIfMissing(settings, "color4", "#e9ddff");
    setDefaultIfMissing(settings, "color5", "#ddd0f6");
    setDefaultIfMissing(settings, "color6", "#674fa5");
    settings.endGroup();
    // Bangumi
    settings.beginGroup("Bangumi");
    setDefaultIfMissing(settings, "user_id", "");
    setDefaultIfMissing(settings, "client_id", "");
    setDefaultIfMissing(settings, "client_secret", "");
    setDefaultIfMissing(settings, "redirect_uri", "");
    setDefaultIfMissing(settings, "access_token", "");
    setDefaultIfMissing(settings, "refresh_token", "");
    setDefaultIfMissing(settings, "expires_at", 0);
    setDefaultIfMissing(settings, "bangumi_base_url", "https://bangumi.tv/");
    setDefaultIfMissing(settings, "rss_guid", "0");
    setDefaultIfMissing(settings, "nsfw", false);
    settings.endGroup();
    // PikPak
    settings.beginGroup("PikPak");
    setDefaultIfMissing(settings, "username", "");
    setDefaultIfMissing(settings, "password", "");
    setDefaultIfMissing(settings, "access_token", "");
    setDefaultIfMissing(settings, "refresh_token", "");
    settings.endGroup();
    // 下载
    settings.beginGroup("Download");
    setDefaultIfMissing(settings, "download_path", "data/download");
    settings.endGroup();
    // 数据源
    settings.beginGroup("EnabledSites");
    setDefaultIfMissing(settings, "sites", "*");
    settings.endGroup();
}

void setDefaultIfMissing(QSettings &settings, const QString &key, const QVariant &defaultValue)
{   // 设置缺失键
    if (!settings.contains(key)) settings.setValue(key, defaultValue);
}

QVariant getConfig(const QString &key, const QVariant &defaultValue)
{   // 读取
    const QSettings settings("./config.ini", QSettings::IniFormat);
    return settings.value(key, defaultValue);
}

void setConfig(const QString &key, const QVariant &value)
{   // 写入
    QSettings settings("./config.ini", QSettings::IniFormat);
    settings.setValue(key, value);
}

QColor getColor(const QString &key, const QColor &defaultValue)
{   // 读取颜色
    const QSettings settings("./config.ini", QSettings::IniFormat);
    const QString colorStr = settings.value("Theme/" + key, defaultValue.name()).toString();
    return {colorStr};
}