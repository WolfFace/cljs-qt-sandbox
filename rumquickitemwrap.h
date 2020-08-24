#ifndef RUMQUICKITEMWRAP_H
#define RUMQUICKITEMWRAP_H

#include <QObject>
#include <QQuickItem>
#include <QJSValue>
#include <QJsonDocument>
#include <QJSValueIterator>
#include <QtDebug>
#include <QQmlEngine>
#include <QQmlContext>
#include <private/qqmlmetatype_p.h>
#include <private/qquickitem_p.h>
#include <private/qqmlcontext_p.h>
#include <private/qqmlproperty_p.h>
#include <private/qjsvalue_p.h>

class RumQuickItemWrap : public QObject
{
    Q_OBJECT
public:

    explicit RumQuickItemWrap(QQuickItem *parent);
    ~RumQuickItemWrap();

    void setParentItem(QQuickItem* parentItem);
    QQuickItem *parentItem() const;
    void refreshItem(const QJSValue& jsItem, QQuickItem* where);
    void createItemInstance(const QJSValue& createItem);
    void assingProps(const QJSValue& jsItem);
    void assignSignalHandler(const QString& signalName, const QJSValue& jsFunction);
    void assignChildren(const QJSValue& childrenArray);
    void destroyItem();

    inline static QJSValueList argsList(const QJSValue& jsValue);
    inline static QQmlType resolveQmlType(const QJSValue &jsItem);
    inline static bool equalsFunctions(const QJSValue& i1, const QJSValue& i2);
    inline static bool equalsValues(const QJSValue& i1, const QJSValue& i2);
    inline static bool equalsArgs(const QJSValueList& l1, const QJSValueList& l2);

    QQuickItem* item = nullptr;
    QJSValue itemArgs;
    QJSValue renderFunction;
    QJSValue* correspondJsItem = nullptr;
    QJSValue* previousJsItem = nullptr;
    QList<RumQuickItemWrap*> children;
    QJSValueList* currentChildren = nullptr;
    QJSValueList* previousChildren = nullptr;

signals:

};

#endif // RUMQUICKITEMWRAP_H
