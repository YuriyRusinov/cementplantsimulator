#pragma once

#include <QMainWindow>

namespace Ui {
    class cement_plant_main_window;
};

class CementPlantMainWindow : public QMainWindow {
public:
    CementPlantMainWindow( QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags() );
    virtual ~CementPlantMainWindow();

private slots:
    void slotOpen();

private:
    Ui::cement_plant_main_window* _UI;

private:
    Q_OBJECT
};
