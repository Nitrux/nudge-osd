#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>
#include <QSurfaceFormat>
#include <QCommandLineParser>
#include <MauiKit4/Core/mauiapp.h>
#include "Controller.h"

int main(int argc, char *argv[]) {
    QSurfaceFormat format;
    format.setAlphaBufferSize(8);
    QSurfaceFormat::setDefaultFormat(format);

    QGuiApplication app(argc, argv);
    app.setApplicationName("nudge-osd");
    app.setDesktopFileName("nudge-osd");
    app.setApplicationVersion("0.1.0");

    // Initialize MauiKit - this loads the MauiKit plugin and makes Maui.Icon available.
    MauiApp::instance();

    // Parse command-line arguments
    QCommandLineParser parser;
    parser.setApplicationDescription("NudgeOSD - On-screen display notifications");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption emojiOption(QStringList() << "emoji" << "nerd-font",
                                    "Use Nerd Font glyphs for icons");
    parser.addOption(emojiOption);

    QCommandLineOption sysIconsOption(QStringList() << "sys-icons" << "system-icons",
                                       "Use system theme icons (default)");
    parser.addOption(sysIconsOption);

    parser.process(app);

    // Create the Controller but don't register DBus yet
    Controller controller;

    // Determine icon style from command-line arguments
    // --emoji takes precedence, otherwise default to system icons
    if (parser.isSet(emojiOption)) {
        controller.setUseNerdFont(true);
    } else {
        controller.setUseNerdFont(false);
    }

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
