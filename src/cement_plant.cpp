#include <QtDebug>
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
