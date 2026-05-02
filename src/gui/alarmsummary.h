#pragma once

#include <QTableWidget>
#include <QDateTime>
#include <QMap>
#include <QVariant>

class AlarmSummary : public QTableWidget
{
    Q_OBJECT

public:
    explicit AlarmSummary(QWidget *parent = nullptr);
    
    void addAlarm(const QString& tag, const QString& description, 
                  double value, double limit, const QString& priority);
    void acknowledgeAlarm(int row);
    void clearAlarms();
    void filterByPriority(const QString& priority);

signals:
    void alarmAcknowledged(const QString& tag);
    void alarmSelected(const QString& tag);

private slots:
    void onItemDoubleClicked(QTableWidgetItem* item);
    void onContextMenu(const QPoint& pos);

private:
    void setupTable();
    QColor getPriorityColor(const QString& priority);
    
    QMap<int, QVariantMap> m_alarmData;
};
