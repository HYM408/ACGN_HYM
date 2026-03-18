#include "captcha_util.h"
#include <QImage>
#include <QUrlQuery>
#include <QNetworkCookieJar>
#include <QNetworkAccessManager>
#include "charset_data.h"
#include <onnxruntime_cxx_api.h>

static const QByteArray userAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36";

static std::vector<float> ImagePreprocess(const QImage &image, const int fixedHeight, int &outWidth)
{   // 图片预处理
    const QImage gray = image.convertToFormat(QImage::Format_Grayscale8);
    const double scale = static_cast<double>(fixedHeight) / gray.height();
    const int newWidth = static_cast<int>(std::round(gray.width() * scale));
    const QImage scaled = gray.scaled(newWidth, fixedHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    outWidth = newWidth;
    std::vector<float> input(fixedHeight * newWidth);
    const uchar *bits = scaled.bits();
    const qsizetype bytesPerLine = scaled.bytesPerLine();
    for (int y = 0; y < fixedHeight; ++y) {
        const uchar *line = bits + y * bytesPerLine;
        for (int x = 0; x < newWidth; ++x) input[y * newWidth + x] = static_cast<float>(line[x]) / 255.0f;
    }
    return input;
}

static QString decodeCTC(const float *data, const int seqLen, const int numClasses, const QStringList &charset)
{   // 解码验证码
    QString result;
    int prevClass = 0;
    for (int t = 0; t < seqLen; ++t) {
        const float *row = data + t * numClasses;
        int maxClass = 0;
        float maxVal = row[0];
        for (int c = 1; c < numClasses; ++c) {
            if (row[c] > maxVal) {
                maxVal = row[c];
                maxClass = c;
            }
        }
        if (maxClass != 0 && maxClass != prevClass) result.append(charset[maxClass - 1]);
        prevClass = maxClass;
    }
    return result;
}

static QString recognizeCaptcha(const QByteArray &imageData)
{   // 验证码识别
    QImage img;
    if (!img.loadFromData(imageData) || img.isNull()) return {};
    static Ort::Env env(ORT_LOGGING_LEVEL_ERROR, "captcha-ocr");
    static Ort::SessionOptions session_options;
    static Ort::Session session(env, ORT_TSTR("common_old.onnx"), session_options);
    static const int targetHeight = [] {
        const auto shape = session.GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
        return static_cast<int>(shape[2]);
    }();
    int actualWidth = 0;
    std::vector<float> inputValues = ImagePreprocess(img, targetHeight, actualWidth);
    const std::vector<int64_t> inputShape = {1, 1, targetHeight, actualWidth};
    const auto memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    const Ort::Value inputTensor = Ort::Value::CreateTensor<float>(memoryInfo, inputValues.data(), inputValues.size(), inputShape.data(), inputShape.size());
    const Ort::AllocatorWithDefaultOptions allocator;
    const auto inputNamePtr = session.GetInputNameAllocated(0, allocator);
    const auto outputNamePtr = session.GetOutputNameAllocated(0, allocator);
    const char* inputNames[] = {inputNamePtr.get()};
    const char* outputNames[] = {outputNamePtr.get()};
    auto outputTensors = session.Run(Ort::RunOptions{}, inputNames, &inputTensor, 1, outputNames, 1);
    const float *outputData = outputTensors[0].GetTensorMutableData<float>();
    const auto actualShape = outputTensors[0].GetTensorTypeAndShapeInfo().GetShape();
    if (actualShape.size() != 3 || actualShape[1] != 1) return {};
    const int seqLen = static_cast<int>(actualShape[0]);
    const int numClasses = static_cast<int>(actualShape[2]);
    const QStringList &charset = CHARSET;
    if (charset.size() + 1 != numClasses) return {};
    return decodeCTC(outputData, seqLen, numClasses, charset);
}

void performCaptchaAsync(const QString &baseUrl, const QString &searchUrl, const QString &keyword, const std::function<void(const QString &html, const QString &error)> &callback)
{   // 验证码流程
    auto *manager = new QNetworkAccessManager();
    manager->setCookieJar(new QNetworkCookieJar(manager));
    QNetworkRequest captchaRequest(QUrl(baseUrl + "/verify/index.html"));
    captchaRequest.setRawHeader("User-Agent", userAgent);
    sendRequestUtil(*manager, captchaRequest, "GET", {}, 1, [=](const QByteArray &imageData, int, const QString &err1) {
        if (!err1.isEmpty() || imageData.isEmpty()) {
            callback({}, "获取验证码失败:" + err1);
            manager->deleteLater();
            return;
        }
        const QString captchaText = recognizeCaptcha(imageData);
        if (captchaText.isEmpty()) {
            callback({}, "验证码识别失败");
            manager->deleteLater();
            return;
        }
        QUrlQuery params;
        params.addQueryItem("type", "search");
        params.addQueryItem("verify", captchaText);
        QNetworkRequest checkRequest(baseUrl + "/index.php/ajax/verify_check");
        checkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        checkRequest.setRawHeader("User-Agent", userAgent);
        sendRequestUtil(*manager, checkRequest, "POST", params.toString().toUtf8(), 1, [=](const QByteArray &, int, const QString &err2) {
            if (!err2.isEmpty()) {
                callback({}, "验证码提交失败: " + err2);
                manager->deleteLater();
                return;
            }
            QString fullSearchUrl = baseUrl + searchUrl;
            fullSearchUrl.replace("{keyword}", keyword);
            QNetworkRequest searchRequest(fullSearchUrl);
            searchRequest.setRawHeader("User-Agent", userAgent);
            sendRequestHtml(*manager, searchRequest, "GET", {}, 1, [=](const QString &html, int, const QString &err3) {
                if (!err3.isEmpty()) callback({}, "搜索失败: " + err3);
                else callback(html, {});
                manager->deleteLater();
            }, nullptr);
        }, nullptr);
    }, nullptr);
}