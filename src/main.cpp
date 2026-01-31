#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>
#include <QSurfaceFormat>
#include <MauiKit4/Core/mauiapp.h>
#include "Controller.h"

int main(int argc, char *argv[]) {
    QSurfaceFormat format;
    format.setAlphaBufferSize(8);
    QSurfaceFormat::setDefaultFormat(format);

    QGuiApplication app(argc, argv);
    app.setApplicationName("nudge-osd");
    app.setDesktopFileName("nudge-osd");

    // Initialize MauiKit - this loads the MauiKit plugin and makes Maui.Icon available!
    MauiApp::instance();

    // Create the Controller but don't register DBus yet
    Controller controller;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("controller", &controller);

    const QUrl url(QStringLiteral("qrc:/OSD/Main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url, &controller](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl) {
                QCoreApplication::exit(-1);
            } else if (obj && url == objUrl) {
                QTimer::singleShot(0, &controller, &Controller::registerDBusService);
            }
        }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
