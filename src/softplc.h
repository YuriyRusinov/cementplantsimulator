#pragma once

#include <string>
#include <map>
#include <vector>

/**
 * @brief Logic Engine for Standard PLC Programs.
 * Handles interlocks, permissives, and sequences (SoW 3.2).
 */
class SoftPLC {
public:
    SoftPLC();
    ~SoftPLC() = default;

    // Executes one PLC scan cycle (SoW 3.2)
    void update(std::map<std::string, double>& ioMap, double dt);

private:
    // Functional Logic Blocks (SoW 3.3)
    void solveInterlocks(std::map<std::string, double>& ioMap);
    void solveSequences(std::map<std::string, double>& ioMap);
    void solveControlLoops(std::map<std::string, double>& ioMap, double dt);

    // Helper for simple PID emulation (SoW 3.3)
    double calculatePID(const std::string& loopName, double setpoint, double pv, double dt);

    // Internal state for integrators/timers
    std::map<std::string, double> m_iterm; 
};
