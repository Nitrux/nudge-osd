#pragma once
#include <QObject>

class Controller : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.nxos.Controller")
    Q_PROPERTY(double value READ value NOTIFY valueChanged)
    Q_PROPERTY(QString icon READ icon NOTIFY iconChanged)
    Q_PROPERTY(bool useNerdFont READ useNerdFont CONSTANT)
    Q_PROPERTY(bool muted READ muted NOTIFY mutedChanged)
    Q_PROPERTY(int osdWidth READ osdWidth CONSTANT)
    Q_PROPERTY(int osdHeight READ osdHeight CONSTANT)
    Q_PROPERTY(int hideTimeout READ hideTimeout CONSTANT)
    Q_PROPERTY(int showAnimationDuration READ showAnimationDuration CONSTANT)
    Q_PROPERTY(int hideAnimationDuration READ hideAnimationDuration CONSTANT)

public:
    explicit Controller(QObject *parent = nullptr);

    double value() const { return m_value; }
    QString icon() const { return m_icon; }
    bool useNerdFont() const { return m_useNerdFont; }
    bool muted() const { return m_muted; }
    int osdWidth() const { return m_osdWidth; }
    int osdHeight() const { return m_osdHeight; }
    int bottomOffset() const { return m_bottomOffset; }
    int hideTimeout() const { return m_hideTimeout; }
    int showAnimationDuration() const { return m_showAnimationDuration; }
    int hideAnimationDuration() const { return m_hideAnimationDuration; }
    int volumeStep() const { return m_volumeStep; }
    int brightnessStep() const { return m_brightnessStep; }
    void registerDBusService();

public slots:
    void update(const QString &type, double percent);

signals:
    void valueChanged();
    void iconChanged();
    void mutedChanged();
    void nudgeTriggered();

private:
    QString mapTypeToIcon(const QString &type, double value) const;

    double m_value = 0.0;
    QString m_icon = "audio-volume-medium";
    bool m_useNerdFont = false;
    bool m_muted = false;
    int m_osdWidth = 292;
    int m_osdHeight = 66;
    int m_bottomOffset = 114;
    int m_hideTimeout = 2000;
    int m_showAnimationDuration = 200;
    int m_hideAnimationDuration = 200;
    int m_volumeStep = 5;
    int m_brightnessStep = 10;
};
