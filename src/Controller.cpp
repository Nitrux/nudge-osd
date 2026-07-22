#include "Controller.h"
#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>
#include <QDir>
#include <QSettings>
#include <QtGlobal>

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    QSettings settings(QDir::homePath() + QStringLiteral("/.config/nudge-osd/nudge-osd.conf"), QSettings::IniFormat);
    const QString iconMode = settings.value(QStringLiteral("Appearance/iconMode"), QStringLiteral("system")).toString().trimmed().toLower();
    m_useNerdFont = iconMode == QLatin1String("emoji");
    m_osdWidth = qBound(160, settings.value(QStringLiteral("Appearance/width"), 292).toInt(), 800);
    m_osdHeight = qBound(56, settings.value(QStringLiteral("Appearance/height"), 66).toInt(), 200);
    m_bottomOffset = qBound(0, settings.value(QStringLiteral("Position/bottomOffset"), 114).toInt(), 1000);
    m_hideTimeout = qBound(250, settings.value(QStringLiteral("Behavior/hideTimeout"), 2000).toInt(), 10000);
    m_showAnimationDuration = qBound(0, settings.value(QStringLiteral("Behavior/showAnimationDuration"), 200).toInt(), 2000);
    m_hideAnimationDuration = qBound(0, settings.value(QStringLiteral("Behavior/hideAnimationDuration"), 200).toInt(), 2000);
    m_volumeStep = qBound(1, settings.value(QStringLiteral("Controls/volumeStep"), 5).toInt(), 100);
    m_brightnessStep = qBound(1, settings.value(QStringLiteral("Controls/brightnessStep"), 10).toInt(), 100);
}

void Controller::registerDBusService()
{
    QDBusConnection bus = QDBusConnection::sessionBus();

    if (!bus.registerService("org.nxos.NudgeOSD")) {
        qCritical() << "Failed to register DBus service:" << bus.lastError().message();
        return;
    }

    if (!bus.registerObject("/Controller", this, QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals)) {
        qCritical() << "Failed to register DBus object:" << bus.lastError().message();
        return;
    }

    qDebug() << "DBus service registered: org.nxos.NudgeOSD";
}

void Controller::update(const QString &type, double percent)
{
    bool hasValueChanged = (m_value != percent);
    if (hasValueChanged) {
        m_value = percent;
        emit valueChanged();
    }

    // Update muted state
    bool isMuted = (type == "audio-volume-muted");
    bool hasMutedChanged = (m_muted != isMuted);
    if (hasMutedChanged) {
        m_muted = isMuted;
        emit mutedChanged();
    }

    // Use clamped value for icon selection to avoid invalid icon states
    QString newIcon = mapTypeToIcon(type, qMin(percent, 100.0));
    bool hasIconChanged = (m_icon != newIcon);
    if (hasIconChanged) {
        m_icon = newIcon;
        emit iconChanged();
    }

    // Only trigger OSD if there's an actual change to display
    if (hasValueChanged || hasIconChanged || hasMutedChanged) {
        emit nudgeTriggered();
    }
}

QString Controller::mapTypeToIcon(const QString &type, double value) const
{
    // Volume icons
    if (type == "audio-volume-muted") {
        return "audio-volume-muted";
    }

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
