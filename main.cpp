#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "rumrender.h"
#include "rumquick.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType<RumRender>("Rum", 1, 0, "RumRender");
    qmlRegisterSingletonType<RumQuick>("RumQuick", 1, 0, "RumQuick", rumQuickProvider);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
