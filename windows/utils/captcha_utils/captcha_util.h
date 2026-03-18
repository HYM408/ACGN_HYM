#ifndef CAPTCHA_UTIL_H
#define CAPTCHA_UTIL_H

#include "../network_util.h"

void performCaptchaAsync(const QString &baseUrl, const QString &searchUrl, const QString &keyword, const std::function<void(const QString &html, const QString &error)>& callback);

#endif // CAPTCHA_UTIL_H