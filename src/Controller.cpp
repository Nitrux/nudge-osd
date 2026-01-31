#include "Controller.h"
#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    // DBus registration will be done later via registerDBusService()
}

void Controller::registerDBusService()
{
    // Register the object on the Session Bus
    QDBusConnection bus = QDBusConnection::sessionBus();

    if (!bus.registerService("org.nxos.NudgeOSD")) {
        qCritical() << "Failed to register DBus service:" << bus.lastError().message();
    }

    if (!bus.registerObject("/Controller", this, QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals)) {
        qCritical() << "Failed to register DBus object:" << bus.lastError().message();
    }

    qDebug() << "DBus service registered: org.nxos.NudgeOSD";
    qDebug() << "DBus object path: /Controller";
}

void Controller::update(const QString &type, double percent)
{
    qDebug() << "UPDATE called: type =" << type << ", percent =" << percent;

    // Clamp the percentage between 0 and 100
    double clampedValue = qBound(0.0, percent, 100.0);

    if (m_value != clampedValue) {
        m_value = clampedValue;
        qDebug() << "Value changed to:" << m_value;
        emit valueChanged();
    }

    // Map type to appropriate icon
    QString newIcon = mapTypeToIcon(type, clampedValue);
    if (m_icon != newIcon) {
        m_icon = newIcon;
        qDebug() << "Icon changed to:" << m_icon;
        emit iconChanged();
    }

    // Trigger the 'show' logic in QML
    qDebug() << "Emitting nudgeTriggered signal";
    emit nudgeTriggered();
}

QString Controller::mapTypeToIcon(const QString &type, double value) const
{
    // Volume icons
    if (type == "volume" || type == "audio-volume") {
        if (value == 0.0)
            return "audio-volume-muted";
        else if (value < 33.0)
            return "audio-volume-low";
        else if (value < 66.0)
            return "audio-volume-medium";
        else
            return "audio-volume-high";
    }

    // Brightness icons
    if (type == "brightness" || type == "display-brightness") {
        if (value < 33.0)
            return "brightness-low";
        else if (value < 66.0)
            return "brightness-medium";
        else
            return "brightness-high";
    }

    // Keyboard brightness
    if (type == "keyboard-brightness") {
        if (value == 0.0)
            return "keyboard-brightness-off";
        else if (value < 50.0)
            return "keyboard-brightness-low";
        else
            return "keyboard-brightness-high";
    }

    // Media controls
    if (type == "media-play")
        return "media-playback-start";
    if (type == "media-pause")
        return "media-playback-pause";
    if (type == "media-stop")
        return "media-playback-stop";
    if (type == "media-next")
        return "media-skip-forward";
    if (type == "media-previous")
        return "media-skip-backward";

    // Microphone
    if (type == "microphone" || type == "microphone-sensitivity") {
        if (value == 0.0)
            return "microphone-sensitivity-muted";
        else if (value < 50.0)
            return "microphone-sensitivity-low";
        else
            return "microphone-sensitivity-high";
    }

    // Battery
    if (type == "battery") {
        if (value < 10.0)
            return "battery-caution";
        else if (value < 30.0)
            return "battery-low";
        else if (value < 80.0)
            return "battery-good";
        else
            return "battery-full";
    }

    // Fallback to the type itself (might be a direct icon name)
    return type;
}
