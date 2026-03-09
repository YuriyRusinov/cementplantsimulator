#pragma once

#include <QtCore>

class QWidget;
class SimulationEngine;

class CementPlant : public QObject {
public:
    CementPlant(QObject *parent = nullptr);
    virtual ~CementPlant();

    void GUIViewControl( QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags() );

signals:
    void setWidgwet( QWidget* w );

private:
        SimulationEngine* m_simulationEng;
private:
    Q_OBJECT
};
