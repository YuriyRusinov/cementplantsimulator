#include "faceplatedialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QComboBox>
#include <QProgressBar>
#include <QTimer>
#include <QMessageBox>

FaceplateDialog::FaceplateDialog(QSharedPointer<ProcessModel> model, const QString& tag, QWidget *parent)
    : QDialog(parent)
    , m_processModel(model)
    , m_tag(tag)
{
    setupUI();
    
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &FaceplateDialog::updateDisplay);
    m_updateTimer->start(200);
    
    setWindowTitle(QString("Faceplate - %1").arg(tag));
    setModal(false);
    resize(400, 500);
}

void FaceplateDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Tag info group
    QGroupBox* infoGroup = new QGroupBox("Tag Information");
    QFormLayout* infoLayout = new QFormLayout();
    
    m_tagLabel = new QLabel();
    m_descriptionLabel = new QLabel();
    m_valueLabel = new QLabel();
    m_unitsLabel = new QLabel();
    m_statusLabel = new QLabel();
    
    infoLayout->addRow("Tag:", m_tagLabel);
    infoLayout->addRow("Description:", m_descriptionLabel);
    infoLayout->addRow("Value:", m_valueLabel);
    infoLayout->addRow("Units:", m_unitsLabel);
    infoLayout->addRow("Status:", m_statusLabel);
    
    infoGroup->setLayout(infoLayout);
    mainLayout->addWidget(infoGroup);
    
    // Value display group
    QGroupBox* valueGroup = new QGroupBox("Process Value");
    QVBoxLayout* valueLayout = new QVBoxLayout();
    
    m_valueBar = new QProgressBar();
    m_valueBar->setRange(0, 100);
    m_valueBar->setTextVisible(true);
    valueLayout->addWidget(m_valueBar);
    
    valueGroup->setLayout(valueLayout);
    mainLayout->addWidget(valueGroup);
    
    // Control group
    QGroupBox* controlGroup = new QGroupBox("Control");
    QVBoxLayout* controlLayout = new QVBoxLayout();
    
    QFormLayout* controlForm = new QFormLayout();
    
    m_setpointSpin = new QDoubleSpinBox();
    m_setpointSpin->setRange(0, 1000);
    m_setpointSpin->setDecimals(1);
    connect(m_setpointSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &FaceplateDialog::onSetpointChanged);
    controlForm->addRow("Setpoint:", m_setpointSpin);
    
    m_outputSlider = new QSlider(Qt::Horizontal);
    m_outputSlider->setRange(0, 100);
    connect(m_outputSlider, &QSlider::valueChanged,
            this, &FaceplateDialog::onOutputChanged);
    controlForm->addRow("Output:", m_outputSlider);
    
    m_modeCombo = new QComboBox();
    m_modeCombo->addItems({"Auto", "Manual", "Local"});
    connect(m_modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FaceplateDialog::onModeChanged);
    controlForm->addRow("Mode:", m_modeCombo);
    
    controlLayout->addLayout(controlForm);
    controlGroup->setLayout(controlLayout);
    mainLayout->addWidget(controlGroup);
    
    // Action buttons
    QGroupBox* actionGroup = new QGroupBox("Actions");
    QHBoxLayout* actionLayout = new QHBoxLayout();
    
    m_startStopButton = new QPushButton("Start");
    connect(m_startStopButton, &QPushButton::clicked, this, &FaceplateDialog::onStartStop);
    actionLayout->addWidget(m_startStopButton);
    
    mAcknowledgeButton = new QPushButton("Acknowledge");
    connect(mAcknowledgeButton, &QPushButton::clicked, this, &FaceplateDialog::onAcknowledge);
    actionLayout->addWidget(mAcknowledgeButton);
    
    QPushButton* closeButton = new QPushButton("Close");
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    actionLayout->addWidget(closeButton);
    
    actionGroup->setLayout(actionLayout);
    mainLayout->addWidget(actionGroup);
}

void FaceplateDialog::setTag(const QString& tag)
{
    m_tag = tag;
    setWindowTitle(QString("Faceplate - %1").arg(tag));
    updateTagInfo();
    updateDisplay();
}

void FaceplateDialog::updateTagInfo()
{
    auto var = m_processModel->getVariable(m_tag);
    if (var) {
        m_tagLabel->setText(m_tag);
        m_descriptionLabel->setText(var->description().isEmpty() ? m_tag : var->description());
        m_unitsLabel->setText(var->unit());
        
        // Set value range for controls
        m_setpointSpin->setRange(var->minValue(), var->maxValue());
        m_valueBar->setRange(static_cast<int>(var->minValue()), 
                            static_cast<int>(var->maxValue()));
    } else {
        m_tagLabel->setText(m_tag);
        m_descriptionLabel->setText("Tag not found");
        m_unitsLabel->setText("");
    }
}

void FaceplateDialog::updateDisplay()
{
    auto var = m_processModel->getVariable(m_tag);
    if (!var) return;
    
    double value = var->value();
    QString valueText = QString::number(value, 'f', 2);
    m_valueLabel->setText(valueText);
    
    // Update progress bar
    int percent = static_cast<int>((value - var->minValue()) / 
                                   (var->maxValue() - var->minValue()) * 100);
    m_valueBar->setValue(percent);
    m_valueBar->setFormat(QString("%1 %2").arg(valueText, var->unit()));
    
    // Update status
    if (var->alarmActive()) {
        if (value > var->alarmHigh()) {
            m_statusLabel->setText("HIGH ALARM");
            m_statusLabel->setStyleSheet("color: red; font-weight: bold;");
        } else if (value < var->alarmLow()) {
            m_statusLabel->setText("LOW ALARM");
            m_statusLabel->setStyleSheet("color: red; font-weight: bold;");
        }
    } else {
        m_statusLabel->setText("Normal");
        m_statusLabel->setStyleSheet("color: green;");
    }
    
    // Update start/stop button text based on tag type
    if (m_tag.contains("fan") || m_tag.contains("motor") || m_tag.contains("mill")) {
        if (value > 0) {
            m_startStopButton->setText("Stop");
            m_startStopButton->setStyleSheet("background-color: #8B0000;");
        } else {
            m_startStopButton->setText("Start");
            m_startStopButton->setStyleSheet("background-color: #006400;");
        }
    }
}

void FaceplateDialog::onSetpointChanged(double value)
{
    // For now, just log the setpoint change
    // In a real system, this would go to a PID controller
    QString msg = QString("Setpoint changed to %1 for %2").arg(value).arg(m_tag);
    m_processModel->processEvent(msg, QVariant());
}

void FaceplateDialog::onOutputChanged(int value)
{
    // For manual control output
    QString msg = QString("Output set to %1% for %2").arg(value).arg(m_tag);
    m_processModel->processEvent(msg, QVariant());
    
    // If in manual mode, directly control the variable
    if (m_modeCombo->currentText() == "Manual") {
        auto var = m_processModel->getVariable(m_tag);
        if (var) {
            double newValue = var->minValue() + (value / 100.0) * (var->maxValue() - var->minValue());
            var->setValue(newValue);
        }
    }
}

void FaceplateDialog::onModeChanged(int index)
{
    QString mode = m_modeCombo->currentText();
    QString msg = QString("Mode changed to %1 for %2").arg(mode, m_tag);
    m_processModel->processEvent(msg, QVariant());
    
    // Enable/disable manual controls based on mode
    bool isManual = (mode == "Manual");
    m_outputSlider->setEnabled(isManual);
    m_setpointSpin->setEnabled(!isManual);
}

void FaceplateDialog::onStartStop()
{
    auto var = m_processModel->getVariable(m_tag);
    if (!var) return;
    
    if (m_startStopButton->text() == "Start") {
        // Start the equipment
        double startValue = var->minValue() + (var->maxValue() - var->minValue()) * 0.5;
        var->setValue(startValue);
        QString msg = QString("Started %1").arg(m_tag);
        m_processModel->processEvent(msg, QVariant());
    } else {
        // Stop the equipment
        var->setValue(var->minValue());
        QString msg = QString("Stopped %1").arg(m_tag);
        m_processModel->processEvent(msg, QVariant());
    }
}

void FaceplateDialog::onAcknowledge()
{
    auto var = m_processModel->getVariable(m_tag);
    if (var && var->alarmActive()) {
        var->resetAlarm();
        QString msg = QString("Alarm acknowledged for %1").arg(m_tag);
        m_processModel->processEvent(msg, QVariant());
    }
}
