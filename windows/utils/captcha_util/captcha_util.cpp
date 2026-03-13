#include "captcha_util.h"
#include <QFile>
#include <QImage>
#include <QUrlQuery>
#include <QNetworkCookieJar>
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
#ifdef _WIN32
    static const std::wstring model_path = L"common_old.onnx";
#else
    static const std::string model_path = "common_old.onnx";
#endif
    static Ort::Env env(ORT_LOGGING_LEVEL_ERROR, "captcha-ocr");
    static Ort::SessionOptions session_options;
    static Ort::Session session(env, model_path.c_str(), session_options);
    static const int targetHeight = [] {
        const auto inputInfo = session.GetInputTypeInfo(0);
        const std::vector<int64_t> shape = inputInfo.GetTensorTypeAndShapeInfo().GetShape();
        return static_cast<int>(shape[2]);
    }();
    const QStringList &charset = CHARSET;
    int actualWidth = 0;
    std::vector<float> inputValues = ImagePreprocess(img, targetHeight, actualWidth);
    const std::vector<int64_t> inputShape = {1, 1, targetHeight, actualWidth};
    const auto memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    const Ort::Value inputTensor = Ort::Value::CreateTensor<float>(memoryInfo, inputValues.data(), inputValues.size(), inputShape.data(), inputShape.size());
    const Ort::AllocatorWithDefaultOptions allocator;
    const Ort::AllocatedStringPtr inputName = session.GetInputNameAllocated(0, allocator);
    const Ort::AllocatedStringPtr outputName = session.GetOutputNameAllocated(0, allocator);
    const char *inputNames[] = {inputName.get()};
    const char *outputNames[] = {outputName.get()};
    const Ort::RunOptions runOptions;
    std::vector<Ort::Value> outputTensors = session.Run(runOptions, inputNames, &inputTensor, 1, outputNames, 1);
    const float *outputData = outputTensors[0].GetTensorMutableData<float>();
    const std::vector<int64_t> actualShape = outputTensors[0].GetTensorTypeAndShapeInfo().GetShape();
    if (actualShape.size() != 3 || actualShape[1] != 1) return {};
    const int seqLen = static_cast<int>(actualShape[0]);
    const int numClasses = static_cast<int>(actualShape[2]);
    if (charset.size() + 1 != numClasses) return {};
    return decodeCTC(outputData, seqLen, numClasses, charset);
}

QString performCaptcha(const QString &baseUrl, const QString &searchUrl,const QString &keyword, const AbortFlag &abortFlag)
{   // 验证码流程
    QNetworkAccessManager manager;
    auto *cookieJar = new QNetworkCookieJar(&manager);
    manager.setCookieJar(cookieJar);
    QNetworkRequest captchaRequest(QUrl(baseUrl + "/verify/index.html"));
    captchaRequest.setRawHeader("User-Agent", userAgent);
    const QByteArray imageData = sendRequestUtil(manager, captchaRequest, "GET", QByteArray(), 2, nullptr, nullptr, abortFlag);
    if (imageData.isEmpty()) return {};
    const QString captchaText = recognizeCaptcha(imageData);
    if (captchaText.isEmpty()) return {};
    const QUrl checkUrl(baseUrl + "/index.php/ajax/verify_check");
    QUrlQuery params;
    params.addQueryItem("type", "search");
    params.addQueryItem("verify", captchaText);
    QNetworkRequest checkRequest(checkUrl);
    checkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    checkRequest.setRawHeader("User-Agent", userAgent);
    const QByteArray postData = params.toString(QUrl::FullyEncoded).toUtf8();
    const QByteArray checkResponse = sendRequestUtil(manager, checkRequest, "POST", postData, 1, nullptr, nullptr, abortFlag);
    if (checkResponse.isEmpty()) return {};
    QString searchPath = baseUrl + searchUrl;
    searchPath.replace("{keyword}", keyword);
    QNetworkRequest searchRequest(searchPath);
    searchRequest.setRawHeader("User-Agent", userAgent);
    QString html = sendRequestHtml(manager, searchRequest, "GET", QByteArray(), 1, nullptr, nullptr, abortFlag);
    if (html.isEmpty()) return {};
    return html;
}