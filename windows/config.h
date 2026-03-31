#ifndef CONFIG_H
#define CONFIG_H

#include <QVariant>

class QSettings;

void initConfig();
void setDefaultIfMissing(QSettings &settings, const QString &key, const QVariant &defaultValue);
QVariant getConfig(const QString &key, const QVariant &defaultValue = QVariant());
void setConfig(const QString &key, const QVariant &value);
QColor getColor(const QString &key, const QColor &defaultValue);

#endif // CONFIG_H