#include "operatorpanel.h"
#include "ui_operator_panel.h"

OperatorPanel::OperatorPanel( QString unitTitle, QWidget* parent, Qt::WindowFlags flags )
    : QWidget( parent, flags ),
    _UI( new Ui::operator_panel ),
    m_prefix( unitTitle )
{
    _UI->setupUi(this);
    setupStyling();

    // Map the prefix to the internal UI labels
    _UI->lblUnitName->setText(m_prefix);

    // Connect local buttons to handlers
    connect(_UI->btnStart, &QPushButton::clicked, this, &OperatorPanel::handleStart);
    connect(_UI->btnStop, &QPushButton::clicked, this, &OperatorPanel::handleStop);
}

OperatorPanel::~OperatorPanel() {
    delete _UI;
}

void OperatorPanel::handleStart()
{
    // Logic: Send Start Command via Tag Bus
    // e.g., "KILN_MAIN_DRIVE_CMD_START"
    QString cmd = QString("%1_CMD_START").arg(m_prefix);
    //TagClient::instance().write(cmd, true);
}

void OperatorPanel::handleStop()
{
    QString cmd = QString("%1_CMD_STOP").arg(m_prefix);
    //TagClient::instance().write(cmd, true);
}

void OperatorPanel::updateValue(const QString &tagName, double value)
{
    // Dynamic updates for gauges and status indicators
    if (tagName.contains("TEMP")) {
        _UI->lcdProcessValue->display( value );
    } else if (tagName.contains("STATUS")) {
        // Change color based on RUN/STOP status
        _UI->statusIndicator->setStyleSheet(value > 0 ? "background: green;" : "background: red;");
    }
}

void OperatorPanel::setupStyling()
{
    // High-contrast industrial CSS
    this->setStyleSheet(
        "QWidget { background-color: #333333; border: 2px solid #555; border-radius: 4px; }"
        "QLabel { color: #00FF00; font-weight: bold; font-family: 'Courier New'; }"
        "QPushButton#btnStart { background-color: #004400; color: white; border: 1px solid #00FF00; }"
        "QPushButton#btnStop { background-color: #440000; color: white; border: 1px solid #FF0000; }"
    );
}
