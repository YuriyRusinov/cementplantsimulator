#pragma once

#include <QtCore>

class QWidget;

class CementPlant : public QObject {
public:
    CementPlant(QObject *parent = nullptr);
    virtual ~CementPlant();

    void GUIViewControl( QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags() );

signals:
    void setWidgwet( QWidget* w );

private:
    Q_OBJECT
};
