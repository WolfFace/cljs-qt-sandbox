#include "rumrender.h"

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
#include <iostream>

#include <private/qv4qobjectwrapper_p.h>

RumRender::RumRender()
{

}

void RumRender::render(QJSValue rumComp)
{
    if (this->rootRumItem != nullptr) {
        this->rootRumItem->refreshItem(rumComp, this);
    } else {
        this->rootRumItem = qobject_cast<RumQuickItemWrap*>(rumComp.toQObject());
        this->rootRumItem->refreshItem(rumComp, this);
    }
}



