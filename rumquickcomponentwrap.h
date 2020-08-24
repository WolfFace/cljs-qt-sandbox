#ifndef RUMQUICKCOMPONENTWRAP_H
#define RUMQUICKCOMPONENTWRAP_H

#include <QObject>

class RumQuickComponentWrap : public QObject
{
    Q_OBJECT
public:
    explicit RumQuickComponentWrap(QObject *parent = nullptr);

signals:

};

#endif // RUMQUICKCOMPONENTWRAP_H
