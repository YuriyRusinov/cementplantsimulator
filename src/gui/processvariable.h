#pragma once

#include <QObject>
#include <QVector>
#include <QDateTime>
#include <QMutex>

class ProcessVariable : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString unit READ unit WRITE setUnit NOTIFY unitChanged)
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(double minValue READ minValue WRITE setMinValue NOTIFY minValueChanged)
    Q_PROPERTY(double maxValue READ maxValue WRITE setMaxValue NOTIFY maxValueChanged)
    Q_PROPERTY(double alarmHigh READ alarmHigh WRITE setAlarmHigh NOTIFY alarmHighChanged)
    Q_PROPERTY(double alarmLow READ alarmLow WRITE setAlarmLow NOTIFY alarmLowChanged)
    Q_PROPERTY(bool alarmActive READ alarmActive NOTIFY alarmActiveChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(bool isReadOnly READ isReadOnly WRITE setReadOnly NOTIFY readOnlyChanged)

public:
    explicit ProcessVariable(QObject *parent = nullptr);
    ProcessVariable(const QString& name, const QString& unit, 
                   double minVal, double maxVal, double defaultVal, 
                   QObject *parent = nullptr);
    
    // Getters
    QString name() const { QMutexLocker locker(&m_mutex); return m_name; }
    QString unit() const { QMutexLocker locker(&m_mutex); return m_unit; }
    QString description() const { QMutexLocker locker(&m_mutex); return m_description; }
    double value() const { QMutexLocker locker(&m_mutex); return m_value; }
    double minValue() const { QMutexLocker locker(&m_mutex); return m_minValue; }
    double maxValue() const { QMutexLocker locker(&m_mutex); return m_maxValue; }
    double alarmHigh() const { QMutexLocker locker(&m_mutex); return m_alarmHigh; }
    double alarmLow() const { QMutexLocker locker(&m_mutex); return m_alarmLow; }
    bool alarmActive() const { QMutexLocker locker(&m_mutex); return m_alarmActive; }
    bool isReadOnly() const { QMutexLocker locker(&m_mutex); return m_isReadOnly; }
    
    // Statistics
    double getAverage(int seconds = 300) const;
    double getMin(int seconds = 300) const;
    double getMax(int seconds = 300) const;
    double getStdDev(int seconds = 300) const;
    
    // Setters
    void setName(const QString& name);
    void setUnit(const QString& unit);
    void setDescription(const QString& desc);
    void setValue(double value);
    void setMinValue(double minVal);
    void setMaxValue(double maxVal);
    void setAlarmHigh(double high);
    void setAlarmLow(double low);
    void setReadOnly(bool readOnly);
    void resetAlarm();
    
    // Data history management
    struct DataPoint {
        QDateTime timestamp;
        double value;
        bool isAlarm;
    };
    
    QVector<DataPoint> getHistory(int seconds = 300) const;
    void clearHistory();
    void setHistorySize(int maxSize) { m_maxHistorySize = maxSize; }
    
    // Value manipulation
    void addToValue(double delta);
    void setValueClamped(double value);
    double getNormalizedValue() const;

signals:
    void nameChanged();
    void unitChanged();
    void descriptionChanged();
    void valueChanged();
    void minValueChanged();
    void maxValueChanged();
    void alarmHighChanged();
    void alarmLowChanged();
    void alarmActiveChanged();
    void readOnlyChanged();
    void alarmTriggered(const QString& tag, double value, double limit);
    void alarmCleared(const QString& tag, double value);

private:
    void checkAlarms();
    void addToHistory(const DataPoint& point);
    void updateStatistics() const;
    
    mutable QMutex m_mutex;
    
    QString m_name;
    QString m_unit;
    QString m_description;
    double m_value;
    double m_minValue;
    double m_maxValue;
    double m_alarmHigh;
    double m_alarmLow;
    bool m_alarmActive;
    bool m_isReadOnly;  // <-- This was missing!
    
    QVector<DataPoint> m_history;
    int m_maxHistorySize;
    
    // Statistics cache
    mutable double m_cachedAverage;
    mutable double m_cachedMin;
    mutable double m_cachedMax;
    mutable double m_cachedStdDev;
    mutable QDateTime m_statsUpdateTime;
    
    static const int DEFAULT_HISTORY_SIZE = 10000;
};
