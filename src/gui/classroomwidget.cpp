#include "classroomwidget.h"
#include "ui_class_room.h"

ClassRoomWidget::ClassRoomWidget( QWidget* parent, Qt::WindowFlags flags )
    : QWidget( parent, flags ),
    _UI( new Ui::class_room_widget )
{
    _UI->setupUi( this );
}

ClassRoomWidget::~ClassRoomWidget() {
    delete _UI;
}

