#include "opstationwidget.h"
#include "mimicwidget.h"
#include "trendviewer.h"
#include "alarmsummary.h"
#include "faceplatedialog.h"
#include <QDateTime>
#include <QMessageBox>
#include <QFileDialog>
#include <QPixmap>
#include <QSplitter>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QDir>

OpStationWidget::OpStationWidget(QSharedPointer<ProcessModel> model, QWidget *parent)
    : QWidget(parent)
    , m_processModel(model)
    , m_mainLayout(nullptr)
    , m_mainSplitter(nullptr)
    , m_navigationTree(nullptr)
    , m_controlPanel(nullptr)
    , m_statusBar(nullptr)
    , m_toolBar(nullptr)
    , m_statusLabel(nullptr)
    , m_timeLabel(nullptr)
    , m_userLabel(nullptr)
    , m_shiftLabel(nullptr)
    , m_mainStack(nullptr)
    , m_overviewMimic(nullptr)
    , m_kilnMimic(nullptr)
    , m_rawMillMimic(nullptr)
    , m_cementMillMimic(nullptr)
    , m_preheaterMimic(nullptr)
    , m_coolerMimic(nullptr)
    , m_trendViewer(nullptr)
    , m_alarmSummary(nullptr)
    , m_faceplate(nullptr)
    , m_updateTimer(nullptr)
    , m_timeTimer(nullptr)
    , m_bottomTabWidget(nullptr)
{
    // Create all widgets first
    createToolBar();
    createStatusBar();
    createNavigationPanel();
    createMainDisplayArea();
    createControlPanel();
    createTrendPanel();
    createAlarmPanel();
    
    // Then setup the UI layout
    setupUI();
    
    // Setup timers and connections
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &OpStationWidget::updateDisplay);
    m_updateTimer->start(500);
    
    m_timeTimer = new QTimer(this);
    connect(m_timeTimer, &QTimer::timeout, this, &OpStationWidget::updateStatusBar);
    m_timeTimer->start(1000);
    
    connect(m_processModel.data(), &ProcessModel::processEvent, 
            this, &OpStationWidget::logOperation);
    
    // Start with overview
    switchToOverview();
}

OpStationWidget::~OpStationWidget()
{
    if (m_faceplate) {
        delete m_faceplate;
    }
}

void OpStationWidget::setupUI()
{
    setStyleSheet(R"(
        QWidget {
            background-color: #2b2b2b;
            color: #ffffff;
        }
        QToolBar {
            background-color: #3a3a3a;
            spacing: 3px;
            border: none;
        }
        QToolBar QLabel {
            color: #ffffff;
        }
        QStatusBar {
            background-color: #2b2b2b;
            color: #00ff00;
        }
        QTabWidget::pane {
            border: 1px solid #555;
            background-color: #2b2b2b;
        }
        QTabBar::tab {
            background-color: #3a3a3a;
            color: #ccc;
            padding: 8px 15px;
        }
        QTabBar::tab:selected {
            background-color: #4a4a4a;
        }
        QGroupBox {
            font-weight: bold;
            border: 2px solid #555;
            border-radius: 5px;
            margin-top: 1ex;
            padding-top: 10px;
            color: #ddd;
        }
        QPushButton {
            background-color: #4a4a4a;
            border: 1px solid #666;
            border-radius: 3px;
            padding: 5px;
            color: white;
        }
        QPushButton:hover {
            background-color: #5a5a5a;
        }
        QPushButton:pressed {
            background-color: #3a3a3a;
        }
        QLineEdit, QDoubleSpinBox, QComboBox {
            background-color: #3a3a3a;
            color: white;
            border: 1px solid #666;
            padding: 3px;
        }
        QTableWidget {
            background-color: #2b2b2b;
            color: #ccc;
            alternate-background-color: #333;
            gridline-color: #555;
        }
        QHeaderView::section {
            background-color: #3a3a3a;
            color: white;
            padding: 5px;
        }
        QSplitter::handle {
            background-color: #555;
        }
        QTreeWidget {
            background-color: #2b2b2b;
            color: #ccc;
            border: none;
        }
        QTreeWidget::item {
            padding: 5px;
        }
        QTreeWidget::item:selected {
            background-color: #4a4a4a;
        }
        QSlider::groove:horizontal {
            height: 6px;
            background: #555;
            border-radius: 3px;
        }
        QSlider::handle:horizontal {
            background: #888;
            width: 14px;
            margin: -4px 0;
            border-radius: 7px;
        }
        QSlider::sub-page:horizontal {
            background: #4a90e2;
            border-radius: 3px;
        }
    )");
    
    // Create main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    
    // Add toolbar at top
    if (m_toolBar) {
        m_mainLayout->addWidget(m_toolBar);
    }
    
    // Add splitter in the middle (already populated with all panels)
    if (m_mainSplitter) {
        m_mainLayout->addWidget(m_mainSplitter, 1);
    }
    
    // Add status bar at bottom
    if (m_statusBar) {
        m_mainLayout->addWidget(m_statusBar);
    }
}

void OpStationWidget::createToolBar()
{
    m_toolBar = new QToolBar(this);
    m_toolBar->setMovable(false);
    m_toolBar->setIconSize(QSize(24, 24));
    
    // Navigation buttons
    QPushButton* overviewBtn = new QPushButton("Overview");
    QPushButton* kilnBtn = new QPushButton("Kiln");
    QPushButton* rawMillBtn = new QPushButton("Raw Mill");
    QPushButton* cementMillBtn = new QPushButton("Cement Mill");
    QPushButton* preheaterBtn = new QPushButton("Preheater");
    QPushButton* coolerBtn = new QPushButton("Cooler");
    
    connect(overviewBtn, &QPushButton::clicked, this, &OpStationWidget::switchToOverview);
    connect(kilnBtn, &QPushButton::clicked, this, &OpStationWidget::switchToKiln);
    connect(rawMillBtn, &QPushButton::clicked, this, &OpStationWidget::switchToRawMill);
    connect(cementMillBtn, &QPushButton::clicked, this, &OpStationWidget::switchToCementMill);
    connect(preheaterBtn, &QPushButton::clicked, this, &OpStationWidget::switchToPreheater);
    connect(coolerBtn, &QPushButton::clicked, this, &OpStationWidget::switchToCooler);
    
    m_toolBar->addWidget(overviewBtn);
    m_toolBar->addWidget(kilnBtn);
    m_toolBar->addWidget(rawMillBtn);
    m_toolBar->addWidget(cementMillBtn);
    m_toolBar->addWidget(preheaterBtn);
    m_toolBar->addWidget(coolerBtn);
    
    m_toolBar->addSeparator();
    
    // Control buttons
    QPushButton* emergencyBtn = new QPushButton("EMERGENCY STOP");
    emergencyBtn->setStyleSheet("background-color: red; color: white; font-weight: bold;");
    connect(emergencyBtn, &QPushButton::clicked, this, &OpStationWidget::onEmergencyStop);
    m_toolBar->addWidget(emergencyBtn);
    
    QPushButton* ackAlarmBtn = new QPushButton("Acknowledge Alarms");
    connect(ackAlarmBtn, &QPushButton::clicked, this, &OpStationWidget::onAcknowledgeAlarm);
    m_toolBar->addWidget(ackAlarmBtn);
    
    QPushButton* screenshotBtn = new QPushButton("Take Screenshot");
    connect(screenshotBtn, &QPushButton::clicked, this, &OpStationWidget::onSaveScreenshot);
    m_toolBar->addWidget(screenshotBtn);
    
    m_toolBar->addSeparator();
    
    // Speed control
    QLabel* speedLabel = new QLabel("Sim Speed:");
    m_toolBar->addWidget(speedLabel);
    
    QSlider* speedSlider = new QSlider(Qt::Horizontal);
    speedSlider->setRange(1, 100);
    speedSlider->setValue(10);
    speedSlider->setMaximumWidth(150);
    connect(speedSlider, &QSlider::valueChanged, [this](int val) {
        m_processModel->setSimulationSpeed(val / 10.0);
        emit operatorAction("SpeedChanged", val / 10.0);
    });
    m_toolBar->addWidget(speedSlider);
    
    QLabel* speedValue = new QLabel("1.0x");
    connect(speedSlider, &QSlider::valueChanged, [speedValue](int val) {
        speedValue->setText(QString("%1x").arg(val / 10.0, 0, 'f', 1));
    });
    m_toolBar->addWidget(speedValue);
    
    m_toolBar->addSeparator();
    
    // View control
    QPushButton* trendsBtn = new QPushButton("Show Trends");
    QPushButton* alarmsBtn = new QPushButton("Show Alarms");
    
    connect(trendsBtn, &QPushButton::clicked, [this]() {
        if (m_bottomTabWidget) {
            m_bottomTabWidget->setCurrentIndex(0);
        }
        emit operatorAction("ShowTrends", QVariant());
    });
    
    connect(alarmsBtn, &QPushButton::clicked, [this]() {
        if (m_bottomTabWidget) {
            m_bottomTabWidget->setCurrentIndex(1);
        }
        emit operatorAction("ShowAlarms", QVariant());
    });
    
    m_toolBar->addWidget(trendsBtn);
    m_toolBar->addWidget(alarmsBtn);
    
    // Add spacer to push remaining items to the right
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_toolBar->addWidget(spacer);
    
    // User info display
    m_userLabel = new QLabel("Operator: Not Logged In");
    m_toolBar->addWidget(m_userLabel);
}

void OpStationWidget::createStatusBar()
{
    m_statusBar = new QStatusBar(this);
    m_statusBar->setSizeGripEnabled(false);
    
    m_statusLabel = new QLabel("System Ready");
    m_timeLabel = new QLabel();
    m_shiftLabel = new QLabel();
    
    m_statusBar->addWidget(m_statusLabel, 1);
    m_statusBar->addWidget(m_shiftLabel);
    m_statusBar->addWidget(m_timeLabel);
}

void OpStationWidget::createNavigationPanel()
{
    m_navigationTree = new QTreeWidget();
    m_navigationTree->setHeaderLabel("Plant Areas");
    m_navigationTree->setMaximumWidth(250);
    m_navigationTree->setMinimumWidth(200);
    
    // Build navigation tree
    QTreeWidgetItem* overviewItem = new QTreeWidgetItem(QStringList() << "Plant Overview");
    
    QTreeWidgetItem* pyroItem = new QTreeWidgetItem(QStringList() << "Pyroprocessing");
    QTreeWidgetItem* grindingItem = new QTreeWidgetItem(QStringList() << "Grinding");
    QTreeWidgetItem* utilitiesItem = new QTreeWidgetItem(QStringList() << "Utilities");
    
    QTreeWidgetItem* preheaterItem = new QTreeWidgetItem(QStringList() << "Preheater");
    QTreeWidgetItem* calcinerItem = new QTreeWidgetItem(QStringList() << "Calciner");
    QTreeWidgetItem* kilnItem = new QTreeWidgetItem(QStringList() << "Kiln");
    QTreeWidgetItem* coolerItem = new QTreeWidgetItem(QStringList() << "Cooler");
    
    pyroItem->addChildren({preheaterItem, calcinerItem, kilnItem, coolerItem});
    
    QTreeWidgetItem* rawMillItem = new QTreeWidgetItem(QStringList() << "Vertical Raw Mill");
    QTreeWidgetItem* cementMillItem = new QTreeWidgetItem(QStringList() << "Cement Mill");
    QTreeWidgetItem* coalMillItem = new QTreeWidgetItem(QStringList() << "Coal Mill");
    
    grindingItem->addChildren({rawMillItem, cementMillItem, coalMillItem});
    
    QTreeWidgetItem* dustItem = new QTreeWidgetItem(QStringList() << "Dust Collectors");
    QTreeWidgetItem* compressorsItem = new QTreeWidgetItem(QStringList() << "Compressors");
    QTreeWidgetItem* pumpsItem = new QTreeWidgetItem(QStringList() << "Pumps");
    
    utilitiesItem->addChildren({dustItem, compressorsItem, pumpsItem});
    
    m_navigationTree->addTopLevelItems({overviewItem, pyroItem, grindingItem, utilitiesItem});
    m_navigationTree->expandAll();
    
    connect(m_navigationTree, &QTreeWidget::itemClicked, 
            [this](QTreeWidgetItem* item, int) {
        QString area = item->text(0);
        if (area == "Plant Overview") switchToOverview();
        else if (area == "Kiln") switchToKiln();
        else if (area == "Vertical Raw Mill") switchToRawMill();
        else if (area == "Cement Mill") switchToCementMill();
        else if (area == "Preheater") switchToPreheater();
        else if (area == "Cooler") switchToCooler();
        emit viewChanged(area);
    });
}

void OpStationWidget::createMainDisplayArea()
{
    m_mainStack = new QStackedWidget();
    
    // Create mimic displays for each area
    m_overviewMimic = new ProcessMimic(m_processModel, "overview", this);
    m_kilnMimic = new ProcessMimic(m_processModel, "kiln", this);
    m_rawMillMimic = new ProcessMimic(m_processModel, "rawmill", this);
    m_cementMillMimic = new ProcessMimic(m_processModel, "cementmill", this);
    m_preheaterMimic = new ProcessMimic(m_processModel, "preheater", this);
    m_coolerMimic = new ProcessMimic(m_processModel, "cooler", this);
    
    m_mainStack->addWidget(m_overviewMimic);
    m_mainStack->addWidget(m_kilnMimic);
    m_mainStack->addWidget(m_rawMillMimic);
    m_mainStack->addWidget(m_cementMillMimic);
    m_mainStack->addWidget(m_preheaterMimic);
    m_mainStack->addWidget(m_coolerMimic);
}

void OpStationWidget::createControlPanel()
{
    m_controlPanel = new QWidget();
    m_controlPanel->setMaximumWidth(350);
    m_controlPanel->setMinimumWidth(280);
    
    QVBoxLayout* layout = new QVBoxLayout(m_controlPanel);
    layout->setSpacing(10);
    
    // Production targets
    QGroupBox* targetGroup = new QGroupBox("Production Targets");
    QGridLayout* targetLayout = new QGridLayout();
    
    targetLayout->addWidget(new QLabel("Production Rate:"), 0, 0);
    QDoubleSpinBox* prodRate = new QDoubleSpinBox();
    prodRate->setRange(0, 500);
    prodRate->setValue(250);
    prodRate->setSuffix(" t/h");
    connect(prodRate, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double val) {
                auto var = m_processModel->getVariable("kiln_feed");
                if (var) var->setValue(val);
            });
    targetLayout->addWidget(prodRate, 0, 1);
    
    targetLayout->addWidget(new QLabel("Quality Target:"), 1, 0);
    QDoubleSpinBox* qualityTarget = new QDoubleSpinBox();
    qualityTarget->setRange(2800, 4200);
    qualityTarget->setValue(3200);
    qualityTarget->setSuffix(" cm²/g");
    targetLayout->addWidget(qualityTarget, 1, 1);
    
    targetGroup->setLayout(targetLayout);
    layout->addWidget(targetGroup);
    
    // Kiln control
    QGroupBox* kilnGroup = new QGroupBox("Kiln Control");
    QFormLayout* kilnLayout = new QFormLayout();
    
    QDoubleSpinBox* fuelSpin = new QDoubleSpinBox();
    fuelSpin->setRange(0, 15);
    fuelSpin->setValue(7.5);
    fuelSpin->setSuffix(" t/h");
    connect(fuelSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double val) {
                auto var = m_processModel->getVariable("burner_fuel");
                if (var) var->setValue(val);
            });
    kilnLayout->addRow("Fuel Flow:", fuelSpin);
    
    QSlider* speedSlider = new QSlider(Qt::Horizontal);
    speedSlider->setRange(0, 500);
    speedSlider->setValue(250);
    connect(speedSlider, &QSlider::valueChanged, [this](int val) {
        auto var = m_processModel->getVariable("kiln_speed");
        if (var) var->setValue(val / 100.0);
    });
    kilnLayout->addRow("Kiln Speed:", speedSlider);
    
    kilnGroup->setLayout(kilnLayout);
    layout->addWidget(kilnGroup);
    
    // Cooler control
    QGroupBox* coolerGroup = new QGroupBox("Cooler Control");
    QFormLayout* coolerLayout = new QFormLayout();
    
    QSlider* fan1Slider = new QSlider(Qt::Horizontal);
    fan1Slider->setRange(0, 100);
    fan1Slider->setValue(60);
    connect(fan1Slider, &QSlider::valueChanged, [this](int val) {
        auto var = m_processModel->getVariable("cooler_fan1");
        if (var) var->setValue(val);
    });
    coolerLayout->addRow("Fan 1:", fan1Slider);
    
    QSlider* fan2Slider = new QSlider(Qt::Horizontal);
    fan2Slider->setRange(0, 100);
    fan2Slider->setValue(60);
    connect(fan2Slider, &QSlider::valueChanged, [this](int val) {
        auto var = m_processModel->getVariable("cooler_fan2");
        if (var) var->setValue(val);
    });
    coolerLayout->addRow("Fan 2:", fan2Slider);
    
    coolerGroup->setLayout(coolerLayout);
    layout->addWidget(coolerGroup);
    
    // Quick actions
    QGroupBox* actionGroup = new QGroupBox("Quick Actions");
    QVBoxLayout* actionLayout = new QVBoxLayout();
    
    QPushButton* startupSeq = new QPushButton("Startup Sequence");
    QPushButton* shutdownSeq = new QPushButton("Shutdown Sequence");
    QPushButton* resetBtn = new QPushButton("Reset to Default");
    
    connect(startupSeq, &QPushButton::clicked, [this]() {
        m_processModel->resetToDefault();
        logOperation("Startup sequence initiated");
    });
    
    connect(shutdownSeq, &QPushButton::clicked, [this]() {
        onEmergencyStop();
        logOperation("Shutdown sequence initiated");
    });
    
    connect(resetBtn, &QPushButton::clicked, [this]() {
        m_processModel->resetToDefault();
        logOperation("Plant reset to default state");
    });
    
    actionLayout->addWidget(startupSeq);
    actionLayout->addWidget(shutdownSeq);
    actionLayout->addWidget(resetBtn);
    
    actionGroup->setLayout(actionLayout);
    layout->addWidget(actionGroup);
    
    layout->addStretch();
}

void OpStationWidget::createTrendPanel()
{
    m_trendViewer = new TrendViewer(m_processModel, this);
}

void OpStationWidget::createAlarmPanel()
{
    m_alarmSummary = new AlarmSummary(this);
    connect(m_alarmSummary, &AlarmSummary::alarmSelected, 
            [this](const QString& tag) { showFaceplate(tag); });
}

void OpStationWidget::switchToOverview()
{
    if (m_mainStack && m_overviewMimic) {
        m_mainStack->setCurrentWidget(m_overviewMimic);
        m_statusLabel->setText("View: Plant Overview");
        updateNavigationHighlights();
        emit viewChanged("Overview");
    }
}

void OpStationWidget::switchToKiln()
{
    if (m_mainStack && m_kilnMimic) {
        m_mainStack->setCurrentWidget(m_kilnMimic);
        m_statusLabel->setText("View: Kiln Section");
        updateNavigationHighlights();
        emit viewChanged("Kiln");
    }
}

void OpStationWidget::switchToRawMill()
{
    if (m_mainStack && m_rawMillMimic) {
        m_mainStack->setCurrentWidget(m_rawMillMimic);
        m_statusLabel->setText("View: Raw Mill Section");
        updateNavigationHighlights();
        emit viewChanged("RawMill");
    }
}

void OpStationWidget::switchToCementMill()
{
    if (m_mainStack && m_cementMillMimic) {
        m_mainStack->setCurrentWidget(m_cementMillMimic);
        m_statusLabel->setText("View: Cement Mill Section");
        updateNavigationHighlights();
        emit viewChanged("CementMill");
    }
}

void OpStationWidget::switchToPreheater()
{
    if (m_mainStack && m_preheaterMimic) {
        m_mainStack->setCurrentWidget(m_preheaterMimic);
        m_statusLabel->setText("View: Preheater Section");
        updateNavigationHighlights();
        emit viewChanged("Preheater");
    }
}

void OpStationWidget::switchToCooler()
{
    if (m_mainStack && m_coolerMimic) {
        m_mainStack->setCurrentWidget(m_coolerMimic);
        m_statusLabel->setText("View: Cooler Section");
        updateNavigationHighlights();
        emit viewChanged("Cooler");
    }
}

void OpStationWidget::switchToTrends()
{
    if (m_bottomTabWidget) {
        m_bottomTabWidget->setCurrentIndex(0);
        emit operatorAction("ShowTrends", QVariant());
    }
}

void OpStationWidget::switchToAlarms()
{
    if (m_bottomTabWidget) {
        m_bottomTabWidget->setCurrentIndex(1);
        emit operatorAction("ShowAlarms", QVariant());
    }
}

void OpStationWidget::switchToReports()
{
    m_statusLabel->setText("Reports - Coming Soon");
    QMessageBox::information(this, "Reports", 
        "Reports module is under development.\n"
        "Check back in the next release for:\n"
        "- Shift reports\n"
        "- Production summaries\n"
        "- Quality analysis\n"
        "- Alarm history");
}

void OpStationWidget::onEmergencyStop()
{
    QMessageBox::StandardButton reply = QMessageBox::critical(
        this, 
        "EMERGENCY STOP", 
        "WARNING: Emergency stop will shutdown all process equipment!\n\n"
        "This will:\n"
        "- Stop all fuel flow\n"
        "- Stop kiln feed\n"
        "- Stop kiln rotation\n"
        "- Stop mills\n\n"
        "Are you sure you want to proceed?",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        m_processModel->setSimulationSpeed(0);
        
        auto burnerFuel = m_processModel->getVariable("burner_fuel");
        if (burnerFuel) burnerFuel->setValue(0);
        
        auto kilnFeed = m_processModel->getVariable("kiln_feed");
        if (kilnFeed) kilnFeed->setValue(0);
        
        auto kilnSpeed = m_processModel->getVariable("kiln_speed");
        if (kilnSpeed) kilnSpeed->setValue(0);
        
        auto rawMillFeed = m_processModel->getVariable("raw_mill_feed");
        if (rawMillFeed) rawMillFeed->setValue(0);
        
        auto cementMillFeed = m_processModel->getVariable("cement_mill_feed");
        if (cementMillFeed) cementMillFeed->setValue(0);
        
        logOperation("EMERGENCY STOP activated by operator");
        m_statusLabel->setText("EMERGENCY STOP ACTIVE");
        m_statusLabel->setStyleSheet("color: red; font-weight: bold;");
        emit emergencyStopTriggered();
        emit operatorAction("EmergencyStop", QVariant());
    }
}

void OpStationWidget::onAcknowledgeAlarm()
{
    int alarmCount = 0;
    for (auto& var : m_processModel->getAllVariables()) {
        if (var->alarmActive()) {
            var->resetAlarm();
            alarmCount++;
        }
    }
    updateAlarmSummary();
    logOperation(QString("Acknowledged %1 alarms").arg(alarmCount));
    m_statusLabel->setText(QString("%1 alarms acknowledged").arg(alarmCount));
    
    if (alarmCount == 0) {
        m_statusLabel->setText("No active alarms");
        m_statusLabel->setStyleSheet("color: #00ff00;");
    }
}

void OpStationWidget::onSaveScreenshot()
{
    QPixmap pixmap = this->grab();
    QString filename = QFileDialog::getSaveFileName(this, "Save Screenshot", 
                        QDir::homePath() + QString("/screenshot_%1.png").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")),
                        "PNG Files (*.png)");
    if (!filename.isEmpty()) {
        if (pixmap.save(filename)) {
            logOperation(QString("Screenshot saved to %1").arg(filename));
            m_statusLabel->setText("Screenshot saved successfully");
        } else {
            m_statusLabel->setText("Failed to save screenshot");
        }
    }
}

void OpStationWidget::updateDisplay()
{
    // Update all mimic displays
    if (m_overviewMimic) m_overviewMimic->updateDisplay();
    if (m_kilnMimic) m_kilnMimic->updateDisplay();
    if (m_rawMillMimic) m_rawMillMimic->updateDisplay();
    if (m_cementMillMimic) m_cementMillMimic->updateDisplay();
    if (m_preheaterMimic) m_preheaterMimic->updateDisplay();
    if (m_coolerMimic) m_coolerMimic->updateDisplay();
    
    // Update trend viewer
    if (m_trendViewer) m_trendViewer->updateTrends();
    
    // Update alarm summary
    updateAlarmSummary();
    
    // Update faceplate if open
    if (m_faceplate && m_faceplate->isVisible()) {
        m_faceplate->updateDisplay();
    }
}

void OpStationWidget::updateAlarmSummary()
{
    if (!m_alarmSummary) return;
    
    m_alarmSummary->clearAlarms();
    
    // Get all variables safely
    QMap<QString, QSharedPointer<ProcessVariable>> variables = m_processModel->getAllVariables();
    
    // Collect alarm data in a separate structure
    struct AlarmInfo {
        QString tag;
        QString name;
        double value;
        double limit;
        QString priority;
    };
    QList<AlarmInfo> activeAlarms;
    
    // Safely collect alarm information
    for (auto it = variables.constBegin(); it != variables.constEnd(); ++it) {
        const QSharedPointer<ProcessVariable>& var = it.value();
        
        // Skip null pointers
        if (var.isNull()) continue;
        
        // Check if alarm is active (thread-safe value access)
        bool isAlarmActive = false;
        double currentValue = 0;
        double highLimit = 0;
        double lowLimit = 0;
        
        // Each ProcessVariable has its own mutex for thread safety
        if (var->alarmActive()) {
            isAlarmActive = true;
            currentValue = var->value();
            highLimit = var->alarmHigh();
            lowLimit = var->alarmLow();
        }
        
        if (isAlarmActive) {
            AlarmInfo info;
            info.tag = it.key();
            info.name = var->name();
            info.value = currentValue;
            info.priority = "HIGH";
            
            if (currentValue > highLimit) {
                info.priority = "CRITICAL";
                info.limit = highLimit;
            } else if (currentValue < lowLimit) {
                info.priority = "LOW";
                info.limit = lowLimit;
            } else {
                info.limit = highLimit;
            }
            
            activeAlarms.append(info);
        }
    }
    
    // Add all collected alarms to the summary widget
    for (const AlarmInfo& info : activeAlarms) {
        m_alarmSummary->addAlarm(info.tag, info.name, info.value, info.limit, info.priority);
    }
    
    // Update status bar
    if (m_statusLabel) {
        bool isEmergencyStop = (m_statusLabel->text() == "EMERGENCY STOP ACTIVE");
        
        if (activeAlarms.size() > 0 && !isEmergencyStop) {
            m_statusLabel->setText(QString("%1 active alarm(s)").arg(activeAlarms.size()));
            m_statusLabel->setStyleSheet("color: orange; font-weight: bold;");
        } else if (activeAlarms.size() == 0 && !isEmergencyStop) {
            m_statusLabel->setText("System Ready");
            m_statusLabel->setStyleSheet("color: #00ff00;");
        }
    }
}

void OpStationWidget::addAlarm(const QString& tag, double value, double limit)
{
    if (m_alarmSummary) {
        m_alarmSummary->addAlarm(tag, tag, value, limit, "HIGH");
        m_statusLabel->setText(QString("ALARM: %1 = %2").arg(tag).arg(value));
        emit operatorAction("AlarmTriggered", tag);
    }
}

void OpStationWidget::showFaceplate(const QString& tag)
{
    if (!m_faceplate) {
        m_faceplate = new FaceplateDialog(m_processModel, tag, this);
    } else {
        m_faceplate->setTag(tag);
    }
    m_faceplate->show();
    m_faceplate->raise();
    m_faceplate->activateWindow();
    emit operatorAction("ShowFaceplate", tag);
}

void OpStationWidget::updateStatusBar()
{
    if (m_timeLabel) {
        m_timeLabel->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    }
    if (m_shiftLabel) {
        m_shiftLabel->setText(m_shiftInfo);
    }
    if (m_userLabel) {
        m_userLabel->setText(QString("Operator: %1").arg(
            m_operatorName.isEmpty() ? "Not Logged In" : m_operatorName));
    }
}

void OpStationWidget::updateNavigationHighlights()
{
    // This would highlight the current view in the navigation tree
    // For now, just update status
}

void OpStationWidget::setOperatorName(const QString& name)
{
    m_operatorName = name;
    updateStatusBar();
    logOperation(QString("Operator %1 logged in").arg(name));
}

void OpStationWidget::setShiftInfo(const QString& shift, const QString& supervisor)
{
    m_shiftInfo = QString("Shift: %1 | Supervisor: %2").arg(shift, supervisor);
    m_supervisorName = supervisor;
    updateStatusBar();
    logOperation(QString("Shift changed to %1, Supervisor: %2").arg(shift, supervisor));
}

void OpStationWidget::logOperation(const QString& action)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString logEntry = QString("[%1] %2").arg(timestamp, action);
    m_operationLog.append(logEntry);
    
    // Keep only last 1000 operations
    while (m_operationLog.size() > 1000) {
        m_operationLog.removeFirst();
    }
    
    // Update status label (but don't override alarm/emergency messages)
    if (!m_statusLabel->text().contains("ALARM") && 
        m_statusLabel->text() != "EMERGENCY STOP ACTIVE") {
        m_statusLabel->setText(action);
    }
    
    emit operatorAction("Log", action);
}
