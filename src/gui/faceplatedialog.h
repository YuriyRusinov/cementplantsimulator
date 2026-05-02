#pragma once

#include <QDialog>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QComboBox>
#include <QPushButton>
#include <QProgressBar>
#include <QTimer>
#include <QSharedPointer>
#include "processmodel.h"

class FaceplateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FaceplateDialog(QSharedPointer<ProcessModel> model, const QString& tag, QWidget *parent = nullptr);
    
    void setTag(const QString& tag);
    void updateDisplay();

private slots:
    void onSetpointChanged(double value);
    void onOutputChanged(int value);
    void onModeChanged(int index);
    void onStartStop();
    void onAcknowledge();

private:
    void setupUI();
    void updateTagInfo();
    
    QSharedPointer<ProcessModel> m_processModel;
    QString m_tag;
    
    // UI Components
    QLabel* m_tagLabel;
    QLabel* m_descriptionLabel;
    QLabel* m_valueLabel;
    QLabel* m_unitsLabel;
    QLabel* m_statusLabel;
    
    QDoubleSpinBox* m_setpointSpin;
    QSlider* m_outputSlider;
    QComboBox* m_modeCombo;
    QPushButton* m_startStopButton;
    QPushButton* mAcknowledgeButton;
    QProgressBar* m_valueBar;
    
    QTimer* m_updateTimer;
};
