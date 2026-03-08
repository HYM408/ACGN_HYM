#include "config.h"

void initConfig()
{   // 初始化配置文件
    QSettings settings("./config.ini", QSettings::IniFormat);
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