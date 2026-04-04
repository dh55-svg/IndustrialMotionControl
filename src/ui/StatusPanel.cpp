#include "StatusPanel.hh"
#include <QFont>

StatusPanel::StatusPanel(QWidget *parent) : QWidget(parent) {
    m_layout = new QGridLayout(this);
    
    QFont font("Consolas", 14, QFont::Bold);

    m_layout->addWidget(new QLabel("<b>Axis</b>"), 0, 0);
    m_layout->addWidget(new QLabel("<b>Position (mm)</b>"), 0, 1);
    m_layout->addWidget(new QLabel("<b>State</b>"), 0, 2);

    // X Axis
    m_layout->addWidget(new QLabel("X"), 1, 0);
    m_lblPosX = new QLabel("0.000"); m_lblPosX->setFont(font);
    m_lblStateX = new QLabel("IDLE");
    m_layout->addWidget(m_lblPosX, 1, 1);
    m_layout->addWidget(m_lblStateX, 1, 2);

    // Y Axis
    m_layout->addWidget(new QLabel("Y"), 2, 0);
    m_lblPosY = new QLabel("0.000"); m_lblPosY->setFont(font);
    m_lblStateY = new QLabel("IDLE");
    m_layout->addWidget(m_lblPosY, 2, 1);
    m_layout->addWidget(m_lblStateY, 2, 2);

    // Z Axis
    m_layout->addWidget(new QLabel("Z"), 3, 0);
    m_lblPosZ = new QLabel("0.000"); m_lblPosZ->setFont(font);
    m_lblStateZ = new QLabel("IDLE");
    m_layout->addWidget(m_lblPosZ, 3, 1);
    m_layout->addWidget(m_lblStateZ, 3, 2);

    m_layout->setColumnStretch(1, 1);
}

void StatusPanel::onPositionUpdated(Axis axis, double pos) {
    QString text = QString("%1").arg(pos, 10, 'f', 3);
    switch (axis) {
        case Axis::X: m_lblPosX->setText(text); break;
        case Axis::Y: m_lblPosY->setText(text); break;
        case Axis::Z: m_lblPosZ->setText(text); break;
    }
}

void StatusPanel::onStateChanged(Axis axis, MotionState state) {
    QString stateStr;
    switch (state) {
        case MotionState::IDLE: stateStr = "IDLE"; break;
        case MotionState::MOVING: stateStr = "<span style='color:green'>MOVING</span>"; break;
        case MotionState::HOMING: stateStr = "<span style='color:orange'>HOMING</span>"; break;
        case MotionState::ESTOP: stateStr = "<span style='color:red'><b>ESTOP</b></span>"; break;
        case MotionState::ALARM: stateStr = "<span style='color:red'>ALARM</span>"; break;
    }
    switch (axis) {
        case Axis::X: m_lblStateX->setText(stateStr); break;
        case Axis::Y: m_lblStateY->setText(stateStr); break;
        case Axis::Z: m_lblStateZ->setText(stateStr); break;
    }
}
