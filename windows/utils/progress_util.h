#ifndef PROGRESS_UTIL_H
#define PROGRESS_UTIL_H

#include "../sql/data_structs.h"

class DatabaseManager;

QString computeProgressText(CollectionData &collection, const QJsonObject &airdatesJson, const DatabaseManager *db);

#endif // PROGRESS_UTIL_H