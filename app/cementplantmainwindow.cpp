#include <QtDebug>
#include "cementplantmainwindow.h"
#include "ui_cement_plant_window.h"

CementPlantMainWindow::CementPlantMainWindow( QWidget* parent, Qt::WindowFlags flags )
    : QMainWindow( parent, flags ),
    _UI( new Ui::cement_plant_main_window ) 
{
    _UI->setupUi( this );

    connect( _UI->action_Open, &QAction::triggered, this, &CementPlantMainWindow::slotOpen );
    connect( _UI->action_Quit, &QAction::triggered, this, &QMainWindow::close );
}

CementPlantMainWindow::~CementPlantMainWindow() {
    delete _UI;
}

void CementPlantMainWindow::slotOpen() {
    qDebug() << __PRETTY_FUNCTION__;
}
