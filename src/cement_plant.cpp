#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QtDebug>

#include <classroomwidget.h>
#include "cement_plant.h"

CementPlant::CementPlant( QObject *parent )
    : QObject( parent ) 
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
    QWidget* w = new ClassRoomWidget( parent, flags );
    emit setWidgwet( w );
}
