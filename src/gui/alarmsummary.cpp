#include "alarmsummary.h"
#include <QHeaderView>
#include <QMenu>
#include <QContextMenuEvent>

AlarmSummary::AlarmSummary(QWidget *parent)
    : QTableWidget(parent)
{
    setupTable();
}

void AlarmSummary::setupTable()
{
    setColumnCount(5);
    setHorizontalHeaderLabels({"Time", "Tag", "Description", "Value", "Priority"});
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setAlternatingRowColors(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTableWidget::customContextMenuRequested, 
            this, &AlarmSummary::onContextMenu);
    connect(this, &QTableWidget::itemDoubleClicked,
            this, &AlarmSummary::onItemDoubleClicked);
}

void AlarmSummary::addAlarm(const QString& tag, const QString& description, 
                            double value, double limit, const QString& priority)
{
    // Check if alarm already exists
    for (int i = 0; i < rowCount(); i++) {
        if (item(i, 1)->text() == tag) {
            return; // Alarm already active
        }
    }
    
    int row = rowCount();
    insertRow(row);
    
    QDateTime now = QDateTime::currentDateTime();
    
    QTableWidgetItem* timeItem = new QTableWidgetItem(now.toString("hh:mm:ss"));
    timeItem->setData(Qt::UserRole, now);
    setItem(row, 0, timeItem);
    
    setItem(row, 1, new QTableWidgetItem(tag));
    setItem(row, 2, new QTableWidgetItem(description));
    setItem(row, 3, new QTableWidgetItem(QString::number(value, 'f', 2)));
    setItem(row, 4, new QTableWidgetItem(priority));
    
    // Color based on priority
    QColor bgColor = getPriorityColor(priority);
    for (int col = 0; col < 5; col++) {
        QTableWidgetItem* item = this->item(row, col);
        if (item) {
            item->setBackground(bgColor);
            if (priority == "CRITICAL") {
                item->setForeground(QColor(255, 255, 255));
                QFont font = item->font();
                font.setBold(true);
                item->setFont(font);
            }
        }
    }
    
    // Scroll to show new alarm
    scrollToBottom();
}

void AlarmSummary::acknowledgeAlarm(int row)
{
    if (row >= 0 && row < rowCount()) {
        QString tag = item(row, 1)->text();
        removeRow(row);
        emit alarmAcknowledged(tag);
    }
}

void AlarmSummary::clearAlarms()
{
    setRowCount(0);
    m_alarmData.clear();
}

void AlarmSummary::filterByPriority(const QString& priority)
{
    for (int i = 0; i < rowCount(); i++) {
        bool visible = (priority.isEmpty() || item(i, 4)->text() == priority);
        setRowHidden(i, !visible);
    }
}

QColor AlarmSummary::getPriorityColor(const QString& priority)
{
    if (priority == "CRITICAL") {
        return QColor(200, 50, 50);
    } else if (priority == "HIGH") {
        return QColor(200, 150, 50);
    } else if (priority == "MEDIUM") {
        return QColor(200, 200, 50);
    } else {
        return QColor(100, 100, 100);
    }
}

void AlarmSummary::onItemDoubleClicked(QTableWidgetItem* item)
{
    if (item) {
        int row = item->row();
        QString tag = this->item(row, 1)->text();
        emit alarmSelected(tag);
    }
}

void AlarmSummary::onContextMenu(const QPoint& pos)
{
    QModelIndex index = indexAt(pos);
    if (!index.isValid()) return;
    
    QMenu menu(this);
    
    QAction* acknowledgeAction = menu.addAction("Acknowledge");
    QAction* acknowledgeAllAction = menu.addAction("Acknowledge All");
    menu.addSeparator();
    QAction* detailsAction = menu.addAction("Show Details");
    
    QAction* selectedAction = menu.exec(viewport()->mapToGlobal(pos));
    
    if (selectedAction == acknowledgeAction) {
        acknowledgeAlarm(index.row());
    } else if (selectedAction == acknowledgeAllAction) {
        clearAlarms();
    } else if (selectedAction == detailsAction) {
        QString tag = this->item(index.row(), 1)->text();
        emit alarmSelected(tag);
    }
}
