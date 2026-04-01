#include "config.h"
#include <QSettings>
#include <QMutexLocker>

static QVariantMap m_config;
static QMutex m_configMutex;
static const QVariantMap m_defaults = {
    // 关闭动作
    {"CloseAction/remember", false},
    {"CloseAction/action", 1},
    // 清理缓存
    {"Cache/next_cleanup_timestamp", 0},
    // 主题
    {"Theme/color1", "#fdf7ff"},
    {"Theme/color2", "#f2ecf4"},
    {"Theme/color3", "#e1dbe4"},
    {"Theme/color4", "#e9ddff"},
    {"Theme/color5", "#ddd0f6"},
    {"Theme/color6", "#674fa5"},
    // Bangumi
    {"Bangumi/user_id", ""},
    {"Bangumi/client_id", ""},
    {"Bangumi/client_secret", ""},
    {"Bangumi/redirect_url", ""},
    {"Bangumi/access_token", ""},
    {"Bangumi/refresh_token", ""},
    {"Bangumi/expires_at", 0},
    {"Bangumi/bangumi_base_url", "https://bangumi.tv/"},
    {"Bangumi/bangumi_public_date_type", 2},
    {"Bangumi/rss_guid", "0"},
    {"Bangumi/nsfw", false},
    // PikPak
    {"PikPak/username", ""},
    {"PikPak/password", ""},
    {"PikPak/access_token", ""},
    {"PikPak/refresh_token", ""},
    // 下载
    {"Download/download_path", "data/download"},
    // 数据源
    {"EnabledSites/sites", "*"},
    // 快捷键
    {"Shortcut/suspendProcess", 27},
};

void initConfig()
{   // 初始化配置文件
    QSettings settings("./config.ini", QSettings::IniFormat);
    for (const QString &key : settings.allKeys()) m_config[key] = settings.value(key);
    QList<QString> toWrite;
    for (auto it = m_defaults.begin(); it != m_defaults.end(); ++it) {
        if (!m_config.contains(it.key())) {
            m_config[it.key()] = it.value();
            toWrite.append(it.key());
        }
    }
    if (toWrite.isEmpty()) return;
    for (const QString &key : toWrite) settings.setValue(key, m_defaults[key]);
    settings.sync();
}

QVariant getConfig(const QString &key, const QVariant &defaultValue)
{   // 读取
    QMutexLocker locker(&m_configMutex);
    if (m_config.contains(key)) return m_config[key];
    if (!defaultValue.isNull()) return defaultValue;
    auto it = m_defaults.find(key);
    return it != m_defaults.end() ? it.value() : QVariant();
}

void setConfig(const QString &key, const QVariant &value)
{   // 写入
    {
        QMutexLocker locker(&m_configMutex);
        m_config[key] = value;
    }
    QSettings settings("./config.ini", QSettings::IniFormat);
    settings.setValue(key, value);
}