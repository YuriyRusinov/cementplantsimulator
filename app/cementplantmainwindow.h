#pragma once

#include <memory>
#include <QMainWindow>

class QWidget;

namespace Ui {
    class cement_plant_main_window;
};

class CementPlant;

class CementPlantMainWindow : public QMainWindow {
public:
    CementPlantMainWindow( QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags() );
    virtual ~CementPlantMainWindow();

private slots:
    void slotOpen();

    void slotAddWidget( QWidget* widget );

private:
    Ui::cement_plant_main_window* _UI;
    QSharedPointer< CementPlant > m_CementPlant;

private:
    Q_OBJECT
};
