#include "processmodel.h"
#include <QDateTime>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <cmath>

ProcessModel::ProcessModel(QObject *parent)
    : QObject(parent)
    , m_simulationSpeed(1.0)
    , m_isRunning(false)
    , m_isPaused(false)
    , m_lastUpdateTime(0)
{
    initializeVariables();
    
    m_state.fuelFlow = 7.5;
    m_state.airFlow = 120.0;
    m_state.rawMaterialFlow = 250.0;
    m_state.kilnRunning = true;
    m_state.coolerRunning = true;
    m_state.rawMillRunning = true;
    m_state.cementMillRunning = true;
    m_state.ambientTemperature = 25.0;
    m_state.fuelHeatValue = 5500.0;
    
    m_simulationTimer = new QTimer(this);
    connect(m_simulationTimer, &QTimer::timeout, this, &ProcessModel::updateSimulation);
    m_simulationTimer->setInterval(50);
}

ProcessModel::~ProcessModel()
{
    stop();
}

void ProcessModel::initializeVariables()
{
    m_variables["kiln_feed"] = QSharedPointer<ProcessVariable>::create("Kiln Feed", "t/h", 0, 500, 250);
    m_variables["kiln_speed"] = QSharedPointer<ProcessVariable>::create("Kiln Speed", "rpm", 0, 5, 2.5);
    m_variables["kiln_torque"] = QSharedPointer<ProcessVariable>::create("Kiln Torque", "%", 0, 100, 45);
    m_variables["burner_fuel"] = QSharedPointer<ProcessVariable>::create("Burner Fuel", "t/h", 0, 15, 7.5);
    m_variables["kiln_temperature"] = QSharedPointer<ProcessVariable>::create("Kiln Temperature", "°C", 0, 2000, 1450);
    m_variables["clinker_temp"] = QSharedPointer<ProcessVariable>::create("Clinker Temperature", "°C", 0, 500, 120);
    m_variables["id_fan_speed"] = QSharedPointer<ProcessVariable>::create("ID Fan Speed", "%", 0, 100, 65);
    m_variables["kiln_draft"] = QSharedPointer<ProcessVariable>::create("Kiln Draft", "mmH2O", -50, 50, -15);
    
    m_variables["kiln_temperature"]->setAlarmHigh(1550);
    m_variables["kiln_temperature"]->setAlarmLow(1300);
    
    m_variables["preheater_temp"] = QSharedPointer<ProcessVariable>::create("Preheater Temperature", "°C", 0, 1000, 850);
    m_variables["preheater_pressure"] = QSharedPointer<ProcessVariable>::create("Preheater Pressure", "mbar", -100, 100, -45);
    m_variables["calciner_temp"] = QSharedPointer<ProcessVariable>::create("Calciner Temperature", "°C", 0, 1200, 880);
    m_variables["calciner_fuel"] = QSharedPointer<ProcessVariable>::create("Calciner Fuel", "t/h", 0, 20, 12);
    m_variables["cyclone_pressure_drop"] = QSharedPointer<ProcessVariable>::create("Cyclone ΔP", "mbar", 0, 200, 85);
    
    m_variables["preheater_temp"]->setAlarmHigh(950);
    m_variables["calciner_temp"]->setAlarmHigh(1050);
    
    m_variables["cooler_speed"] = QSharedPointer<ProcessVariable>::create("Cooler Speed", "rpm", 0, 10, 5);
    m_variables["cooler_temp_out"] = QSharedPointer<ProcessVariable>::create("Cooler Outlet Temp", "°C", 0, 300, 85);
    m_variables["cooler_fan1"] = QSharedPointer<ProcessVariable>::create("Cooler Fan 1", "%", 0, 100, 60);
    m_variables["cooler_fan2"] = QSharedPointer<ProcessVariable>::create("Cooler Fan 2", "%", 0, 100, 60);
    m_variables["cooler_fan3"] = QSharedPointer<ProcessVariable>::create("Cooler Fan 3", "%", 0, 100, 55);
    m_variables["cooler_pressure"] = QSharedPointer<ProcessVariable>::create("Cooler Pressure", "mbar", 0, 50, 15);
    
    m_variables["raw_mill_feed"] = QSharedPointer<ProcessVariable>::create("Raw Mill Feed", "t/h", 0, 400, 200);
    m_variables["raw_mill_power"] = QSharedPointer<ProcessVariable>::create("Raw Mill Power", "kW", 0, 5000, 2500);
    m_variables["raw_mill_temp_out"] = QSharedPointer<ProcessVariable>::create("Raw Mill Outlet Temp", "°C", 0, 150, 95);
    m_variables["raw_mill_pressure"] = QSharedPointer<ProcessVariable>::create("Raw Mill Pressure", "mbar", -50, 50, -15);
    m_variables["raw_mill_vibration"] = QSharedPointer<ProcessVariable>::create("Raw Mill Vibration", "mm/s", 0, 10, 2.5);
    m_variables["raw_mill_separator_speed"] = QSharedPointer<ProcessVariable>::create("Separator Speed", "rpm", 0, 200, 100);
    
    m_variables["raw_mill_vibration"]->setAlarmHigh(5.0);
    m_variables["raw_mill_temp_out"]->setAlarmHigh(120);
    
    m_variables["cement_mill_feed"] = QSharedPointer<ProcessVariable>::create("Cement Mill Feed", "t/h", 0, 300, 150);
    m_variables["cement_mill_power"] = QSharedPointer<ProcessVariable>::create("Cement Mill Power", "kW", 0, 4000, 2000);
    m_variables["cement_mill_temp"] = QSharedPointer<ProcessVariable>::create("Cement Mill Temp", "°C", 0, 150, 85);
    m_variables["cement_mill_vibration"] = QSharedPointer<ProcessVariable>::create("Cement Mill Vibration", "mm/s", 0, 8, 2.0);
    m_variables["cement_mill_delta_p"] = QSharedPointer<ProcessVariable>::create("Mill ΔP", "mbar", 0, 150, 65);
    
    m_variables["free_lime"] = QSharedPointer<ProcessVariable>::create("Free Lime", "%", 0, 5, 1.2);
    m_variables["blaine"] = QSharedPointer<ProcessVariable>::create("Blaine", "cm²/g", 2000, 5000, 3200);
    m_variables["oxygen"] = QSharedPointer<ProcessVariable>::create("O2", "%", 0, 25, 3.5);
    m_variables["co"] = QSharedPointer<ProcessVariable>::create("CO", "ppm", 0, 2000, 150);
    m_variables["no"] = QSharedPointer<ProcessVariable>::create("NO", "ppm", 0, 1000, 300);
    m_variables["c3s"] = QSharedPointer<ProcessVariable>::create("C3S", "%", 40, 70, 55);
    m_variables["c2s"] = QSharedPointer<ProcessVariable>::create("C2S", "%", 10, 30, 20);
    
    m_variables["free_lime"]->setAlarmHigh(2.0);
    m_variables["free_lime"]->setAlarmLow(0.5);
    m_variables["oxygen"]->setAlarmLow(1.5);
    m_variables["oxygen"]->setAlarmHigh(6.0);
    m_variables["co"]->setAlarmHigh(500);
    m_variables["co"]->setAlarmHigh(1000);
    
    m_variables["nox"] = QSharedPointer<ProcessVariable>::create("NOx", "mg/Nm³", 0, 1000, 350);
    m_variables["so2"] = QSharedPointer<ProcessVariable>::create("SO2", "mg/Nm³", 0, 500, 120);
    m_variables["dust"] = QSharedPointer<ProcessVariable>::create("Dust", "mg/Nm³", 0, 100, 25);
    
    m_variables["nox"]->setAlarmHigh(500);
    m_variables["so2"]->setAlarmHigh(200);
    m_variables["dust"]->setAlarmHigh(30);
    
    for (auto it = m_variables.begin(); it != m_variables.end(); ++it) {
        connect(it.value().data(), &ProcessVariable::alarmTriggered,
                this, &ProcessModel::alarmTriggered);
    }
}

void ProcessModel::start()
{
    QMutexLocker locker(&m_mutex);
    if (!m_isRunning) {
        m_isRunning = true;
        m_isPaused = false;
        m_lastUpdateTime = QDateTime::currentMSecsSinceEpoch();
        m_simulationTimer->start();
        emit isRunningChanged();
        emit processEvent("Simulation started", QVariant());
    }
}

void ProcessModel::stop()
{
    QMutexLocker locker(&m_mutex);
    if (m_isRunning) {
        m_isRunning = false;
        m_simulationTimer->stop();
        emit isRunningChanged();
        emit processEvent("Simulation stopped", QVariant());
    }
}

void ProcessModel::pause()
{
    QMutexLocker locker(&m_mutex);
    if (m_isRunning && !m_isPaused) {
        m_isPaused = true;
        m_simulationTimer->stop();
        emit isPausedChanged();
        emit processEvent("Simulation paused", QVariant());
    }
}

void ProcessModel::resume()
{
    QMutexLocker locker(&m_mutex);
    if (m_isRunning && m_isPaused) {
        m_isPaused = false;
        m_lastUpdateTime = QDateTime::currentMSecsSinceEpoch();
        m_simulationTimer->start();
        emit isPausedChanged();
        emit processEvent("Simulation resumed", QVariant());
    }
}

void ProcessModel::setSimulationSpeed(double speed)
{
    QMutexLocker locker(&m_mutex);
    if (m_simulationSpeed != speed) {
        m_simulationSpeed = qBound(0.1, speed, 10.0);
        emit simulationSpeedChanged();
        emit processEvent(QString("Simulation speed changed to %1x").arg(m_simulationSpeed), QVariant());
    }
}

void ProcessModel::updateSimulation()
{
    QMutexLocker locker(&m_mutex);
    if (!m_isRunning || m_isPaused) return;
    
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    double dt = (currentTime - m_lastUpdateTime) / 1000.0 * m_simulationSpeed;
    m_lastUpdateTime = currentTime;
    
    dt = qMin(dt, 0.1);
    
    if (dt > 0) {
        updateDynamics(dt);
        checkInterlocks();
        emit simulationUpdated();
    }
}

void ProcessModel::updateDynamics(double dt)
{
    updateKilnDynamics(dt);
    updatePreheaterDynamics(dt);
    updateCoolerDynamics(dt);
    updateMillDynamics(dt);
    updateQualityParameters(dt);
    updateEmissions(dt);
}

void ProcessModel::updateKilnDynamics(double dt)
{
    if (!m_state.kilnRunning) {
        double currentTemp = m_variables["kiln_temperature"]->value();
        double coolingRate = 50.0 * dt;
        m_variables["kiln_temperature"]->setValue(qMax(20.0, currentTemp - coolingRate));
        return;
    }
    
    auto kilnTemp = m_variables["kiln_temperature"];
    auto burnerFuel = m_variables["burner_fuel"];
    auto kilnFeed = m_variables["kiln_feed"];
    auto kilnSpeed = m_variables["kiln_speed"];
    auto idFan = m_variables["id_fan_speed"];
    
    double fuelEffect = burnerFuel->value() * 50.0 * (m_state.fuelHeatValue / 5500.0);
    double feedEffect = kilnFeed->value() * 0.5;
    double speedEffect = kilnSpeed->value() * 10.0;
    double draftEffect = idFan->value() * 0.5;
    double ambientEffect = (kilnTemp->value() - m_state.ambientTemperature) * 0.01;
    
    double tempChange = (fuelEffect - feedEffect - speedEffect - draftEffect - ambientEffect) * dt / 100.0;
    double newTemp = kilnTemp->value() + tempChange;
    kilnTemp->setValue(qBound(800.0, newTemp, 1650.0));
    
    double torqueBase = (kilnFeed->value() / 500.0) * 100.0;
    double tempFactor = 1.0 - qAbs(1450.0 - kilnTemp->value()) / 500.0;
    m_variables["kiln_torque"]->setValue(qBound(10.0, torqueBase * tempFactor, 95.0));
    
    double clinkerTemp = m_variables["clinker_temp"]->value();
    double targetClinkerTemp = kilnTemp->value() - 1330.0;
    double clinkerChange = (targetClinkerTemp - clinkerTemp) * dt / 30.0;
    m_variables["clinker_temp"]->setValue(qBound(50.0, clinkerTemp + clinkerChange, 200.0));
    
    double targetDraft = -20.0 - (idFan->value() * 0.5);
    m_variables["kiln_draft"]->setValue(targetDraft);
}

void ProcessModel::updatePreheaterDynamics(double dt)
{
    auto preheaterTemp = m_variables["preheater_temp"];
    auto calcinerTemp = m_variables["calciner_temp"];
    auto calcinerFuel = m_variables["calciner_fuel"];
    auto kilnTemp = m_variables["kiln_temperature"];
    auto idFan = m_variables["id_fan_speed"];
    auto cycloneDP = m_variables["cyclone_pressure_drop"];
    
    double targetPreheaterTemp = (kilnTemp->value() * 0.4 + calcinerTemp->value() * 0.6);
    double tempChange = (targetPreheaterTemp - preheaterTemp->value()) * dt / 20.0;
    preheaterTemp->setValue(preheaterTemp->value() + tempChange);
    
    double fuelEffect = calcinerFuel->value() * 40.0;
    double kilnGasEffect = kilnTemp->value() * 0.1;
    double targetCalcinerTemp = 800.0 + fuelEffect + kilnGasEffect * 0.5;
    double calcinerChange = (targetCalcinerTemp - calcinerTemp->value()) * dt / 15.0;
    calcinerTemp->setValue(calcinerTemp->value() + calcinerChange);
    
    double targetDP = 50.0 + (idFan->value() * 0.5);
    double dpChange = (targetDP - cycloneDP->value()) * dt / 10.0;
    cycloneDP->setValue(qBound(20.0, cycloneDP->value() + dpChange, 150.0));
    
    double targetPressure = -30.0 - (idFan->value() * 0.7);
    m_variables["preheater_pressure"]->setValue(targetPressure);
}

void ProcessModel::updateCoolerDynamics(double dt)
{
    auto coolerTemp = m_variables["cooler_temp_out"];
    auto coolerSpeed = m_variables["cooler_speed"];
    auto coolerFan1 = m_variables["cooler_fan1"];
    auto coolerFan2 = m_variables["cooler_fan2"];
    auto coolerFan3 = m_variables["cooler_fan3"];
    auto clinkerTemp = m_variables["clinker_temp"];
    auto coolerPressure = m_variables["cooler_pressure"];
    
    double coolingEffect = coolerSpeed->value() * 15.0;
    double fanEffect = (coolerFan1->value() + coolerFan2->value() + coolerFan3->value()) / 3.0;
    
    double targetTemp = 70.0 + (clinkerTemp->value() / 8.0) - coolingEffect - fanEffect / 2.0;
    targetTemp = qBound(40.0, targetTemp, 180.0);
    
    double tempChange = (targetTemp - coolerTemp->value()) * dt / 8.0;
    coolerTemp->setValue(coolerTemp->value() + tempChange);
    
    double targetPressure = 5.0 + (fanEffect / 20.0);
    coolerPressure->setValue(qBound(2.0, targetPressure, 30.0));
}

void ProcessModel::updateMillDynamics(double dt)
{
    auto rawMillPower = m_variables["raw_mill_power"];
    auto rawMillFeed = m_variables["raw_mill_feed"];
    auto rawMillTemp = m_variables["raw_mill_temp_out"];
    auto rawMillVibration = m_variables["raw_mill_vibration"];
    auto rawMillSeparator = m_variables["raw_mill_separator_speed"];
    
    if (m_state.rawMillRunning) {
        double targetPower = rawMillFeed->value() * 12.5;
        double powerChange = (targetPower - rawMillPower->value()) * dt / 5.0;
        rawMillPower->setValue(rawMillPower->value() + powerChange);
        
        double targetTemp = 80.0 + (rawMillFeed->value() / 20.0);
        double tempChange = (targetTemp - rawMillTemp->value()) * dt / 10.0;
        rawMillTemp->setValue(rawMillTemp->value() + tempChange);
        
        double targetVib = 1.0 + qAbs(rawMillFeed->value() - 200.0) / 100.0 + 
                          qAbs(rawMillSeparator->value() - 100.0) / 50.0;
        double vibChange = (targetVib - rawMillVibration->value()) * dt / 8.0;
        rawMillVibration->setValue(qBound(0.5, rawMillVibration->value() + vibChange, 8.0));
    } else {
        rawMillPower->setValue(0);
        double coolDown = (rawMillTemp->value() - m_state.ambientTemperature) * dt / 20.0;
        rawMillTemp->setValue(rawMillTemp->value() - coolDown);
    }
    
    auto cementMillPower = m_variables["cement_mill_power"];
    auto cementMillFeed = m_variables["cement_mill_feed"];
    auto cementMillTemp = m_variables["cement_mill_temp"];
    auto cementMillVib = m_variables["cement_mill_vibration"];
    auto cementMillDP = m_variables["cement_mill_delta_p"];
    
    if (m_state.cementMillRunning) {
        double targetCementPower = cementMillFeed->value() * 13.33;
        double cementPowerChange = (targetCementPower - cementMillPower->value()) * dt / 5.0;
        cementMillPower->setValue(cementMillPower->value() + cementPowerChange);
        
        double targetCementTemp = 75.0 + (cementMillFeed->value() / 15.0);
        double cementTempChange = (targetCementTemp - cementMillTemp->value()) * dt / 12.0;
        cementMillTemp->setValue(cementMillTemp->value() + cementTempChange);
        
        double targetCementVib = 1.0 + qAbs(cementMillFeed->value() - 150.0) / 75.0;
        double cementVibChange = (targetCementVib - cementMillVib->value()) * dt / 10.0;
        cementMillVib->setValue(qBound(0.5, cementMillVib->value() + cementVibChange, 7.0));
        
        double targetDP = 40.0 + (cementMillFeed->value() / 5.0);
        double dpChange = (targetDP - cementMillDP->value()) * dt / 15.0;
        cementMillDP->setValue(qBound(20.0, cementMillDP->value() + dpChange, 120.0));
    } else {
        cementMillPower->setValue(0);
        double coolDown = (cementMillTemp->value() - m_state.ambientTemperature) * dt / 25.0;
        cementMillTemp->setValue(cementMillTemp->value() - coolDown);
    }
}

void ProcessModel::updateQualityParameters(double dt)
{
    auto freeLime = m_variables["free_lime"];
    auto blaine = m_variables["blaine"];
    auto oxygen = m_variables["oxygen"];
    auto co = m_variables["co"];
    auto no = m_variables["no"];
    auto burnerFuel = m_variables["burner_fuel"];
    auto kilnTemp = m_variables["kiln_temperature"];
    auto kilnFeed = m_variables["kiln_feed"];
    auto cementMillFeed = m_variables["cement_mill_feed"];
    
    double targetFreeLime = qMax(0.2, qMin(3.5, 3.0 - (kilnTemp->value() - 1400.0) / 150.0));
    targetFreeLime += (kilnFeed->value() - 250.0) / 500.0;
    double limeChange = (targetFreeLime - freeLime->value()) * dt / 30.0;
    freeLime->setValue(qBound(0.1, freeLime->value() + limeChange, 4.0));
    
    double targetBlaine = 3000.0 + (cementMillFeed->value() / 10.0);
    targetBlaine = qBound(2500.0, targetBlaine, 4500.0);
    double blaineChange = (targetBlaine - blaine->value()) * dt / 60.0;
    blaine->setValue(blaine->value() + blaineChange);
    
    double targetO2 = 4.0 - (burnerFuel->value() / 10.0) * 2.0;
    targetO2 = qBound(1.0, targetO2, 8.0);
    double o2Change = (targetO2 - oxygen->value()) * dt / 20.0;
    oxygen->setValue(oxygen->value() + o2Change);
    
    double targetCO = 100.0 + qMax(0.0, (oxygen->value() - 2.5) * 80.0);
    targetCO += (burnerFuel->value() - 7.5) * 20.0;
    double coChange = (targetCO - co->value()) * dt / 15.0;
    co->setValue(qBound(20.0, co->value() + coChange, 1500.0));
    
    double targetNO = 200.0 + (kilnTemp->value() - 1400.0) * 1.5;
    targetNO += oxygen->value() * 20.0;
    targetNO = qBound(50.0, targetNO, 800.0);
    double noChange = (targetNO - no->value()) * dt / 25.0;
    no->setValue(no->value() + noChange);
    
    double targetC3S = 50.0 + (freeLime->value() * 10.0);
    m_variables["c3s"]->setValue(qBound(45.0, targetC3S, 65.0));
    
    double targetC2S = 25.0 - (freeLime->value() * 5.0);
    m_variables["c2s"]->setValue(qBound(15.0, targetC2S, 35.0));
}

void ProcessModel::updateEmissions(double dt)
{
    auto nox = m_variables["nox"];
    auto so2 = m_variables["so2"];
    auto dust = m_variables["dust"];
    auto kilnTemp = m_variables["kiln_temperature"];
    auto burnerFuel = m_variables["burner_fuel"];
    auto oxygen = m_variables["oxygen"];
    
    double targetNOx = 200.0 + (kilnTemp->value() - 1400.0) * 2.0;
    targetNOx += burnerFuel->value() * 10.0;
    targetNOx += oxygen->value() * 15.0;
    targetNOx = qBound(100.0, targetNOx, 800.0);
    double noxChange = (targetNOx - nox->value()) * dt / 30.0;
    nox->setValue(nox->value() + noxChange);
    
    double sulfurInFuel = 2.5;
    double targetSO2 = 80.0 + burnerFuel->value() * sulfurInFuel * 5.0;
    targetSO2 = qBound(50.0, targetSO2, 400.0);
    double so2Change = (targetSO2 - so2->value()) * dt / 45.0;
    so2->setValue(so2->value() + so2Change);
    
    double gasFlow = burnerFuel->value() * 100.0;
    double filterEfficiency = 0.995;
    double targetDust = 15.0 + (gasFlow / 1000.0) * (1.0 - filterEfficiency) * 1000.0;
    targetDust = qBound(5.0, targetDust, 80.0);
    double dustChange = (targetDust - dust->value()) * dt / 20.0;
    dust->setValue(dust->value() + dustChange);
}

void ProcessModel::checkInterlocks()
{
    auto kilnTemp = m_variables["kiln_temperature"];
    auto co = m_variables["co"];
    auto oxygen = m_variables["oxygen"];
    auto rawMillVib = m_variables["raw_mill_vibration"];
    auto cementMillVib = m_variables["cement_mill_vibration"];
    
    if (kilnTemp->value() > 1600.0 && m_state.kilnRunning) {
        m_state.kilnRunning = false;
        m_variables["burner_fuel"]->setValue(0);
        emit processEvent("HIGH TEMPERATURE INTERLOCK - Kiln stopped", QVariant());
    }
    
    if (co->value() > 1200.0 && m_state.kilnRunning) {
        m_state.kilnRunning = false;
        m_variables["burner_fuel"]->setValue(0);
        emit processEvent("HIGH CO INTERLOCK - Kiln stopped", QVariant());
    }
    
    if (oxygen->value() < 1.0 && m_state.kilnRunning) {
        m_state.kilnRunning = false;
        m_variables["burner_fuel"]->setValue(0);
        emit processEvent("LOW OXYGEN INTERLOCK - Kiln stopped", QVariant());
    }
    
    if (rawMillVib->value() > 7.0 && m_state.rawMillRunning) {
        m_state.rawMillRunning = false;
        m_variables["raw_mill_feed"]->setValue(0);
        emit processEvent("HIGH VIBRATION - Raw mill stopped", QVariant());
    }
    
    if (cementMillVib->value() > 6.0 && m_state.cementMillRunning) {
        m_state.cementMillRunning = false;
        m_variables["cement_mill_feed"]->setValue(0);
        emit processEvent("HIGH VIBRATION - Cement mill stopped", QVariant());
    }
}

void ProcessModel::resetToDefault()
{
    m_variables["kiln_feed"]->setValue(250);
    m_variables["kiln_speed"]->setValue(2.5);
    m_variables["burner_fuel"]->setValue(7.5);
    m_variables["kiln_temperature"]->setValue(1450);
    m_variables["free_lime"]->setValue(1.2);
    m_variables["oxygen"]->setValue(3.5);
    m_variables["co"]->setValue(150);
    m_variables["cooler_fan1"]->setValue(60);
    m_variables["cooler_fan2"]->setValue(60);
    m_variables["cooler_speed"]->setValue(5);
    m_variables["raw_mill_feed"]->setValue(200);
    m_variables["cement_mill_feed"]->setValue(150);
    
    m_state.fuelFlow = 7.5;
    m_state.kilnRunning = true;
    m_state.coolerRunning = true;
    m_state.rawMillRunning = true;
    m_state.cementMillRunning = true;
    
    emit processEvent("Scenario reset to default", QVariant());
}

void ProcessModel::saveState(const QString& filename)
{
    QJsonObject json;
    for (auto it = m_variables.begin(); it != m_variables.end(); ++it) {
        json[it.key()] = it.value()->value();
    }
    
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(json).toJson());
        emit processEvent(QString("State saved to %1").arg(filename), QVariant());
    }
}

void ProcessModel::loadState(const QString& filename)
{
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonObject json = doc.object();
        
        for (auto it = json.begin(); it != json.end(); ++it) {
            if (m_variables.contains(it.key())) {
                m_variables[it.key()]->setValue(it.value().toDouble());
            }
        }
        
        emit processEvent(QString("State loaded from %1").arg(filename), QVariant());
    }
}

QSharedPointer<ProcessVariable> ProcessModel::getVariable(const QString& name)
{
    if (m_variables.contains(name)) {
        return m_variables[name];
    }
    return QSharedPointer<ProcessVariable>();
}
