#pragma once

#include <QObject>
#include <QTimer>
#include <QMap>
#include <QSharedPointer>
#include <QMutex>
#include "processvariable.h"

class ProcessModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double simulationSpeed READ simulationSpeed WRITE setSimulationSpeed NOTIFY simulationSpeedChanged)
    Q_PROPERTY(bool isRunning READ isRunning NOTIFY isRunningChanged)
    Q_PROPERTY(bool isPaused READ isPaused NOTIFY isPausedChanged)

public:
    explicit ProcessModel(QObject *parent = nullptr);
    ~ProcessModel();

    QSharedPointer<ProcessVariable> getVariable(const QString& name);
    QMap<QString, QSharedPointer<ProcessVariable>> getAllVariables() const { return m_variables; }
    
    double simulationSpeed() const { return m_simulationSpeed; }
    void setSimulationSpeed(double speed);
    void start();
    void stop();
    void pause();
    void resume();
    bool isRunning() const { return m_isRunning; }
    bool isPaused() const { return m_isPaused; }
    
    void resetToDefault();
    void saveState(const QString& filename);
    void loadState(const QString& filename);
    
    double getKilnTemperature() const { return m_variables["kiln_temperature"]->value(); }
    double getKilnSpeed() const { return m_variables["kiln_speed"]->value(); }
    double getKilnFeed() const { return m_variables["kiln_feed"]->value(); }
    double getBurnerFuel() const { return m_variables["burner_fuel"]->value(); }
    double getFreeLime() const { return m_variables["free_lime"]->value(); }
    double getOxygen() const { return m_variables["oxygen"]->value(); }
    double getCO() const { return m_variables["co"]->value(); }
    double getRawMillPower() const { return m_variables["raw_mill_power"]->value(); }
    double getCementMillPower() const { return m_variables["cement_mill_power"]->value(); }

signals:
    void simulationSpeedChanged();
    void simulationUpdated();
    void isRunningChanged();
    void isPausedChanged();
    void processEvent(const QString& event, const QVariant& data);
    void alarmTriggered(const QString& tag, double value, double limit);

private slots:
    void updateSimulation();

private:
    void initializeVariables();
    void updateDynamics(double dt);
    void checkInterlocks();
    
    void updateKilnDynamics(double dt);
    void updatePreheaterDynamics(double dt);
    void updateCoolerDynamics(double dt);
    void updateMillDynamics(double dt);
    void updateQualityParameters(double dt);
    void updateEmissions(double dt);
    
    QMap<QString, QSharedPointer<ProcessVariable>> m_variables;
    QTimer* m_simulationTimer;
    double m_simulationSpeed;
    bool m_isRunning;
    bool m_isPaused;
    qint64 m_lastUpdateTime;
    QMutex m_mutex;
    
    struct ProcessState {
        double fuelFlow;
        double airFlow;
        double rawMaterialFlow;
        bool kilnRunning;
        bool coolerRunning;
        bool rawMillRunning;
        bool cementMillRunning;
        double ambientTemperature;
        double fuelHeatValue;
    } m_state;
};
