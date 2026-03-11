#include "tagbridge.h"
#include <QtDebug>

TagBridge::TagBridge(QObject *parent) : QObject(parent)
{
#ifdef CementPlantV1DEBUG
    qDebug() << __PRETTY_FUNCTION__;
#endif
}

void TagBridge::writeTag(const QString& tagName, QVariant value) {
    // In a full implementation, this would call back into the CementPlant singleton
    // to update the I/O System/SoftPLC.
    emit tagChanged(tagName, value); 
}

void TagBridge::updateFromCore(const QString& tagName, double value) {
    QMutexLocker locker(&m_mutex);

    if (shouldUpdate(tagName, value)) {
        m_tagCache[tagName] = value;
        
        // Unlock before emitting to prevent potential deadlocks in UI slots
        locker.unlock(); 
        emit tagChanged(tagName, QVariant(value));
    }
}

bool TagBridge::shouldUpdate(const QString& name, double newValue) {
    if (!m_tagCache.contains(name)) return true;

    double oldValue = m_tagCache[name];
    
    // Deadband logic: Skip update if change is negligible (NFR 9.1)
    if (std::abs(oldValue) < 1e-6) return std::abs(newValue) > 1e-6;
    
    double relativeChange = std::abs((newValue - oldValue) / oldValue);
    return relativeChange > m_deadband;
}
