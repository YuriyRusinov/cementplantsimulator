#pragma once

#include <QtCore>

class CementPlant : public QObject {
public:
    CementPlant(QObject *parent = nullptr);
    virtual ~CementPlant();

private:
    Q_OBJECT
};
