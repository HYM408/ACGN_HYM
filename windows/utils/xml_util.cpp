#include "xml_util.h"
#include <QUrlQuery>
#include <libxml/HTMLparser.h>

XmlDocPtr XmlUtil::parseHtml(const QString &html)
{   // 解析 HTML
    const xmlDocPtr doc = htmlReadDoc(reinterpret_cast<const xmlChar*>(html.toUtf8().constData()), nullptr, "UTF-8", HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    return XmlDocPtr(doc);
}

QStringList XmlUtil::xpath(const xmlDocPtr doc, const QString &expr)
{   // 查询 文档级
    if (!doc) return {};
    const XmlXPathContextPtr ctx(xmlXPathNewContext(doc));
    if (!ctx) return {};
    const XmlXPathObjectPtr obj(xmlXPathEvalExpression(reinterpret_cast<const xmlChar*>(expr.toUtf8().constData()), ctx.data()));
    if (!obj || !obj->nodesetval) return {};
    QStringList results;
    for (int i = 0; i < obj->nodesetval->nodeNr; ++i) {
        QString text = nodeContent(obj->nodesetval->nodeTab[i]);
        if (!text.isEmpty()) results.append(text);
    }
    return results;
}

QList<xmlNodePtr> XmlUtil::xpathNodes(const xmlDocPtr doc, const QString &expr)
{   // 查询 文档级(操作节点)
    QList<xmlNodePtr> nodes;
    if (!doc) return nodes;
    const XmlXPathContextPtr ctx(xmlXPathNewContext(doc));
    if (!ctx) return nodes;
    const XmlXPathObjectPtr obj(xmlXPathEvalExpression(reinterpret_cast<const xmlChar*>(expr.toUtf8().constData()), ctx.data()));
    if (!obj || !obj->nodesetval) return nodes;
    for (int i = 0; i < obj->nodesetval->nodeNr; ++i) nodes.append(obj->nodesetval->nodeTab[i]);
    return nodes;
}

QList<xmlNodePtr> XmlUtil::xpathNodes(const xmlNodePtr node, const QString &expr)
{   // 查询 节点级(操作节点)
    QList<xmlNodePtr> nodes;
    if (!node || !node->doc) return nodes;
    const XmlXPathContextPtr ctx(xmlXPathNewContext(node->doc));
    if (!ctx) return nodes;
    ctx->node = node;
    const XmlXPathObjectPtr obj(xmlXPathEvalExpression(reinterpret_cast<const xmlChar*>(expr.toUtf8().constData()), ctx.data()));
    if (!obj || !obj->nodesetval) return nodes;
    for (int i = 0; i < obj->nodesetval->nodeNr; ++i) nodes.append(obj->nodesetval->nodeTab[i]);
    return nodes;
}

QString XmlUtil::nodeContent(const xmlNodePtr node)
{   // 取纯内容
    if (!node) return {};
    xmlChar* content = xmlNodeGetContent(node);
    if (!content) return {};
    QString result = QString::fromUtf8(content).trimmed();
    xmlFree(content);
    return result;
}

QString XmlUtil::nodeAttribute(const xmlNodePtr node, const char *attrName)
{   // 获取节点属性
    if (!node) return {};
    xmlChar* value = xmlGetProp(node, reinterpret_cast<const xmlChar*>(attrName));
    if (!value) return {};
    QString result = QString::fromUtf8(value);
    xmlFree(value);
    return result;
}

QString XmlUtil::joinUrl(const QString &base, const QString &relative)
{   // URL 加工
    if (relative.startsWith("http://") || relative.startsWith("https://")) return relative;
    const QUrl baseUrl(base);
    const QUrl relativeUrl(relative);
    return baseUrl.resolved(relativeUrl).toString();
}

QString XmlUtil::normalizeUrl(const QString &url)
{   // URL 规范
    if (url.startsWith("http://")) return "https://" + url.mid(7);
    return url;
}

QString XmlUtil::extractNestedUrl(const QString &url, const QString &param)
{   // 处理嵌套链接
    const QUrl qurl(url);
    const QUrlQuery query(qurl.query());
    if (query.hasQueryItem(param)) return query.queryItemValue(param);
    return url;
}