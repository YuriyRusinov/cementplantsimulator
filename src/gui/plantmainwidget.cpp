#include "plantmainwidget.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>

PlantMainWidget::PlantMainWidget( QWidget *parent, Qt::WindowFlags flags ) : QWidget( parent, flags ) {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);

    // Initializing the layers defined in the image
    setupTopLayer();
    setupMiddleLayer(); // The ECS 'Bus'
    
    m_viewContainer = new QStackedWidget(this);
    m_mainLayout->addWidget(m_viewContainer, 1); // Main display area
}

void PlantMainWidget::setupTopLayer() {
    auto *topBar = new QWidget();
    auto *layout = new QHBoxLayout(topBar);

    // Modules from diagram: OPStation, PXP, QCX
    QPushButton *btnOp = new QPushButton("OPStation");
    QPushButton *btnPxp = new QPushButton("PXP");
    QPushButton *btnQcx = new QPushButton("QCX");

    layout->addWidget(btnOp);
    layout->addWidget(btnPxp);
    layout->addWidget(btnQcx);

    m_mainLayout->addWidget(topBar);
}

void PlantMainWidget::setupMiddleLayer() {
    // The ECS acts as the bridge to the I/O System
    auto *ecsHeader = new QLabel("EXPERT CONTROL SYSTEM (ECS) ACTIVE");
    ecsHeader->setAlignment(Qt::AlignCenter);
    ecsHeader->setStyleSheet("background-color: #A0A0A0; color: black; font-weight: bold;");
    
    m_mainLayout->addWidget(ecsHeader);
}
