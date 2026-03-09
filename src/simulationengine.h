#pragma once

#include <string>
#include <map>
#include <vector>
#include <mutex>

/**
 * @brief The Core Math Engine (gPROMS / First-Principles Model)
 * Represents the physical cement plant: Raw Mill, Kiln, Cooler.
 */
class SimulationEngine {
public:
    SimulationEngine();
    ~SimulationEngine() = default;

    // Simulation Control (SoW 9.1)
    void step(double dt); 
    void reset();

    // Data Interface (The "I/O System" in your diagram)
    double getTagValue(const std::string& tagName);
    void setTagValue(const std::string& tagName, double value);
    
    // Batch update for the Soft PLC / TagBridge
    std::map<std::string, double> getAllTags();

    // State Management (SoW 9.2)
    bool saveSnapshot(const std::string& filePath);
    bool loadSnapshot(const std::string& filePath);

private:
    // Internal physics sub-models (SoW 2.2)
    void updateRawMill(double dt);
    void updatePyroProcess(double dt); // Preheater, Calciner, Kiln
    void updateCementMill(double dt);

    // The central memory map for all process variables
    std::map<std::string, double> m_tagBus;
    mutable std::mutex m_busMutex; // Ensures thread-safe access from TagBridge
};

