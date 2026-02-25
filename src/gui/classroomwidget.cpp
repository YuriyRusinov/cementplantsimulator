#include <QtDebug>
#include "classroomwidget.h"
#include "ui_class_room.h"

ClassRoomWidget::ClassRoomWidget( QWidget* parent, Qt::WindowFlags flags )
    : QWidget( parent, flags ),
    _UI( new Ui::class_room_widget )
{
    _UI->setupUi( this );

    connect( _UI->pbOPStation, &QAbstractButton::clicked, this, &ClassRoomWidget::slotOPS );
    connect( _UI->pbPXP, &QAbstractButton::clicked, this, &ClassRoomWidget::slotPXP );
    connect( _UI->pbQCX, &QAbstractButton::clicked, this, &ClassRoomWidget::slotQCX );
    connect( _UI->pbECS, &QAbstractButton::clicked, this, &ClassRoomWidget::slotECS );
    connect( _UI->pbIO, &QAbstractButton::clicked, this, &ClassRoomWidget::slotIOSys );
    connect( _UI->pbPLCPlant, &QAbstractButton::clicked, this, &ClassRoomWidget::slotPLant );
}

ClassRoomWidget::~ClassRoomWidget() {
    delete _UI;
}

void ClassRoomWidget::slotOPS() {
    qDebug() << __PRETTY_FUNCTION__;
}

void ClassRoomWidget::slotPXP() {
    qDebug() << __PRETTY_FUNCTION__;
}

void ClassRoomWidget::slotQCX() {
    qDebug() << __PRETTY_FUNCTION__;
}

void ClassRoomWidget::slotECS() {
    qDebug() << __PRETTY_FUNCTION__;
}

void ClassRoomWidget::slotIOSys() {
    qDebug() << __PRETTY_FUNCTION__;
}

void ClassRoomWidget::slotPLant() {
    qDebug() << __PRETTY_FUNCTION__;
}
