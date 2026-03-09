#pragma once

#include <QWidget>
#include <QStackedWidget>
#include <QVBoxLayout>
#include "operatorpanel.h"

class PlantMainWidget : public QWidget {
    Q_OBJECT
public:
    explicit PlantMainWidget( QWidget *parent = nullptr, Qt::WindowFlags flags =  Qt::WindowFlags() );

private:
    // Core Layers from Diagram
    void setupTopLayer();    // OPStation, PXP, QCX
    void setupMiddleLayer(); // ECS & I/O System
    void setupBottomLayer(); // gPROMS/Model & Soft PLC

    QVBoxLayout *m_mainLayout;
    QStackedWidget *m_viewContainer; // For switching between OPStation/PXP/QCX
};

