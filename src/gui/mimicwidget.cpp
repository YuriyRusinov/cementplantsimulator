#include "mimicwidget.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QLinearGradient>
#include <QFont>
#include <QMouseEvent>

ProcessMimic::ProcessMimic(QSharedPointer<ProcessModel> model, const QString& area, QWidget *parent)
    : QGraphicsView(parent)
    , m_processModel(model)
    , m_area(area)
    , m_highlighted(false)
{
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);
    setRenderHint(QPainter::Antialiasing);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setStyleSheet("background-color: #1e1e1e; border: none;");
    
    // Create the appropriate view based on area
    if (area == "overview") createOverview();
    else if (area == "kiln") createKilnView();
    else if (area == "rawmill") createRawMillView();
    else if (area == "cementmill") createCementMillView();
    else if (area == "preheater") createPreheaterView();
    else if (area == "cooler") createCoolerView();
}

void ProcessMimic::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
}

void ProcessMimic::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
    
    // Find which equipment was clicked
    QPointF scenePos = mapToScene(event->pos());
    QGraphicsItem* item = scene()->itemAt(scenePos, QTransform());
    
    if (item) {
        QString tag = item->data(0).toString();
        if (!tag.isEmpty()) {
            emit equipmentClicked(tag);
        }
    }
}

void ProcessMimic::updateDisplay()
{
    // Update colors and values of all equipment
    // For kiln temperature, update the color of the kiln body
    auto kilnTempVar = m_processModel->getVariable("kiln_temperature");
    if (kilnTempVar) {
        double value = kilnTempVar->value();
        double intensity = (value - 800.0) / 1000.0;
        intensity = qBound(0.0, intensity, 1.0);
        
        QColor color;
        if (value > 1550) color = QColor(255, 0, 0);
        else if (value > 1450) color = QColor(255, 100, 0);
        else if (value > 1300) color = QColor(255, 200, 0);
        else color = QColor(255, 255, 0);
        
        // Find the kiln body item and cast to QGraphicsRectItem
        if (m_graphicsItems.contains("kiln_body")) {
            auto* rectItem = dynamic_cast<QGraphicsRectItem*>(m_graphicsItems["kiln_body"]);
            if (rectItem) {
                rectItem->setBrush(QBrush(color));
            }
        }
        
        // Update temperature text
        if (m_graphicsItems.contains("temp_text")) {
            auto* textItem = dynamic_cast<QGraphicsTextItem*>(m_graphicsItems["temp_text"]);
            if (textItem) {
                textItem->setPlainText(QString("Kiln Temperature: %1 °C").arg(value, 0, 'f', 0));
            }
        }
    }
    
    // Update kiln speed text
    auto kilnSpeedVar = m_processModel->getVariable("kiln_speed");
    if (kilnSpeedVar && m_graphicsItems.contains("speed_text")) {
        auto* textItem = dynamic_cast<QGraphicsTextItem*>(m_graphicsItems["speed_text"]);
        if (textItem) {
            textItem->setPlainText(QString("Kiln Speed: %1 rpm").arg(kilnSpeedVar->value(), 0, 'f', 1));
        }
    }
    
    // Update raw mill power text
    auto rawMillPowerVar = m_processModel->getVariable("raw_mill_power");
    if (rawMillPowerVar && m_graphicsItems.contains("power_text")) {
        auto* textItem = dynamic_cast<QGraphicsTextItem*>(m_graphicsItems["power_text"]);
        if (textItem) {
            textItem->setPlainText(QString("Mill Power: %1 kW").arg(rawMillPowerVar->value(), 0, 'f', 0));
        }
    }
    
    // Update raw mill vibration text
    auto rawMillVibVar = m_processModel->getVariable("raw_mill_vibration");
    if (rawMillVibVar && m_graphicsItems.contains("vib_text")) {
        auto* textItem = dynamic_cast<QGraphicsTextItem*>(m_graphicsItems["vib_text"]);
        if (textItem) {
            double vib = rawMillVibVar->value();
            textItem->setPlainText(QString("Vibration: %1 mm/s").arg(vib, 0, 'f', 1));
            
            // Change color based on vibration level
            if (vib > 5.0) {
                textItem->setDefaultTextColor(QColor(255, 50, 50));
            } else if (vib > 3.0) {
                textItem->setDefaultTextColor(QColor(255, 200, 50));
            } else {
                textItem->setDefaultTextColor(QColor(50, 255, 50));
            }
        }
    }
    
    // Update cement mill power text
    auto cementMillPowerVar = m_processModel->getVariable("cement_mill_power");
    if (cementMillPowerVar && m_graphicsItems.contains("cement_power_text")) {
        auto* textItem = dynamic_cast<QGraphicsTextItem*>(m_graphicsItems["cement_power_text"]);
        if (textItem) {
            textItem->setPlainText(QString("Mill Power: %1 kW").arg(cementMillPowerVar->value(), 0, 'f', 0));
        }
    }
    
    // Update cement mill temperature text
    auto cementMillTempVar = m_processModel->getVariable("cement_mill_temp");
    if (cementMillTempVar && m_graphicsItems.contains("cement_temp_text")) {
        auto* textItem = dynamic_cast<QGraphicsTextItem*>(m_graphicsItems["cement_temp_text"]);
        if (textItem) {
            double temp = cementMillTempVar->value();
            textItem->setPlainText(QString("Temperature: %1 °C").arg(temp, 0, 'f', 0));
            
            if (temp > 120) {
                textItem->setDefaultTextColor(QColor(255, 50, 50));
            } else if (temp > 100) {
                textItem->setDefaultTextColor(QColor(255, 200, 50));
            } else {
                textItem->setDefaultTextColor(QColor(255, 255, 255));
            }
        }
    }
    
    // Update preheater temperature text
    auto preheaterTempVar = m_processModel->getVariable("preheater_temp");
    if (preheaterTempVar && m_graphicsItems.contains("preheater_temp_text")) {
        auto* textItem = dynamic_cast<QGraphicsTextItem*>(m_graphicsItems["preheater_temp_text"]);
        if (textItem) {
            textItem->setPlainText(QString("Preheater Temp: %1 °C").arg(preheaterTempVar->value(), 0, 'f', 0));
        }
    }
    
    // Update calciner temperature text
    auto calcinerTempVar = m_processModel->getVariable("calciner_temp");
    if (calcinerTempVar && m_graphicsItems.contains("calciner_temp_text")) {
        auto* textItem = dynamic_cast<QGraphicsTextItem*>(m_graphicsItems["calciner_temp_text"]);
        if (textItem) {
            textItem->setPlainText(QString("Calciner Temp: %1 °C").arg(calcinerTempVar->value(), 0, 'f', 0));
        }
    }
    
    // Update cooler outlet temperature text
    auto coolerTempVar = m_processModel->getVariable("cooler_temp_out");
    if (coolerTempVar && m_graphicsItems.contains("cooler_temp_text")) {
        auto* textItem = dynamic_cast<QGraphicsTextItem*>(m_graphicsItems["cooler_temp_text"]);
        if (textItem) {
            textItem->setPlainText(QString("Outlet Temp: %1 °C").arg(coolerTempVar->value(), 0, 'f', 0));
        }
    }
    
    // Update clinker temperature text
    auto clinkerTempVar = m_processModel->getVariable("clinker_temp");
    if (clinkerTempVar && m_graphicsItems.contains("clinker_temp_text")) {
        auto* textItem = dynamic_cast<QGraphicsTextItem*>(m_graphicsItems["clinker_temp_text"]);
        if (textItem) {
            textItem->setPlainText(QString("Clinker Temp: %1 °C").arg(clinkerTempVar->value(), 0, 'f', 0));
        }
    }
    
    // Update blaine text
    auto blaineVar = m_processModel->getVariable("blaine");
    if (blaineVar && m_graphicsItems.contains("blaine_text")) {
        auto* textItem = dynamic_cast<QGraphicsTextItem*>(m_graphicsItems["blaine_text"]);
        if (textItem) {
            textItem->setPlainText(QString("Blaine: %1 cm²/g").arg(blaineVar->value(), 0, 'f', 0));
        }
    }
    
    viewport()->update();
}

void ProcessMimic::createOverview()
{
    scene()->setSceneRect(0, 0, 800, 600);
    
    // Draw main equipment positions
    QMap<QString, QPointF> positions;
    positions["Raw Mill"] = QPointF(100, 200);
    positions["Preheater"] = QPointF(300, 150);
    positions["Kiln"] = QPointF(500, 250);
    positions["Cooler"] = QPointF(650, 350);
    positions["Cement Mill"] = QPointF(350, 450);
    
    for (auto it = positions.begin(); it != positions.end(); ++it) {
        QGraphicsRectItem* rect = scene()->addRect(it->x(), it->y(), 100, 80);
        rect->setBrush(QBrush(QColor(100, 100, 150)));
        rect->setPen(QPen(QColor(200, 200, 200), 2));
        rect->setData(0, it.key());
        
        QGraphicsTextItem* text = scene()->addText(it.key());
        text->setDefaultTextColor(QColor(255, 255, 255));
        text->setPos(it->x() + 10, it->y() + 30);
        
        m_graphicsItems[it.key()] = rect;
    }
    
    // Draw connections
    QPen connectionPen(QColor(150, 150, 150), 3);
    scene()->addLine(200, 240, 300, 190, connectionPen);
    scene()->addLine(400, 190, 500, 290, connectionPen);
    scene()->addLine(600, 290, 650, 350, connectionPen);
    scene()->addLine(400, 300, 350, 450, connectionPen);
}

void ProcessMimic::createKilnView()
{
    scene()->setSceneRect(0, 0, 800, 600);
    
    // Draw kiln body
    QGraphicsRectItem* kilnBody = scene()->addRect(200, 200, 400, 80);
    kilnBody->setBrush(QBrush(QColor(150, 100, 50)));
    kilnBody->setPen(QPen(QColor(200, 200, 200), 2));
    kilnBody->setData(0, "kiln");
    m_graphicsItems["kiln_body"] = kilnBody;
    
    // Draw burner
    QGraphicsRectItem* burner = scene()->addRect(600, 220, 50, 40);
    burner->setBrush(QBrush(QColor(200, 50, 50)));
    burner->setPen(QPen(QColor(255, 100, 100), 2));
    burner->setData(0, "burner");
    m_graphicsItems["burner"] = burner;
    
    // Draw feed end
    QGraphicsRectItem* feedEnd = scene()->addRect(150, 220, 50, 40);
    feedEnd->setBrush(QBrush(QColor(100, 100, 100)));
    feedEnd->setData(0, "feed_end");
    m_graphicsItems["feed_end"] = feedEnd;
    
    // Add temperature text
    QGraphicsTextItem* tempText = scene()->addText("Kiln Temperature: --- °C");
    tempText->setDefaultTextColor(QColor(255, 255, 255));
    tempText->setPos(300, 100);
    m_graphicsItems["temp_text"] = tempText;
    
    // Add speed text
    QGraphicsTextItem* speedText = scene()->addText("Kiln Speed: --- rpm");
    speedText->setDefaultTextColor(QColor(255, 255, 255));
    speedText->setPos(300, 130);
    m_graphicsItems["speed_text"] = speedText;
    
    // Add labels
    QGraphicsTextItem* burnerLabel = scene()->addText("Burner");
    burnerLabel->setDefaultTextColor(QColor(255, 255, 255));
    burnerLabel->setPos(610, 230);
    
    QGraphicsTextItem* kilnLabel = scene()->addText("Rotary Kiln");
    kilnLabel->setDefaultTextColor(QColor(255, 255, 255));
    kilnLabel->setPos(350, 180);
}

void ProcessMimic::createRawMillView()
{
    scene()->setSceneRect(0, 0, 800, 600);
    
    // Draw mill body
    QGraphicsEllipseItem* millBody = scene()->addEllipse(250, 200, 200, 200);
    millBody->setBrush(QBrush(QColor(100, 100, 120)));
    millBody->setPen(QPen(QColor(200, 200, 200), 2));
    millBody->setData(0, "raw_mill");
    m_graphicsItems["mill_body"] = millBody;
    
    // Draw feed inlet
    QGraphicsRectItem* feedInlet = scene()->addRect(300, 150, 100, 50);
    feedInlet->setBrush(QBrush(QColor(80, 80, 100)));
    feedInlet->setData(0, "feed_inlet");
    m_graphicsItems["feed_inlet"] = feedInlet;
    
    // Draw product outlet
    QGraphicsRectItem* outlet = scene()->addRect(300, 400, 100, 50);
    outlet->setBrush(QBrush(QColor(80, 80, 100)));
    outlet->setData(0, "outlet");
    m_graphicsItems["outlet"] = outlet;
    
    // Draw separator
    QGraphicsRectItem* separator = scene()->addRect(550, 250, 80, 100);
    separator->setBrush(QBrush(QColor(120, 120, 140)));
    separator->setData(0, "separator");
    m_graphicsItems["separator"] = separator;
    
    // Add text displays
    QGraphicsTextItem* powerText = scene()->addText("Mill Power: --- kW");
    powerText->setDefaultTextColor(QColor(255, 255, 255));
    powerText->setPos(250, 50);
    m_graphicsItems["power_text"] = powerText;
    
    QGraphicsTextItem* vibText = scene()->addText("Vibration: --- mm/s");
    vibText->setDefaultTextColor(QColor(255, 255, 255));
    vibText->setPos(250, 80);
    m_graphicsItems["vib_text"] = vibText;
    
    // Add labels
    QGraphicsTextItem* millLabel = scene()->addText("Vertical Raw Mill");
    millLabel->setDefaultTextColor(QColor(255, 255, 255));
    millLabel->setPos(280, 180);
}

void ProcessMimic::createCementMillView()
{
    scene()->setSceneRect(0, 0, 800, 600);
    
    // Draw ball mill
    QGraphicsRectItem* millBody = scene()->addRect(200, 250, 300, 80);
    millBody->setBrush(QBrush(QColor(100, 100, 120)));
    millBody->setPen(QPen(QColor(200, 200, 200), 2));
    millBody->setData(0, "cement_mill");
    m_graphicsItems["mill_body"] = millBody;
    
    // Draw motor
    QGraphicsRectItem* motor = scene()->addRect(150, 260, 50, 60);
    motor->setBrush(QBrush(QColor(80, 80, 100)));
    motor->setData(0, "motor");
    m_graphicsItems["motor"] = motor;
    
    // Draw gearbox
    QGraphicsRectItem* gearbox = scene()->addRect(180, 275, 40, 30);
    gearbox->setBrush(QBrush(QColor(120, 120, 100)));
    m_graphicsItems["gearbox"] = gearbox;
    
    // Add text displays
    QGraphicsTextItem* powerText = scene()->addText("Mill Power: --- kW");
    powerText->setDefaultTextColor(QColor(255, 255, 255));
    powerText->setPos(300, 100);
    m_graphicsItems["cement_power_text"] = powerText;
    
    QGraphicsTextItem* tempText = scene()->addText("Temperature: --- °C");
    tempText->setDefaultTextColor(QColor(255, 255, 255));
    tempText->setPos(300, 130);
    m_graphicsItems["cement_temp_text"] = tempText;
    
    QGraphicsTextItem* blaineText = scene()->addText("Blaine: --- cm²/g");
    blaineText->setDefaultTextColor(QColor(255, 255, 255));
    blaineText->setPos(300, 160);
    m_graphicsItems["blaine_text"] = blaineText;
}

void ProcessMimic::createPreheaterView()
{
    scene()->setSceneRect(0, 0, 800, 600);
    
    // Draw cyclone stages
    QPointF positions[] = {
        QPointF(300, 100), QPointF(450, 100),
        QPointF(300, 200), QPointF(450, 200),
        QPointF(300, 300), QPointF(450, 300)
    };
    
    for (int i = 0; i < 6; i++) {
        QGraphicsPolygonItem* cyclone = new QGraphicsPolygonItem();
        QPolygonF triangle;
        triangle << QPointF(0, 0) << QPointF(50, 0) << QPointF(25, 60);
        cyclone->setPolygon(triangle);
        cyclone->setPos(positions[i]);
        cyclone->setBrush(QBrush(QColor(100, 100, 130)));
        cyclone->setPen(QPen(QColor(200, 200, 200), 1));
        cyclone->setData(0, QString("cyclone_%1").arg(i));
        scene()->addItem(cyclone);
        m_graphicsItems[QString("cyclone_%1").arg(i)] = cyclone;
    }
    
    // Draw calciner
    QGraphicsRectItem* calciner = scene()->addRect(350, 400, 100, 120);
    calciner->setBrush(QBrush(QColor(150, 100, 50)));
    calciner->setPen(QPen(QColor(255, 150, 50), 2));
    calciner->setData(0, "calciner");
    m_graphicsItems["calciner"] = calciner;
    
    // Add text displays
    QGraphicsTextItem* tempText = scene()->addText("Preheater Temp: --- °C");
    tempText->setDefaultTextColor(QColor(255, 255, 255));
    tempText->setPos(250, 50);
    m_graphicsItems["preheater_temp_text"] = tempText;
    
    QGraphicsTextItem* calcinerText = scene()->addText("Calciner Temp: --- °C");
    calcinerText->setDefaultTextColor(QColor(255, 255, 255));
    calcinerText->setPos(350, 540);
    m_graphicsItems["calciner_temp_text"] = calcinerText;
}

void ProcessMimic::createCoolerView()
{
    scene()->setSceneRect(0, 0, 800, 600);
    
    // Draw grate cooler
    QGraphicsRectItem* grate = scene()->addRect(200, 250, 400, 60);
    grate->setBrush(QBrush(QColor(100, 100, 120)));
    grate->setPen(QPen(QColor(200, 200, 200), 2));
    grate->setData(0, "cooler");
    m_graphicsItems["grate"] = grate;
    
    // Draw fans
    for (int i = 0; i < 3; i++) {
        QGraphicsEllipseItem* fan = scene()->addEllipse(250 + i * 100, 320, 40, 40);
        fan->setBrush(QBrush(QColor(80, 80, 100)));
        fan->setPen(QPen(QColor(150, 150, 200), 1));
        fan->setData(0, QString("fan_%1").arg(i + 1));
        m_graphicsItems[QString("fan_%1").arg(i + 1)] = fan;
        
        QGraphicsTextItem* fanLabel = scene()->addText(QString("Fan %1").arg(i + 1));
        fanLabel->setDefaultTextColor(QColor(200, 200, 200));
        fanLabel->setPos(260 + i * 100, 365);
    }
    
    // Add temperature text
    QGraphicsTextItem* tempText = scene()->addText("Outlet Temp: --- °C");
    tempText->setDefaultTextColor(QColor(255, 255, 255));
    tempText->setPos(300, 150);
    m_graphicsItems["cooler_temp_text"] = tempText;
    
    // Add clinker temp text
    QGraphicsTextItem* clinkerText = scene()->addText("Clinker Temp: --- °C");
    clinkerText->setDefaultTextColor(QColor(255, 255, 255));
    clinkerText->setPos(300, 180);
    m_graphicsItems["clinker_temp_text"] = clinkerText;
}
