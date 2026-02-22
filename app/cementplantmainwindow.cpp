#include <QMdiSubWindow>
#include <QtDebug>
#include <cement_plant.h>
#include "cementplantmainwindow.h"
#include "ui_cement_plant_window.h"

CementPlantMainWindow::CementPlantMainWindow( QWidget* parent, Qt::WindowFlags flags )
    : QMainWindow( parent, flags ),
    _UI( new Ui::cement_plant_main_window ),
    m_CementPlant( new CementPlant() )
{
    _UI->setupUi( this );

    connect( _UI->action_Open, &QAction::triggered, this, &CementPlantMainWindow::slotOpen );
    connect( _UI->action_Quit, &QAction::triggered, this, &QMainWindow::close );
    connect( m_CementPlant.get(), &CementPlant::setWidgwet, this, &CementPlantMainWindow::slotAddWidget );
}

CementPlantMainWindow::~CementPlantMainWindow() {
    delete _UI;
}

void CementPlantMainWindow::slotOpen() {
    qDebug() << __PRETTY_FUNCTION__;
    m_CementPlant->GUIViewControl();
}

void CementPlantMainWindow::slotAddWidget( QWidget* widget ) {
    if( widget == nullptr)
        return;
    QMdiSubWindow * mdiW = _UI->mdiArea->addSubWindow( widget, Qt::SubWindow );
    widget->setAttribute( Qt::WA_DeleteOnClose );
    widget->show();
    mdiW->setAttribute( Qt::WA_DeleteOnClose );
    mdiW->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    mdiW->setMinimumSize( QSize(512, 512) );
}
