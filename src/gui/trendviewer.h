#pragma once

#include <QWidget>
#include <QMap>
#include <QVector>
#include <QPointF>
#include <QColor>
#include <QDateTime>
#include <QSharedPointer>
#include "processmodel.h"

class TrendViewer : public QWidget
{
    Q_OBJECT

public:
    explicit TrendViewer(QSharedPointer<ProcessModel> model, QWidget *parent = nullptr);
    
    void addTrend(const QString& tag, const QColor& color);
    void removeTrend(const QString& tag);
    void clearTrends();
    void setTimeRange(int seconds);
    void setAutoRange(bool autoRange);
    void exportTrendData(const QString& filename);

public slots:
    void updateTrends();
    void zoomIn();
    void zoomOut();
    void panLeft();
    void panRight();
    void setShowGrid(bool show);
    void setShowLegend(bool show);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    struct TrendData {
        QString tag;
        QColor color;
        QVector<QPointF> points;
        double minValue;
        double maxValue;
        bool visible;
    };
    
    QSharedPointer<ProcessModel> m_processModel;
    QMap<QString, TrendData> m_trends;
    
    double m_timeRange;
    double m_timeOffset;
    double m_yMin;
    double m_yMax;
    bool m_autoRange;
    
    QPoint m_lastMousePos;
    bool m_isPanning;
    
    bool m_showGrid;
    bool m_showLegend;
    
    static const int MARGIN = 50;
};
