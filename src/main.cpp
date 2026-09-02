// SPDX-License-Identifier: BSD-3-Clause
// Copyright 2026 Nitrux Latinoamericana S.C. <hello@nxos.org>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>
#include <QSurfaceFormat>
#include <QCommandLineParser>
#include <QProcess>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QMargins>
#include <QScreen>
#include <QWindow>
#include <KLocalizedString>
#include <MauiKit4/Core/mauiapp.h>
#include <LayerShellQt/Window>
#include "Controller.h"

// Helper function to get current volume from wpctl
static double getCurrentVolume() {
    QProcess wpctl;
    wpctl.start("wpctl", {"get-volume", "@DEFAULT_AUDIO_SINK@"});
    wpctl.waitForFinished();
    QString output = wpctl.readAllStandardOutput().trimmed();
    return output.split(' ').value(1).toDouble();
}

// Helper function to check if audio is muted
static bool isMuted() {
    QProcess wpctl;
    wpctl.start("wpctl", {"get-volume", "@DEFAULT_AUDIO_SINK@"});
    wpctl.waitForFinished();
    QString output = wpctl.readAllStandardOutput().trimmed();
    return output.contains("[MUTED]");
}

// Helper function to get current brightness percentage
static double getCurrentBrightness() {
    QProcess brightnessctl;
    brightnessctl.start("brightnessctl", {"get"});
    brightnessctl.waitForFinished();
    double current = brightnessctl.readAllStandardOutput().trimmed().toDouble();

    brightnessctl.start("brightnessctl", {"max"});
    brightnessctl.waitForFinished();
    double max = brightnessctl.readAllStandardOutput().trimmed().toDouble();

    return (current / max) * 100.0;
}

static void configureLayerShellWindow(QWindow *window, const Controller *controller)
{
    if (!window || !controller) {
        return;
    }

    auto *layerShellWindow = LayerShellQt::Window::get(window);
    if (!layerShellWindow) {
        return;
    }

    const QString layerScope = qEnvironmentVariableIsSet("NUDGE_OSD_LAYER_NAMESPACE")
        ? QString::fromLocal8Bit(qgetenv("NUDGE_OSD_LAYER_NAMESPACE"))
        : QStringLiteral("nudge-osd");
    layerShellWindow->setScope(layerScope);
    layerShellWindow->setLayer(LayerShellQt::Window::LayerOverlay);
    layerShellWindow->setKeyboardInteractivity(LayerShellQt::Window::KeyboardInteractivityNone);
    layerShellWindow->setWantsToBeOnActiveScreen(true);
    layerShellWindow->setCloseOnDismissed(false);
    layerShellWindow->setExclusiveZone(0);

    LayerShellQt::Window::Anchors anchors;
    anchors |= LayerShellQt::Window::AnchorBottom;
    layerShellWindow->setAnchors(anchors);
    layerShellWindow->setMargins(QMargins(0, 0, 0, controller->bottomOffset()));
    layerShellWindow->setDesiredSize(QSize(controller->osdWidth(), controller->osdHeight()));
}

int main(int argc, char *argv[]) {
    KLocalizedString::setApplicationDomain("nudge-osd");

    QGuiApplication app(argc, argv);
    app.setApplicationName("nudge-osd");
    app.setDesktopFileName("nudge-osd");
    app.setApplicationVersion("0.1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("NudgeOSD - On-screen display notifications");
    parser.addHelpOption();
    parser.addVersionOption();

    // Volume control options
    QCommandLineOption volumeUpOption(QStringList() << "volume-up", "Increase volume and show OSD [default from settings]");
    QCommandLineOption volumeDownOption(QStringList() << "volume-down", "Decrease volume and show OSD [default from settings]");
    QCommandLineOption volumeMuteOption(QStringList() << "volume-mute", "Toggle volume mute and show OSD");
    parser.addOption(volumeUpOption);
    parser.addOption(volumeDownOption);
    parser.addOption(volumeMuteOption);

    // Brightness control options
    QCommandLineOption brightnessUpOption(QStringList() << "brightness-up", "Increase brightness and show OSD [default from settings]");
    QCommandLineOption brightnessDownOption(QStringList() << "brightness-down", "Decrease brightness and show OSD [default from settings]");
    parser.addOption(brightnessUpOption);
    parser.addOption(brightnessDownOption);
    parser.addPositionalArgument("amount", "Override the configured step.", "[amount]");

    parser.process(app);

    Controller controller;
    const QString amountArgument = parser.positionalArguments().value(0);
    const auto stepAmount = [&amountArgument](int configuredAmount) {
        if (amountArgument.isEmpty()) {
            return configuredAmount;
        }

        bool ok = false;
        const int amount = amountArgument.toInt(&ok);
        return ok ? qBound(1, amount, 100) : configuredAmount;
    };

    // Check if this is a one-shot command
    bool isOneShot = parser.isSet(volumeUpOption) || parser.isSet(volumeDownOption) ||
                     parser.isSet(volumeMuteOption) || parser.isSet(brightnessUpOption) ||
                     parser.isSet(brightnessDownOption);

    if (isOneShot) {
        QDBusInterface iface("org.nxos.NudgeOSD", "/Controller", "org.nxos.Controller", QDBusConnection::sessionBus());

        if (!iface.isValid()) {
            qCritical() << "Cannot connect to NudgeOSD service. Is it running?";
            return 1;
        }

        if (parser.isSet(volumeUpOption)) {
            const int amount = stepAmount(controller.volumeStep());
            double currentVolume = getCurrentVolume();
            double newVolume = qMin(currentVolume + (amount / 100.0), 1.0);
            QProcess::execute("wpctl", {"set-volume", "@DEFAULT_AUDIO_SINK@", QString::number(newVolume)});
            iface.call("update", "volume", newVolume * 100.0);
        } else if (parser.isSet(volumeDownOption)) {
            const int amount = stepAmount(controller.volumeStep());
            double currentVolume = getCurrentVolume();
            double newVolume = qMax(currentVolume - (amount / 100.0), 0.0);
            QProcess::execute("wpctl", {"set-volume", "@DEFAULT_AUDIO_SINK@", QString::number(newVolume)});
            iface.call("update", "volume", newVolume * 100.0);
        } else if (parser.isSet(volumeMuteOption)) {
            QProcess::execute("wpctl", {"set-mute", "@DEFAULT_AUDIO_SINK@", "toggle"});
            double volume = getCurrentVolume() * 100.0;
            // Use special type "audio-volume-muted" if muted to force icon update
            QString type = isMuted() ? "audio-volume-muted" : "volume";
            iface.call("update", type, volume);
        } else if (parser.isSet(brightnessUpOption)) {
            const int amount = stepAmount(controller.brightnessStep());
            QProcess::execute("brightnessctl", {"set", QString::number(amount) + "%+"});
            iface.call("update", "brightness", getCurrentBrightness());
        } else if (parser.isSet(brightnessDownOption)) {
            const int amount = stepAmount(controller.brightnessStep());
            QProcess::execute("brightnessctl", {"set", QString::number(amount) + "%-"});
            iface.call("update", "brightness", getCurrentBrightness());
        }

        return 0;
    }

    // Running in daemon mode
    QSurfaceFormat format;
    format.setAlphaBufferSize(8);
    QSurfaceFormat::setDefaultFormat(format);

    MauiApp::instance();

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("controller", &controller);

    const QUrl url(QStringLiteral("qrc:/OSD/Main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url, &controller](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl) {
                QCoreApplication::exit(-1);
            } else if (obj && url == objUrl) {
                if (auto *window = qobject_cast<QWindow *>(obj)) {
                    const auto reconfigureWindow = [window, &controller] {
                        configureLayerShellWindow(window, &controller);
                    };
                    reconfigureWindow();

                    QObject::connect(window,
                                     &QWindow::widthChanged,
                                     window,
                                     [reconfigureWindow](int) { reconfigureWindow(); });
                    QObject::connect(window,
                                     &QWindow::heightChanged,
                                     window,
                                     [reconfigureWindow](int) { reconfigureWindow(); });
                    QObject::connect(window,
                                     &QWindow::visibleChanged,
                                     window,
                                     [reconfigureWindow](bool) { reconfigureWindow(); });
                    QObject::connect(window,
                                     &QWindow::screenChanged,
                                     window,
                                     [reconfigureWindow](QScreen *) { reconfigureWindow(); });
                    QObject::connect(&controller,
                                     &Controller::configurationChanged,
                                     window,
                                     reconfigureWindow);
                }

                QTimer::singleShot(0, &controller, &Controller::registerDBusService);
            }
        }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
