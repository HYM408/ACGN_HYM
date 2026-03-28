#ifndef PROGRESS_UTIL_H
#define PROGRESS_UTIL_H

#include "../sql/data_structs.h"

class DatabaseManager;

QString computeProgressText(const SubjectsData &subjectsData, const QJsonObject &airdatesJson);

#endif // PROGRESS_UTIL_H