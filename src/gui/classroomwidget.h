#pragma once

#include <QWidget>

namespace Ui {
    class class_room_widget;
};

class ClassRoomWidget : public QWidget {
public:
    ClassRoomWidget( QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags() );
    virtual ~ClassRoomWidget();

private slots:
    void slotOPS();
    void slotPXP();
    void slotQCX();
    void slotECS();
    void slotIOSys();
    void slotPLant();

private:
    Ui::class_room_widget* _UI;

private:
    Q_OBJECT
};
