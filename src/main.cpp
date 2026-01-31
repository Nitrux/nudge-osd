#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>
#include <QSurfaceFormat>
#include <QCommandLineParser>
#include <QProcess>
#include <QDBusInterface>
#include <QDBusConnection>
#include <MauiKit4/Core/mauiapp.h>
#include "Controller.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    app.setApplicationName("nudge-osd");
    app.setDesktopFileName("nudge-osd");
    app.setApplicationVersion("0.1.0");

    // Parse command-line arguments
    QCommandLineParser parser;
    parser.setApplicationDescription("NudgeOSD - On-screen display notifications");
    parser.addHelpOption();
    parser.addVersionOption();

    // Icon style options
    QCommandLineOption emojiOption(QStringList() << "emoji" << "nerd-font",
                                    "Use Nerd Font glyphs for icons");
    parser.addOption(emojiOption);

    QCommandLineOption sysIconsOption(QStringList() << "sys-icons" << "system-icons",
                                       "Use system theme icons (default)");
    parser.addOption(sysIconsOption);

    // Volume control options (with optional value)
    QCommandLineOption volumeUpOption(QStringList() << "volume-up", "Increase volume and show OSD [default: 5%]", "amount");
    QCommandLineOption volumeDownOption(QStringList() << "volume-down", "Decrease volume and show OSD [default: 5%]", "amount");
    QCommandLineOption volumeMuteOption(QStringList() << "volume-mute", "Toggle volume mute and show OSD");
    parser.addOption(volumeUpOption);
    parser.addOption(volumeDownOption);
    parser.addOption(volumeMuteOption);

    // Brightness control options (with optional value)
    QCommandLineOption brightnessUpOption(QStringList() << "brightness-up", "Increase brightness and show OSD [default: 10%]", "amount");
    QCommandLineOption brightnessDownOption(QStringList() << "brightness-down", "Decrease brightness and show OSD [default: 10%]", "amount");
    parser.addOption(brightnessUpOption);
    parser.addOption(brightnessDownOption);

    parser.process(app);

    // Check if this is a one-shot command (volume/brightness control)
    bool isOneShot = parser.isSet(volumeUpOption) || parser.isSet(volumeDownOption) ||
                     parser.isSet(volumeMuteOption) || parser.isSet(brightnessUpOption) ||
                     parser.isSet(brightnessDownOption);

    if (isOneShot) {
        // Send DBus command and exit
        QDBusInterface iface("org.nxos.NudgeOSD", "/Controller", "org.nxos.Controller", QDBusConnection::sessionBus());

        if (!iface.isValid()) {
            qCritical() << "Cannot connect to NudgeOSD service. Is it running?";
            return 1;
        }

        if (parser.isSet(volumeUpOption)) {
            int amount = parser.value(volumeUpOption).isEmpty() ? 5 : parser.value(volumeUpOption).toInt();
            QProcess::execute("wpctl", {"set-volume", "@DEFAULT_AUDIO_SINK@", QString::number(amount) + "%+"});
            QProcess wpctl;
            wpctl.start("wpctl", {"get-volume", "@DEFAULT_AUDIO_SINK@"});
            wpctl.waitForFinished();
            QString output = wpctl.readAllStandardOutput().trimmed();
            double volume = output.split(' ').value(1).toDouble() * 100.0;
            iface.call("update", "volume", volume);
        } else if (parser.isSet(volumeDownOption)) {
            int amount = parser.value(volumeDownOption).isEmpty() ? 5 : parser.value(volumeDownOption).toInt();
            QProcess::execute("wpctl", {"set-volume", "@DEFAULT_AUDIO_SINK@", QString::number(amount) + "%-"});
            QProcess wpctl;
            wpctl.start("wpctl", {"get-volume", "@DEFAULT_AUDIO_SINK@"});
            wpctl.waitForFinished();
            QString output = wpctl.readAllStandardOutput().trimmed();
            double volume = output.split(' ').value(1).toDouble() * 100.0;
            iface.call("update", "volume", volume);
        } else if (parser.isSet(volumeMuteOption)) {
            QProcess::execute("wpctl", {"set-mute", "@DEFAULT_AUDIO_SINK@", "toggle"});
            QProcess wpctl;
            wpctl.start("wpctl", {"get-volume", "@DEFAULT_AUDIO_SINK@"});
            wpctl.waitForFinished();
            QString output = wpctl.readAllStandardOutput().trimmed();
            double volume = output.split(' ').value(1).toDouble() * 100.0;
            iface.call("update", "volume", volume);
        } else if (parser.isSet(brightnessUpOption)) {
            int amount = parser.value(brightnessUpOption).isEmpty() ? 10 : parser.value(brightnessUpOption).toInt();
            QProcess::execute("brightnessctl", {"set", QString::number(amount) + "%+"});
            QProcess brightnessctl;
            brightnessctl.start("brightnessctl", {"get"});
            brightnessctl.waitForFinished();
            double current = brightnessctl.readAllStandardOutput().trimmed().toDouble();
            brightnessctl.start("brightnessctl", {"max"});
            brightnessctl.waitForFinished();
            double max = brightnessctl.readAllStandardOutput().trimmed().toDouble();
            double percentage = (current / max) * 100.0;
            iface.call("update", "brightness", percentage);
        } else if (parser.isSet(brightnessDownOption)) {
            int amount = parser.value(brightnessDownOption).isEmpty() ? 10 : parser.value(brightnessDownOption).toInt();
            QProcess::execute("brightnessctl", {"set", QString::number(amount) + "%-"});
            QProcess brightnessctl;
            brightnessctl.start("brightnessctl", {"get"});
            brightnessctl.waitForFinished();
            double current = brightnessctl.readAllStandardOutput().trimmed().toDouble();
            brightnessctl.start("brightnessctl", {"max"});
            brightnessctl.waitForFinished();
            double max = brightnessctl.readAllStandardOutput().trimmed().toDouble();
            double percentage = (current / max) * 100.0;
            iface.call("update", "brightness", percentage);
        }

        return 0;
    }

    // Running in daemon mode - initialize MauiKit and GUI
    QSurfaceFormat format;
    format.setAlphaBufferSize(8);
    QSurfaceFormat::setDefaultFormat(format);

    // Initialize MauiKit - this loads the MauiKit plugin and makes Maui.Icon available
    MauiApp::instance();

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
