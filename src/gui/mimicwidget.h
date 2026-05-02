#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsTextItem>
#include <QMap>
#include <QSharedPointer>
#include "processmodel.h"

class ProcessMimic : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ProcessMimic(QSharedPointer<ProcessModel> model, const QString& area, QWidget *parent = nullptr);
    
    void setArea(const QString& area) { m_area = area; }
    void updateDisplay();
    void setHighlighted(bool highlighted) { m_highlighted = highlighted; }

signals:
    void equipmentClicked(const QString& tag);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    void createOverview();
    void createKilnView();
    void createRawMillView();
    void createCementMillView();
    void createPreheaterView();
    void createCoolerView();
    
    QSharedPointer<ProcessModel> m_processModel;
    QString m_area;
    QGraphicsScene* m_scene;
    bool m_highlighted;
    QMap<QString, QGraphicsItem*> m_graphicsItems;
};
