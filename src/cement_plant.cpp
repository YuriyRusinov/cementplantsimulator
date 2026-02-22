#include <QWidget>
#include <QLabel>
#include <QGridLayout>
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

void CementPlant::GUIViewControl( QWidget* parent, Qt::WindowFlags flags ) {
    QWidget* w = new QWidget( parent, flags );
    QLabel* lTitle = new QLabel(tr("Cement plant control"), w );
    QGridLayout* lay = new QGridLayout( w );
    lay->addWidget( lTitle, 0, 0, 1, 1 );
    emit setWidgwet( w );
}
