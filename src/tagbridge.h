#pragma once

#include <QObject>
#include <QHash>
#include <QVariant>
#include <QMutex>
#include <cmath>

/**
 * @brief Thread-safe bridge between Pure C++ Core and Qt UI.
 * Implements data filtering for high-performance DCS UI (SoW 4.1, 7.1).
 */
class TagBridge : public QObject {
    Q_OBJECT

public:
    explicit TagBridge(QObject *parent = nullptr);

    // Interface for UI to write values (Setpoints/Commands)
    void writeTag(const QString& tagName, QVariant value);

    // Interface for SimulationEngine to push updates
    void updateFromCore(const QString& tagName, double value);

signals:
    // Emitted only when value changes beyond the deadband
    void tagChanged(const QString& tagName, QVariant value);

private:
    QHash<QString, double> m_tagCache;
    QMutex m_mutex;

    // Deadband threshold: e.g., only update UI if change > 0.01%
    const double m_deadband = 0.001; 
    bool shouldUpdate(const QString& name, double newValue);
};
