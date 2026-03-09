#pragma once

#include <QWidget>

namespace Ui {
    class operator_panel;
};

class OperatorPanel : public QWidget {
public:
    // Pass a prefix like "KILN_1" or "RAW_MILL" to make the panel reusable
    explicit OperatorPanel( QString unitTitle, QWidget* parent=nullptr, Qt::WindowFlags = Qt::WindowFlags() );
    virtual ~OperatorPanel();

private slots:
    // UI Events
    void handleStart();
    void handleStop();
    void updateValue(const QString &tagName, double value);

private:
    Ui::operator_panel* _UI;
    QString m_prefix;
    void setupStyling();

private:
    Q_OBJECT
};
