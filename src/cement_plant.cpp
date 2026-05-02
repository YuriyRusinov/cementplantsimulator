#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QtDebug>

#include <classroomwidget.h>
#include <plantmainwidget.h>
#include <simulationengine.h>
#include "softplc.h"
#include "tagbridge.h"

#include "cement_plant.h"
#include "opstationwidget.h"

CementPlant::CementPlant( QObject *parent )
    : QObject( parent ),
    m_model( std::make_shared< SimulationEngine >()),
    m_plc( std::make_shared<SoftPLC>()),
    m_bridge( std::make_shared<TagBridge>() ),
    m_proc( std::make_shared<ProcessModel>() )
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
    PlantMainWidget* w = new PlantMainWidget( parent, flags ) ;//ClassRoomWidget( parent, flags );
    emit setWidgwet( w );

    connect( w, &PlantMainWidget::startOPStation, this, &CementPlant::plantOPS );
    connect( w, &PlantMainWidget::startPXP, this, &CementPlant::plantPXP );
    connect( w, &PlantMainWidget::startQCX, this, &CementPlant::plantQCX );
}

void CementPlant::plantOPS() {
    qDebug() << __PRETTY_FUNCTION__;

    QWidget* wSender = qobject_cast< QWidget* >( this->sender() );
    OpStationWidget* opsW = new OpStationWidget( QSharedPointer< ProcessModel >(m_proc.get()), wSender );
    emit setWidgwet( opsW );
}

void CementPlant::plantPXP() {
    qDebug() << __PRETTY_FUNCTION__;
}

void CementPlant::plantQCX() {
    qDebug() << __PRETTY_FUNCTION__;
}

