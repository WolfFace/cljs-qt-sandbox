#include "rumquick.h"
#include <QtDebug>

#include "rumquickitemwrap.h"


RumQuick::RumQuick(QObject *parent) : QObject(parent)
{}

QJSValue RumQuick::createRumQuickItem(QJSValue renderFunction, QJSValue args)
{
    RumQuickItemWrap* rumQuickItem = new RumQuickItemWrap(nullptr);
    rumQuickItem->renderFunction = renderFunction;
    rumQuickItem->itemArgs = args;
    return QJSValue(renderFunction.engine()->newQObject(rumQuickItem));
}
