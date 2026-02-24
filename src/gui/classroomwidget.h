#pragma once

#include <QWidget>

namespace Ui {
    class class_room_widget;
};

class ClassRoomWidget : public QWidget {
public:
    ClassRoomWidget( QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags() );
    virtual ~ClassRoomWidget();

private:
    Ui::class_room_widget* _UI;

private:
    Q_OBJECT
};
