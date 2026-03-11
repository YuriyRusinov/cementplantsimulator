#include <QtDebug>
#include "softplc.h"
#include <algorithm>

SoftPLC::SoftPLC()
{
#ifdef CementPlantV1DEBUG
    qDebug() << __PRETTY_FUNCTION__;
#endif
}

void SoftPLC::update(std::map<std::string, double>& ioMap, double dt) {
    // 1. Safety and Equipment Interlocks (Highest Priority)
    solveInterlocks(ioMap);

    // 2. Automations and Sequences (Start/Stop chains)
    solveSequences(ioMap);

    // 3. Regulatory Control (PIDs)
    solveControlLoops(ioMap, dt);
}

void SoftPLC::solveInterlocks(std::map<std::string, double>& ioMap) {
    // Example: ID Fan before Kiln Burner (SoW 3.3)
    bool idFanRunning = ioMap["ID_FAN_STATUS"] > 0.5;
    bool burnerCmd = ioMap["KILN_BURNER_CMD"] > 0.5;

    if (burnerCmd && !idFanRunning) {
        // Force Burner Off if Interlock is missing (SoW 3.2)
        ioMap["KILN_BURNER_STATUS"] = 0.0;
        ioMap["KILN_BURNER_INTERLOCK_FAIL"] = 1.0; // Raise Alarm for UI (SoW 3.1)
    } else {
        ioMap["KILN_BURNER_INTERLOCK_FAIL"] = 0.0;
    }

    // Example: Cooler Fans before Grate Drive (SoW 3.3)
    bool coolerFansRunning = ioMap["COOLER_FAN_TOTAL_FLOW"] > 1000.0;
    if (ioMap["GRATE_DRIVE_CMD"] > 0.5 && !coolerFansRunning) {
        ioMap["GRATE_DRIVE_STATUS"] = 0.0;
        ioMap["GRATE_INTERLOCK_ACTIVE"] = 1.0;
    }
}

void SoftPLC::solveSequences(std::map<std::string, double>& ioMap) {
    // Logic for motor group start/stop sequences (SoW 3.2)
    // If Start command received and Permissives OK, transition Status to 'Starting'
    if (ioMap["KILN_CMD_START"] > 0.5 && ioMap["KILN_BURNER_INTERLOCK_FAIL"] < 0.5) {
        ioMap["KILN_MAIN_DRIVE_STATUS"] = 1.0; // Running
        ioMap["KILN_CMD_START"] = 0.0;         // Reset Pulse
    }
}

void SoftPLC::solveControlLoops(std::map<std::string, double>& ioMap, double dt) {
    // Analog control blocks for Draft, Temp, Feed (SoW 3.3)
    if (ioMap["ID_FAN_AUTO_MODE"] > 0.5) {
        double sp = ioMap["ID_FAN_DRAFT_SP"];
        double pv = ioMap["ID_FAN_DRAFT_PV"];
        ioMap["ID_FAN_SPEED_OUT"] = calculatePID("ID_FAN_DRAFT", sp, pv, dt);
    }
}

double SoftPLC::calculatePID(const std::string& loopName, double setpoint, double pv, double dt) {
    double error = setpoint - pv;
    m_iterm[loopName] += error * dt * 0.1; // Simple Integral gain
    m_iterm[loopName] = std::clamp(m_iterm[loopName], 0.0, 100.0); // Anti-windup
    
    return std::clamp(error * 0.5 + m_iterm[loopName], 0.0, 100.0);
}
