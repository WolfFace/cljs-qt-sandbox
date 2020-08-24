#ifndef RUMQUICK_H
#define RUMQUICK_H

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>

class RumQuick : public QObject
{
    Q_OBJECT

public:
    explicit RumQuick(QObject *parent = nullptr);

    enum class Message {
        Info,
        Debug,
        Warning,
        Error
    };
    // Enum registration for use in QML
    Q_ENUM(Message)

    Q_INVOKABLE QJSValue createRumQuickItem(QJSValue renderFunction, QJSValue args);

signals:

public slots:
};

static QObject *rumQuickProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    RumQuick *singletonClass = new RumQuick();
    return singletonClass;
}

#endif // RUMQUICK_H
