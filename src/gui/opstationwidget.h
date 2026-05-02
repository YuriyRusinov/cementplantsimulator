#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QSplitter>
#include <QTableWidget>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QDial>
#include <QSlider>
#include <QComboBox>
#include <QGroupBox>
#include <QStackedWidget>
#include <QTimer>
#include <QTreeWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QToolBar>
#include <QStatusBar>
#include "processmodel.h"

class TrendViewer;
class AlarmSummary;
class ProcessMimic;
class FaceplateDialog;

class OpStationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OpStationWidget(QSharedPointer<ProcessModel> model, QWidget *parent = nullptr);
    ~OpStationWidget();

    void setOperatorName(const QString& name);
    void setShiftInfo(const QString& shift, const QString& supervisor);
    void logOperation(const QString& action);
    
    QWidget* getControlPanel() { return m_controlPanel; }
    QStatusBar* getStatusBar() { return m_statusBar; }

signals:
    void emergencyStopTriggered();
    void viewChanged(const QString& view);
    void operatorAction(const QString& action, const QVariant& data);

public slots:
    void switchToOverview();
    void switchToKiln();
    void switchToRawMill();
    void switchToCementMill();
    void switchToPreheater();
    void switchToCooler();
    void switchToTrends();
    void switchToAlarms();
    void switchToReports();
    
    void onEmergencyStop();
    void onAcknowledgeAlarm();
    void onSaveScreenshot();

private slots:
    void updateDisplay();
    void updateAlarmSummary();
    void updateStatusBar();
    void addAlarm(const QString& tag, double value, double limit);
    void showFaceplate(const QString& tag);

private:
    void setupUI();
    void createToolBar();
    void createStatusBar();
    void createNavigationPanel();
    void createMainDisplayArea();
    void createControlPanel();
    void createTrendPanel();
    void createAlarmPanel();
    void updateNavigationHighlights();
    
    QSharedPointer<ProcessModel> m_processModel;
    
    // Main layout components
    QVBoxLayout* m_mainLayout;
    QSplitter* m_mainSplitter;
    
    // UI Components
    QTreeWidget* m_navigationTree;
    QWidget* m_controlPanel;
    QStatusBar* m_statusBar;
    QToolBar* m_toolBar;
    QLabel* m_statusLabel;
    QLabel* m_timeLabel;
    QLabel* m_userLabel;
    QLabel* m_shiftLabel;
    
    QStackedWidget* m_mainStack;
    
    ProcessMimic* m_overviewMimic;
    ProcessMimic* m_kilnMimic;
    ProcessMimic* m_rawMillMimic;
    ProcessMimic* m_cementMillMimic;
    ProcessMimic* m_preheaterMimic;
    ProcessMimic* m_coolerMimic;
    
    TrendViewer* m_trendViewer;
    AlarmSummary* m_alarmSummary;
    FaceplateDialog* m_faceplate;
    
    QTimer* m_updateTimer;
    QTimer* m_timeTimer;
    
    QString m_operatorName;
    QString m_shiftInfo;
    QString m_supervisorName;
    QList<QString> m_operationLog;
    
    QTabWidget* m_bottomTabWidget;
};
