#include <QtDebug>
#include "simulationengine.h"
#include <fstream>
#include <iostream>

SimulationEngine::SimulationEngine() {
    reset();
#ifdef CementPlantV1DEBUG
    qDebug() << __PRETTY_FUNCTION__;
#endif
}

void SimulationEngine::reset() {
    std::lock_guard<std::mutex> lock(m_busMutex);
    m_tagBus.clear();
    
    // Initialize default plant state (SoW 5.1)
    m_tagBus["KILN_TEMP"] = 25.0;      // Ambient
    m_tagBus["KILN_SPEED_PV"] = 0.0;   // Stopped
    m_tagBus["FEED_RATE_TPH"] = 0.0;
    m_tagBus["ID_FAN_DRAFT"] = 0.0;
}

void SimulationEngine::step(double dt) {
    // Process the physics in sequence (SoW 2.1)
    updateRawMill(dt);
    updatePyroProcess(dt);
    updateCementMill(dt);
}

void SimulationEngine::updatePyroProcess(double dt) {
    std::lock_guard<std::mutex> lock(m_busMutex);

    // Simplified First-Principles logic example (SoW 2.2)
    double fuel = m_tagBus["KILN_FUEL_FLOW"];
    double feed = m_tagBus["FEED_RATE_TPH"];
    double speed = m_tagBus["KILN_SPEED_PV"];

    // Heat balance approximation: Temp increases with fuel, decreases with feed
    double heatGain = (fuel * 0.8) - (feed * 0.2);
    m_tagBus["KILN_TEMP"] += heatGain * dt;

    // Apply constraints (NFR 9.2: Robustness)
    if (m_tagBus["KILN_TEMP"] < 25.0) m_tagBus["KILN_TEMP"] = 25.0;
}

void SimulationEngine::updateRawMill(double dt) {
    std::lock_guard<std::mutex> lock(m_busMutex);
    // Logic for VRM / Ball Mill dynamics (SoW 2.1)
}

void SimulationEngine::updateCementMill(double dt) {
    std::lock_guard<std::mutex> lock(m_busMutex);
    // Logic for finish grinding (SoW 2.1)
}

double SimulationEngine::getTagValue(const std::string& tagName) {
    std::lock_guard<std::mutex> lock(m_busMutex);
    return m_tagBus.count(tagName) ? m_tagBus[tagName] : 0.0;
}

void SimulationEngine::setTagValue(const std::string& tagName, double value) {
    std::lock_guard<std::mutex> lock(m_busMutex);
    m_tagBus[tagName] = value;
}

std::map<std::string, double> SimulationEngine::getAllTags() {
    std::lock_guard<std::mutex> lock(m_busMutex);
    return m_tagBus; // Returns a copy for safe cross-thread iteration
}

bool SimulationEngine::saveSnapshot(const std::string& filePath) {
    // Implementation of state serialization (SoW 8.3)
    return true;
}
