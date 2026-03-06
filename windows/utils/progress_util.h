#ifndef PROGRESS_UTIL_H
#define PROGRESS_UTIL_H

#include "../sql/data_structs.h"

QString computeProgressText(const CollectionData &collection, const QJsonObject &airdatesJson);

#endif // PROGRESS_UTIL_H