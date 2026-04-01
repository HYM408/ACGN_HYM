#ifndef CONFIG_H
#define CONFIG_H

#include <QVariant>

class QSettings;

void initConfig();
QVariant getConfig(const QString &key, const QVariant &defaultValue = QVariant());
void setConfig(const QString &key, const QVariant &value);

#endif // CONFIG_H