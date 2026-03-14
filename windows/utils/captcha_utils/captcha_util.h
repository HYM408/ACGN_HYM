#ifndef CAPTCHA_UTIL_H
#define CAPTCHA_UTIL_H

#include "../network_util.h"

QString performCaptcha(const QString &baseUrl, const QString &searchUrl, const QString &keyword, const AbortFlag &abortFlag);

#endif // CAPTCHA_UTIL_H