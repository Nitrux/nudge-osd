#pragma once
#include <QObject>

class Controller : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.nxos.Controller")
    Q_PROPERTY(double value READ value NOTIFY valueChanged)
    Q_PROPERTY(QString icon READ icon NOTIFY iconChanged)

public:
    explicit Controller(QObject *parent = nullptr);
    double value() const { return m_value; }
    QString icon() const { return m_icon; }
    void registerDBusService();

public slots:
    // This will be called via DBus
    void update(const QString &type, double percent);

signals:
    void valueChanged();
    void iconChanged();
    void nudgeTriggered();

private:
    QString mapTypeToIcon(const QString &type, double value) const;

    double m_value = 0.0;
    QString m_icon = "audio-volume-medium";
};
