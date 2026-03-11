#pragma once

#include <QObject>
#include <QThread>
#include <QMutex>
#include <memory>
//
class SimulationEngine;
class SoftPLC;
class TagBridge;
/**
 * @brief Master Orchestrator (ECS Layer) for the Cement Plant Simulator.
 * Inherits QObject to manage the simulation thread lifecycle (SoW 7.1).
 */
class CementPlant : public QObject {
    Q_OBJECT

public:
    // Singleton Access (Single Source of Truth)
    static CementPlant& instance( QObject* parent=nullptr ) {
        static CementPlant _instance( parent );
        return _instance;
    }

    // Lifecycle Control (SoW 1.1, 9.1)
    void startPlant();
    void stopPlant();
    void setSimulationSpeed(double speed); // e.g., 1.0 to 10.0x (SoW 9.1)

    // Bridge Access for UI Signals/Slots (SoW 4.1)
    std::shared_ptr<TagBridge> bridge() const { return m_bridge; }

    // Instructor Interface (SoW 4.2)
    void injectFault(const std::string& faultID);
    void GUIViewControl( QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags() );

signals:
    void setWidgwet( QWidget* w );


private:
    // Private constructor for Singleton pattern
    explicit CementPlant(QObject *parent = nullptr);
    ~CementPlant();
    
    // Prevent copying
    CementPlant(const CementPlant&) = delete;
    void operator=(const CementPlant&) = delete;

    // Core Components (Software Organization Diagram)
    std::shared_ptr<SimulationEngine> m_model;  // Pure C++ Math
    std::shared_ptr<SoftPLC> m_plc;            // Pure C++ Logic
    std::shared_ptr<TagBridge> m_bridge;       // QObject UI Bridge

    // Threading (NFR 9.1)
    QThread m_simThread;
    bool m_isRunning;
    double m_simSpeed;
    QMutex m_stateMutex;

signals:
    void simulationStarted();
    void simulationStopped();
    void errorOccurred(const QString& msg);
};
