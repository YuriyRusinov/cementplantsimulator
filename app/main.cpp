#include <QApplication>
#include <QSharedPointer>
#include <iostream>
#include <memory>

#include <cement_plant.h>
#include "cementplantmainwindow.h"

int main(int argc, char* argv[]) {
    std::cout << " Test application for cement plant simulator " << argv[0] << " number of args " << argc << std::endl;
    //CementPlant* cPtr = &CementPlant::instance();
    //std::shared_ptr< CementPlant > mCementP( cPtr );//std::make_shared<CementPlant::instance()>());// = std::shared_ptr<CementPlant>(nullptr);//CementPlant::instance());
    QSharedPointer<QApplication> app ( new QApplication(argc, argv) );

    QSharedPointer<CementPlantMainWindow> cmpW ( new CementPlantMainWindow );
    cmpW->show();

    int res = app->exec();

    return res;
}
