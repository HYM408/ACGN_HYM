#ifndef XML_UTIL_H
#define XML_UTIL_H

#include <QScopedPointer>
#include <libxml/xpath.h>

struct XmlDocDeleter {static void cleanup(const xmlDocPtr d) {if (d) xmlFreeDoc(d);}};
struct XmlXPathCtxDeleter {static void cleanup(const xmlXPathContextPtr c) {if (c) xmlXPathFreeContext(c);}};
struct XmlXPathObjDeleter {static void cleanup(const xmlXPathObjectPtr o) {if (o) xmlXPathFreeObject(o);}};
using XmlDocPtr = QScopedPointer<xmlDoc, XmlDocDeleter>;
using XmlXPathContextPtr = QScopedPointer<xmlXPathContext, XmlXPathCtxDeleter>;
using XmlXPathObjectPtr = QScopedPointer<xmlXPathObject, XmlXPathObjDeleter>;

class XmlUtil
{
public:
    static XmlDocPtr parseHtml(const QString &html);
    static QStringList xpath(xmlDocPtr doc, const QString &expr);
    static QList<xmlNodePtr> xpathNodes(xmlDocPtr doc, const QString &expr);
    static QList<xmlNodePtr> xpathNodes(xmlNodePtr node, const QString &expr);
    static QString nodeContent(xmlNodePtr node);
    static QString nodeAttribute(xmlNodePtr node, const char *attrName);
    static QString joinUrl(const QString &base, const QString &relative);
    static QString normalizeUrl(const QString &url);
    static QString extractNestedUrl(const QString &url, const QString &param);
};

#endif // XML_UTIL_H