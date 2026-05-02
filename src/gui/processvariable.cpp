#include "processvariable.h"
#include <cmath>
#include <algorithm>

ProcessVariable::ProcessVariable(QObject *parent) 
    : QObject(parent)
    , m_value(0.0)
    , m_minValue(0.0)
    , m_maxValue(100.0)
    , m_alarmHigh(0.0)
    , m_alarmLow(0.0)
    , m_alarmActive(false)
    , m_isReadOnly(false)  // <-- Initialize m_isReadOnly
    , m_maxHistorySize(DEFAULT_HISTORY_SIZE)
    , m_cachedAverage(0.0)
    , m_cachedMin(0.0)
    , m_cachedMax(0.0)
    , m_cachedStdDev(0.0)
{
}

ProcessVariable::ProcessVariable(const QString& name, const QString& unit, 
                               double minVal, double maxVal, double defaultVal, 
                               QObject *parent)
    : QObject(parent)
    , m_name(name)
    , m_unit(unit)
    , m_value(defaultVal)
    , m_minValue(minVal)
    , m_maxValue(maxVal)
    , m_alarmHigh(maxVal)
    , m_alarmLow(minVal)
    , m_alarmActive(false)
    , m_isReadOnly(false)  // <-- Initialize m_isReadOnly
    , m_maxHistorySize(DEFAULT_HISTORY_SIZE)
    , m_cachedAverage(defaultVal)
    , m_cachedMin(defaultVal)
    , m_cachedMax(defaultVal)
    , m_cachedStdDev(0.0)
{
}

void ProcessVariable::setValue(double value)
{
    QMutexLocker locker(&m_mutex);
    
    // Check read-only flag
    if (m_isReadOnly) {
        return;  // Don't allow modification of read-only variables
    }
    
    double newValue = qBound(m_minValue, value, m_maxValue);
    
    if (qAbs(m_value - newValue) > 0.0001) {
        m_value = newValue;
        
        DataPoint point;
        point.timestamp = QDateTime::currentDateTime();
        point.value = m_value;
        point.isAlarm = false;
        addToHistory(point);
        
        m_statsUpdateTime = QDateTime();
        
        emit valueChanged();
        checkAlarms();
    }
}

void ProcessVariable::addToValue(double delta)
{
    QMutexLocker locker(&m_mutex);
    
    // Check read-only flag
    if (m_isReadOnly) {
        return;
    }
    
    setValue(m_value + delta);
}

void ProcessVariable::setValueClamped(double value)
{
    setValue(value);  // setValue already clamps and checks read-only
}

double ProcessVariable::getNormalizedValue() const
{
    QMutexLocker locker(&m_mutex);
    if (m_maxValue == m_minValue) return 0.5;
    return (m_value - m_minValue) / (m_maxValue - m_minValue);
}

void ProcessVariable::checkAlarms()
{
    bool wasActive = m_alarmActive;
    
    if (m_alarmHigh > m_minValue && m_value > m_alarmHigh) {
        if (!wasActive) {
            m_alarmActive = true;
            emit alarmTriggered(m_name, m_value, m_alarmHigh);
            
            DataPoint point;
            point.timestamp = QDateTime::currentDateTime();
            point.value = m_value;
            point.isAlarm = true;
            addToHistory(point);
        }
    }
    else if (m_alarmLow < m_maxValue && m_value < m_alarmLow) {
        if (!wasActive) {
            m_alarmActive = true;
            emit alarmTriggered(m_name, m_value, m_alarmLow);
            
            DataPoint point;
            point.timestamp = QDateTime::currentDateTime();
            point.value = m_value;
            point.isAlarm = true;
            addToHistory(point);
        }
    }
    else {
        if (wasActive) {
            m_alarmActive = false;
            emit alarmCleared(m_name, m_value);
        }
    }
    
    if (wasActive != m_alarmActive) {
        emit alarmActiveChanged();
    }
}

void ProcessVariable::addToHistory(const DataPoint& point)
{
    m_history.append(point);
    
    while (m_history.size() > m_maxHistorySize) {
        m_history.removeFirst();
    }
}

QVector<ProcessVariable::DataPoint> ProcessVariable::getHistory(int seconds) const
{
    QMutexLocker locker(&m_mutex);
    
    QVector<DataPoint> result;
    QDateTime cutoff = QDateTime::currentDateTime().addSecs(-seconds);
    
    for (const auto& point : m_history) {
        if (point.timestamp >= cutoff) {
            result.append(point);
        }
    }
    
    return result;
}

void ProcessVariable::clearHistory()
{
    QMutexLocker locker(&m_mutex);
    m_history.clear();
    m_statsUpdateTime = QDateTime();
}

void ProcessVariable::updateStatistics() const
{
    QMutexLocker locker(&m_mutex);
    
    QDateTime now = QDateTime::currentDateTime();
    if (m_statsUpdateTime.isValid() && m_statsUpdateTime.secsTo(now) < 5) {
        return;
    }
    
    QVector<DataPoint> recentHistory = getHistory(300);
    
    if (recentHistory.isEmpty()) {
        m_cachedAverage = m_value;
        m_cachedMin = m_value;
        m_cachedMax = m_value;
        m_cachedStdDev = 0.0;
        return;
    }
    
    double sum = 0.0;
    m_cachedMin = recentHistory[0].value;
    m_cachedMax = recentHistory[0].value;
    
    for (const auto& point : recentHistory) {
        sum += point.value;
        if (point.value < m_cachedMin) m_cachedMin = point.value;
        if (point.value > m_cachedMax) m_cachedMax = point.value;
    }
    
    m_cachedAverage = sum / recentHistory.size();
    
    double variance = 0.0;
    for (const auto& point : recentHistory) {
        double diff = point.value - m_cachedAverage;
        variance += diff * diff;
    }
    variance /= recentHistory.size();
    m_cachedStdDev = std::sqrt(variance);
    
    m_statsUpdateTime = now;
}

double ProcessVariable::getAverage(int seconds) const
{
    updateStatistics();
    return m_cachedAverage;
}

double ProcessVariable::getMin(int seconds) const
{
    updateStatistics();
    return m_cachedMin;
}

double ProcessVariable::getMax(int seconds) const
{
    updateStatistics();
    return m_cachedMax;
}

double ProcessVariable::getStdDev(int seconds) const
{
    updateStatistics();
    return m_cachedStdDev;
}

void ProcessVariable::resetAlarm()
{
    QMutexLocker locker(&m_mutex);
    
    if (m_alarmActive) {
        m_alarmActive = false;
        emit alarmCleared(m_name, m_value);
        emit alarmActiveChanged();
    }
}

void ProcessVariable::setName(const QString& name)
{
    QMutexLocker locker(&m_mutex);
    if (m_name != name) {
        m_name = name;
        emit nameChanged();
    }
}

void ProcessVariable::setUnit(const QString& unit)
{
    QMutexLocker locker(&m_mutex);
    if (m_unit != unit) {
        m_unit = unit;
        emit unitChanged();
    }
}

void ProcessVariable::setDescription(const QString& desc)
{
    QMutexLocker locker(&m_mutex);
    if (m_description != desc) {
        m_description = desc;
        emit descriptionChanged();
    }
}

void ProcessVariable::setMinValue(double minVal)
{
    QMutexLocker locker(&m_mutex);
    if (m_minValue != minVal) {
        m_minValue = minVal;
        emit minValueChanged();
        
        if (m_value < m_minValue) {
            setValue(m_value);
        }
    }
}

void ProcessVariable::setMaxValue(double maxVal)
{
    QMutexLocker locker(&m_mutex);
    if (m_maxValue != maxVal) {
        m_maxValue = maxVal;
        emit maxValueChanged();
        
        if (m_value > m_maxValue) {
            setValue(m_value);
        }
    }
}

void ProcessVariable::setAlarmHigh(double high)
{
    QMutexLocker locker(&m_mutex);
    if (m_alarmHigh != high) {
        m_alarmHigh = high;
        emit alarmHighChanged();
        checkAlarms();
    }
}

void ProcessVariable::setAlarmLow(double low)
{
    QMutexLocker locker(&m_mutex);
    if (m_alarmLow != low) {
        m_alarmLow = low;
        emit alarmLowChanged();
        checkAlarms();
    }
}

void ProcessVariable::setReadOnly(bool readOnly)
{
    QMutexLocker locker(&m_mutex);
    if (m_isReadOnly != readOnly) {
        m_isReadOnly = readOnly;
        emit readOnlyChanged();
        
        // Log the change
        QString msg = QString("Variable %1 set to %2 mode")
                          .arg(m_name)
                          .arg(readOnly ? "READ ONLY" : "READ/WRITE");
        emit descriptionChanged();  // Use this to signal change
    }
}
