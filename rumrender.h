#ifndef RUMRENDER_H
#define RUMRENDER_H

#include <QQuickItem>
#include <QJSValue>
#include "rumquickitemwrap.h"

class RumRender : public QQuickItem
{
    Q_OBJECT
private:
//    QQuickItem* rootItem = nullptr;
    RumQuickItemWrap* rootRumItem = nullptr;
public:
    RumRender();

    Q_INVOKABLE void render(QJSValue rumComp);   // ... time cleaning ...
signals:

};

#endif // RUMRENDER_H
