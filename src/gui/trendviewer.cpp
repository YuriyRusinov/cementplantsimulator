#include "trendviewer.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QFile>
#include <QTextStream>
#include <cmath>

TrendViewer::TrendViewer(QSharedPointer<ProcessModel> model, QWidget *parent)
    : QWidget(parent)
    , m_processModel(model)
    , m_timeRange(300)
    , m_timeOffset(0)
    , m_yMin(0)
    , m_yMax(100)
    , m_autoRange(true)
    , m_isPanning(false)
    , m_showGrid(true)
    , m_showLegend(true)
{
    setMinimumHeight(200);
    setMouseTracking(true);
    
    // Add default trends
    addTrend("kiln_temperature", QColor(255, 50, 50));
    addTrend("free_lime", QColor(50, 255, 50));
    addTrend("oxygen", QColor(50, 50, 255));
    addTrend("co", QColor(255, 255, 50));
}

void TrendViewer::addTrend(const QString& tag, const QColor& color)
{
    if (!m_trends.contains(tag)) {
        TrendData data;
        data.tag = tag;
        data.color = color;
        data.visible = true;
        data.minValue = 0;
        data.maxValue = 100;
        m_trends[tag] = data;
        update();
    }
}

void TrendViewer::removeTrend(const QString& tag)
{
    m_trends.remove(tag);
    update();
}

void TrendViewer::clearTrends()
{
    m_trends.clear();
    update();
}

void TrendViewer::setTimeRange(int seconds)
{
    m_timeRange = qBound(30, seconds, 3600);
    update();
}

void TrendViewer::setAutoRange(bool autoRange)
{
    m_autoRange = autoRange;
    update();
}

void TrendViewer::updateTrends()
{
    if (!m_autoRange) {
        update();
        return;
    }
    
    // Calculate Y range based on visible trends
    double globalMin = 1e9;
    double globalMax = -1e9;
    bool hasData = false;
    
    double currentTime = QDateTime::currentDateTime().toSecsSinceEpoch();
    double cutoffTime = currentTime - m_timeRange;
    
    for (auto& trend : m_trends) {
        if (!trend.visible) continue;
        
        auto var = m_processModel->getVariable(trend.tag);
        if (!var) continue;
        
        auto history = var->getHistory(m_timeRange);
        double trendMin = 1e9;
        double trendMax = -1e9;
        
        for (const auto& point : history) {
            double pointTime = point.timestamp.toSecsSinceEpoch();
            if (pointTime >= cutoffTime) {
                trendMin = qMin(trendMin, point.value);
                trendMax = qMax(trendMax, point.value);
                hasData = true;
            }
        }
        
        if (trendMin < 1e9) {
            trend.minValue = trendMin;
            trend.maxValue = trendMax;
            globalMin = qMin(globalMin, trendMin);
            globalMax = qMax(globalMax, trendMax);
        }
    }
    
    if (hasData) {
        double padding = (globalMax - globalMin) * 0.1;
        m_yMin = globalMin - padding;
        m_yMax = globalMax + padding;
        
        if (qAbs(m_yMax - m_yMin) < 0.001) {
            m_yMin = m_yMin - 1;
            m_yMax = m_yMax + 1;
        }
    }
    
    update();
}

void TrendViewer::exportTrendData(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) return;
    
    QTextStream stream(&file);
    
    // Write header
    stream << "Time";
    for (auto it = m_trends.begin(); it != m_trends.end(); ++it) {
        stream << "," << it.key();
    }
    stream << "\n";
    
    // Get data points
    QMap<double, QMap<QString, double>> timeData;
    double currentTime = QDateTime::currentDateTime().toSecsSinceEpoch();
    double cutoffTime = currentTime - m_timeRange;
    
    for (auto& trend : m_trends) {
        if (!trend.visible) continue;
        
        auto var = m_processModel->getVariable(trend.tag);
        if (!var) continue;
        
        auto history = var->getHistory(m_timeRange);
        for (const auto& point : history) {
            double pointTime = point.timestamp.toSecsSinceEpoch();
            if (pointTime >= cutoffTime) {
                timeData[pointTime][trend.tag] = point.value;
            }
        }
    }
    
    // Write data rows
    for (auto it = timeData.begin(); it != timeData.end(); ++it) {
        QDateTime dt = QDateTime::fromSecsSinceEpoch(it.key());
        stream << dt.toString("yyyy-MM-dd hh:mm:ss");
        
        for (auto trendIt = m_trends.begin(); trendIt != m_trends.end(); ++trendIt) {
            stream << "," << QString::number(it.value()[trendIt.key()], 'f', 2);
        }
        stream << "\n";
    }
    
    file.close();
}

void TrendViewer::zoomIn()
{
    m_timeRange = qMax(30.0, m_timeRange * 0.8);
    update();
}

void TrendViewer::zoomOut()
{
    m_timeRange = qMin(3600.0, m_timeRange * 1.2);
    update();
}

void TrendViewer::panLeft()
{
    m_timeOffset += m_timeRange * 0.1;
    update();
}

void TrendViewer::panRight()
{
    m_timeOffset = qMax(0.0, m_timeOffset - m_timeRange * 0.1);
    update();
}

void TrendViewer::setShowGrid(bool show)
{
    m_showGrid = show;
    update();
}

void TrendViewer::setShowLegend(bool show)
{
    m_showLegend = show;
    update();
}

void TrendViewer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(30, 30, 30));
    
    QRect plotRect = rect().adjusted(MARGIN, MARGIN, -MARGIN, -MARGIN);
    
    if (m_showGrid) {
        painter.setPen(QPen(QColor(60, 60, 60), 1));
        
        // Vertical grid lines
        for (int i = 0; i <= 4; i++) {
            int x = plotRect.left() + (plotRect.width() * i) / 4;
            painter.drawLine(x, plotRect.top(), x, plotRect.bottom());
        }
        
        // Horizontal grid lines
        for (int i = 0; i <= 4; i++) {
            int y = plotRect.top() + (plotRect.height() * i) / 4;
            painter.drawLine(plotRect.left(), y, plotRect.right(), y);
        }
    }
    
    // Draw axes
    painter.setPen(QPen(QColor(150, 150, 150), 2));
    painter.drawRect(plotRect);
    
    // Draw Y-axis labels
    painter.setPen(QColor(200, 200, 200));
    QFont font = painter.font();
    font.setPointSize(8);
    painter.setFont(font);
    
    for (int i = 0; i <= 4; i++) {
        double value = m_yMin + (m_yMax - m_yMin) * (1.0 - i / 4.0);
        int y = plotRect.top() + (plotRect.height() * i) / 4;
        painter.drawText(5, y + 3, QString::number(value, 'f', 1));
    }
    
    // Draw X-axis labels
    double currentTime = QDateTime::currentDateTime().toSecsSinceEpoch();
    for (int i = 0; i <= 4; i++) {
        double timeOffset = (m_timeRange * i) / 4.0;
        double pointTime = currentTime - m_timeOffset - timeOffset;
        QDateTime dt = QDateTime::fromSecsSinceEpoch(pointTime);
        
        int x = plotRect.left() + (plotRect.width() * i) / 4;
        painter.drawText(x - 20, plotRect.bottom() + 15, dt.toString("hh:mm:ss"));
    }
    
    // Draw trends
    for (auto& trend : m_trends) {
        if (!trend.visible) continue;
        
        auto var = m_processModel->getVariable(trend.tag);
        if (!var) continue;
        
        auto history = var->getHistory(m_timeRange);
        
        QVector<QPointF> points;
        double cutoffTime = currentTime - m_timeRange - m_timeOffset;
        
        for (const auto& point : history) {
            double pointTime = point.timestamp.toSecsSinceEpoch();
            if (pointTime >= cutoffTime && pointTime <= currentTime - m_timeOffset) {
                double x = plotRect.left() + (plotRect.width() * 
                          (1.0 - (currentTime - m_timeOffset - pointTime) / m_timeRange));
                double y = plotRect.bottom() - (plotRect.height() * 
                          ((point.value - m_yMin) / (m_yMax - m_yMin)));
                
                if (y >= plotRect.top() && y <= plotRect.bottom()) {
                    points.append(QPointF(x, y));
                }
            }
        }
        
        if (points.size() > 1) {
            painter.setPen(QPen(trend.color, 2));
            for (int i = 0; i < points.size() - 1; i++) {
                painter.drawLine(points[i], points[i + 1]);
            }
        }
    }
    
    // Draw legend
    if (m_showLegend) {
        int legendX = plotRect.right() + 10;
        int legendY = plotRect.top();
        int legendItemHeight = 20;
        
        for (auto& trend : m_trends) {
            if (!trend.visible) continue;
            
            painter.setPen(QPen(trend.color, 2));
            painter.drawLine(legendX, legendY + 5, legendX + 20, legendY + 5);
            
            painter.setPen(QColor(200, 200, 200));
            painter.drawText(legendX + 25, legendY + 9, trend.tag);
            
            legendY += legendItemHeight;
        }
    }
}

void TrendViewer::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isPanning = true;
        m_lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
}

void TrendViewer::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isPanning) {
        int dx = event->x() - m_lastMousePos.x();
        double panDelta = (dx / (double)width()) * m_timeRange;
        m_timeOffset = qMax(0.0, m_timeOffset - panDelta);
        m_lastMousePos = event->pos();
        update();
    }
}

void TrendViewer::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_isPanning = false;
    setCursor(Qt::ArrowCursor);
}

void TrendViewer::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() > 0) {
        zoomIn();
    } else if (event->angleDelta().y() < 0) {
        zoomOut();
    }
}

void TrendViewer::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    update();
}
