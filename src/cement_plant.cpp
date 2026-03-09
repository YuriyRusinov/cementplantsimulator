#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QtDebug>

#include <classroomwidget.h>
#include <plantmainwidget.h>
#include <simulationengine.h>
#include "cement_plant.h"

CementPlant::CementPlant( QObject *parent )
    : QObject( parent ),
    m_simulationEng( new SimulationEngine )
{
#ifdef CementPlantV1DEBUG
    qDebug() << __PRETTY_FUNCTION__;
#endif
}

CementPlant::~CementPlant() {
#ifdef CementPlantV1DEBUG
    qDebug() << __PRETTY_FUNCTION__;
#endif
}

void CementPlant::GUIViewControl( QWidget* parent, Qt::WindowFlags flags ) {
    QWidget* w = new PlantMainWidget( parent, flags ) ;//ClassRoomWidget( parent, flags );
    emit setWidgwet( w );
}
