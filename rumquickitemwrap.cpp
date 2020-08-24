#include "rumquickitemwrap.h"

// TODO: enable args
struct QObjectSlotDispatcher : public QtPrivate::QSlotObjectBase
{
    QJSValue func;
    int signalIndex;
    QObjectSlotDispatcher()
        : QtPrivate::QSlotObjectBase(&impl)
        , signalIndex(-1)
    {}
    static void impl(int which, QSlotObjectBase *this_, QObject *r, void **metaArgs, bool *ret)
    {
        switch (which) {
        case Destroy: {
            auto thisDispatcher = (QObjectSlotDispatcher*) this_;
            delete thisDispatcher;
        }
        break;
        case Call: {
            auto thisDispatcher = (QObjectSlotDispatcher*) this_;
            thisDispatcher->func.call();
        }
        break;
        case Compare: {
            qDebug() << "Try to compare!";
        }
        break;
        case NumOperations:
        break;
        }
    };
};





RumQuickItemWrap::RumQuickItemWrap(QQuickItem *parent) : QObject(parent)
{}

RumQuickItemWrap::~RumQuickItemWrap()
{
    qDebug() << "RumQuickItemWrap deletes!";
    foreach (RumQuickItemWrap* c, this->children) {
        c->item->setParentItem(nullptr);
        delete c;
    }
    delete this->correspondJsItem;
    delete this->previousJsItem;
    delete this->currentChildren;
    delete this->previousChildren;
    delete this->item;
}

void RumQuickItemWrap::setParentItem(QQuickItem *parentItem)
{
    this->setParent(parentItem);
}

QQuickItem *RumQuickItemWrap::parentItem() const
{
    return qobject_cast<QQuickItem*>(this->parent());
}

void RumQuickItemWrap::refreshItem(const QJSValue &jsItem, QQuickItem *where)
{
    this->setParentItem(where);

    bool haveRebuild = false;

    if (this->previousJsItem != nullptr) {
        delete previousJsItem;
    }
    this->previousJsItem = this->correspondJsItem;

    if (jsItem.isQObject()) {
        RumQuickItemWrap* newItem = qobject_cast<RumQuickItemWrap*>(jsItem.toQObject());

        if (!RumQuickItemWrap::equalsFunctions(this->renderFunction, newItem->renderFunction)) {
            this->renderFunction = newItem->renderFunction;
            haveRebuild = true;
        }
        if (!RumQuickItemWrap::equalsArgs(argsList(newItem->itemArgs), argsList(this->itemArgs))) {
            this->itemArgs = newItem->itemArgs;
            haveRebuild = true;
        }
        if (haveRebuild) {
            this->correspondJsItem = new QJSValue(this->renderFunction.call(argsList(this->itemArgs)));
        }

        if (newItem != this) {
            newItem->deleteLater();
        }
    } else {
        this->renderFunction = QJSValue::NullValue;
        this->itemArgs = QJSValue::NullValue;
        this->correspondJsItem = new QJSValue(jsItem);

        if (this->previousJsItem) {
            haveRebuild = !RumQuickItemWrap::equalsValues(
                        this->previousJsItem->property("type"),
                        this->correspondJsItem->property("type"));
        }
    }

    if (haveRebuild || this->previousJsItem == nullptr) {
        this->destroyItem();
        this->createItemInstance(*this->correspondJsItem);
    }
    item->setParent(this);
    item->setParentItem(where);

    this->assingProps(*this->correspondJsItem);
}

void RumQuickItemWrap::createItemInstance(const QJSValue &createItem)
{
    QObject *object = nullptr;
    QQmlContext* context = QQmlEngine::contextForObject(this->parentItem());
    QQmlType type = RumQuickItemWrap::resolveQmlType(createItem);

    if (type.isValid()) {
        if ( type.isComposite()) {
             QQmlComponent component(context->engine(), type.sourceUrl());
             object = component.beginCreate(context);
             component.completeCreate();
             QQmlEngine::setObjectOwnership(object, QQmlEngine::CppOwnership);
        } else
        {
            if (type.typeName() == "QQmlComponent") {
                object = new QQmlComponent(context->engine(), nullptr);
            } else  {
                object = type.create();
            }
        }
    }

    if (object && QQmlEngine::contextForObject(object) == nullptr) {
        QQmlEngine::setContextForObject(object, context);
    }
    QQmlEngine::setObjectOwnership(object, QQmlEngine::CppOwnership);

    if (!object) {
        qWarning() << "RumRender: Cannot create an object of type"
                   << QString::fromLatin1("%1 %2,%3")
                      .arg(QString(type.typeName()))
                      .arg(type.majorVersion())
                      .arg(type.minorVersion())
                   << "- type isn't known to declarative meta type system";
    }

    QQuickItem* item = qobject_cast<QQuickItem*>(object);
    this->item = item;
}

void RumQuickItemWrap::assingProps(const QJSValue &jsItem)
{
    QJSValue props = jsItem.property("props");

    if (props.isNull()) {
        return;
    }

    QJSValueIterator it(props);
    while (it.hasNext()) {
        it.next();
        QJSValue previousProperty = QJSValue::UndefinedValue;
        if (this->previousJsItem != nullptr) {
            previousProperty = this->previousJsItem->property("props").property(it.name());
        }

        if (it.name().startsWith("on") && it.value().isCallable()) {
            this->assignSignalHandler(it.name(), it.value());
        } else if (it.name() != "children") {
            if (it.value().isQObject()) {
                // TODO: check
                QQmlEngine::contextForObject(this->item)->setContextProperty(it.name(), it.value().toQObject());
            } else {
                this->item->setProperty(it.name().toStdString().data(), it.value().toVariant());
            }

        } else if (it.name() == "children") {
            this->assignChildren(it.value());
        }
    }
}

void RumQuickItemWrap::assignSignalHandler(const QString &signalName, const QJSValue &jsFunction)
{
    qDebug() << "Assign signal " << signalName;
    QString formattedSignalName(signalName);
    formattedSignalName.remove(0, 2);
    formattedSignalName.replace(0, 1, formattedSignalName.at(0).toLower());

    int signal = item->metaObject()->indexOfSignal(formattedSignalName.toLatin1() + "()");

    if (signal != -1) {
        QObjectSlotDispatcher* dispatcher = new QObjectSlotDispatcher;
        dispatcher->func = jsFunction;
        dispatcher->signalIndex = signal;

        if (QQmlData *ddata = QQmlData::get(item)) {
            if (QQmlPropertyCache *propertyCache = ddata->propertyCache) {
                QQmlPropertyPrivate::flushSignal(item, propertyCache->methodIndexToSignalIndex(signal));
            }
        }

        static QMap<QString, QMetaObject::Connection> connections;
        if (connections[signalName]) {
            item->disconnect(connections[signalName]);
        }
        connections[signalName] = QObjectPrivate::connect(item, signal, dispatcher, Qt::QueuedConnection);
    }
}

void RumQuickItemWrap::assignChildren(const QJSValue &childrenArray)
{
    QJSValueList list;
    if (childrenArray.isArray()) {
        quint32 size = childrenArray.property("length").toUInt();
        for (quint32 i = 0; i < size; i++) {
            QJSValue child = childrenArray.property(i);
            list.append(child);
        }
    } else if (childrenArray.isObject()) {
        list.append(childrenArray);
    }

    QList<RumQuickItemWrap*> pasteList;
    QListIterator<RumQuickItemWrap*> it(this->children);
    foreach (QJSValue childToPaste, list) {
        RumQuickItemWrap* toAppend;
        if (it.hasNext()) {
            toAppend = it.next();
            toAppend->refreshItem(childToPaste, this->item);
        } else {
            if (childToPaste.isQObject()) {
                toAppend = qobject_cast<RumQuickItemWrap*>(childToPaste.toQObject());
                toAppend->refreshItem(childToPaste, this->item);
            } else if (childToPaste.isObject()) {
                toAppend = new RumQuickItemWrap(this->item);
                toAppend->setParentItem(this->item);
                toAppend->refreshItem(childToPaste, this->item);
            }
        }

        if (!pasteList.isEmpty()) {
            toAppend->item->stackAfter(pasteList.last()->item);
        }
        pasteList.append(toAppend);
    }

    // remove "tailed"
    while (it.hasNext()) {
        qDebug() << "REMOVE TAIL!";
        RumQuickItemWrap* existedItem = it.next();
        existedItem->deleteLater();
    }

    this->children = QList<RumQuickItemWrap*>(pasteList);
}

void RumQuickItemWrap::destroyItem()
{
    if (this->item != nullptr) {
        this->item->setParentItem(nullptr);
        this->item->deleteLater();
    }
}

QJSValueList RumQuickItemWrap::argsList(const QJSValue& jsValue)
{
    QJSValueList list;

    if (jsValue.isNull() || jsValue.isUndefined()) {
        return list;
    }

    quint32 size = jsValue.property("length").toUInt();
    for (quint32 i = 0; i < size; i++) {
        list.append(jsValue.property(i));
    }
    return list;
}

QQmlType RumQuickItemWrap::resolveQmlType(const QJSValue &jsItem)
{
    static QMap<QString, QQmlType> typeCache;

    QString typeName = jsItem.property("type").toString();
    QString qtypeName = "QQuick" + typeName;

    QQmlType type;
    if (typeCache.contains(typeName)) {
        return typeCache[typeName];
    }

    auto qmlTypes = QQmlMetaType::qmlAllTypes();
    foreach (QQmlType t, qmlTypes) {
        if ((t.typeName() == qtypeName.toLatin1() && t.majorVersion() >= 1)
                || t.sourceUrl().fileName() == (typeName.toLatin1() + ".qml")) {
            qDebug() << "Item " << typeName << " created!";
            type = t;
            break;
        }
    }

    typeCache[typeName] = type;
    return type;
}

bool RumQuickItemWrap::equalsFunctions(const QJSValue &i1, const QJSValue &i2)
{
    return false;
}

bool RumQuickItemWrap::equalsValues(const QJSValue &i1, const QJSValue &i2)
{
    return i1.strictlyEquals(i2);
}

bool RumQuickItemWrap::equalsArgs(const QJSValueList &l1, const QJSValueList &l2)
{
    if (l1.count() != l2.count()) {
        return false;
    }

    QListIterator<QJSValue> it1(l1);
    QListIterator<QJSValue> it2(l2);
    while (it1.hasNext()) {
        if (!RumQuickItemWrap::equalsValues(it1.next(), it2.next())) {
            return false;
        }
    }
    return true;
}
