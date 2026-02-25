#include "config.h"
#include <QSettings>

void initConfig()
{   // 初始化配置文件
    QSettings settings("./config.ini", QSettings::IniFormat);
    if (settings.allKeys().isEmpty()) {
        // Bangumi
        settings.beginGroup("Bangumi");
        settings.setValue("user_id", "");
        settings.setValue("client_id", "");
        settings.setValue("client_secret", "");
        settings.setValue("redirect_uri", "");
        settings.setValue("access_token", "");
        settings.setValue("refresh_token", "");
        settings.setValue("expires_at", 0);
        settings.setValue("bangumi_base_url", "https://bangumi.tv/");
        settings.setValue("rss_guid", "0");
        settings.endGroup();
        // PikPak
        settings.beginGroup("PikPak");
        settings.setValue("username", "");
        settings.setValue("password", "");
        settings.setValue("access_token", "");
        settings.setValue("refresh_token", "");
        settings.endGroup();
        // 下载
        settings.beginGroup("Download");
        settings.setValue("download_path", "data/download");
        settings.endGroup();
        // 数据源
        settings.beginGroup("EnabledSites");
        settings.setValue("api", "*");
        settings.setValue("site", "*");
        settings.setValue("bt", "*");
        settings.endGroup();
    }
}

QVariant getConfig(const QString &key, const QVariant &defaultValue)
{   // 读取
    QSettings settings("./config.ini", QSettings::IniFormat);
    return settings.value(key, defaultValue);
}

void setConfig(const QString &key, const QVariant &value)
{   // 写入
    QSettings settings("./config.ini", QSettings::IniFormat);
    settings.setValue(key, value);
}
