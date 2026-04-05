#include "StatusPanel.hh"
#include <QFont>

/**
 * @brief 构造函数 - 创建状态显示面板UI
 * 
 * 创建表格形式的状态显示界面：
 * - 表头：Axis | Position(mm) | State
 * - 三行数据：X/Y/Z轴的位置和状态
 */
StatusPanel::StatusPanel(QWidget *parent) : QWidget(parent) {
    m_layout = new QGridLayout(this);
    
    // 设置位置显示字体（等宽字体，便于对齐）
    QFont font("Consolas", 14, QFont::Bold);
    // ========== 创建表头 ==========
    m_layout->addWidget(new QLabel("<b>Axis</b>"), 0, 0);
    m_layout->addWidget(new QLabel("<b>Position (mm)</b>"), 0, 1);
    m_layout->addWidget(new QLabel("<b>State</b>"), 0, 2);

    // ========== X轴状态行 ==========
    m_layout->addWidget(new QLabel("X"), 1, 0);
    m_lblPosX = new QLabel("0.000"); 
    m_lblPosX->setFont(font);
    m_lblStateX = new QLabel("IDLE");
    m_layout->addWidget(m_lblPosX, 1, 1);
    m_layout->addWidget(m_lblStateX, 1, 2);

    // ========== Y轴状态行 ==========
    m_layout->addWidget(new QLabel("Y"), 2, 0);
    m_lblPosY = new QLabel("0.000"); 
    m_lblPosY->setFont(font);
    m_lblStateY = new QLabel("IDLE");
    m_layout->addWidget(m_lblPosY, 2, 1);
    m_layout->addWidget(m_lblStateY, 2, 2);

    // ========== Z轴状态行 ==========
    m_layout->addWidget(new QLabel("Z"), 3, 0);
    m_lblPosZ = new QLabel("0.000"); 
    m_lblPosZ->setFont(font);
    m_lblStateZ = new QLabel("IDLE");
    m_layout->addWidget(m_lblPosZ, 3, 1);
    m_layout->addWidget(m_lblStateZ, 3, 2);

    // 设置位置列自动拉伸
    m_layout->setColumnStretch(1, 1);
}

/**
 * @brief 位置更新槽函数 - 更新指定轴的位置显示
 * @param axis 目标轴
 * @param pos 新位置 (mm)
 */
void StatusPanel::onPositionUpdated(Axis axis, double pos) {
    // 格式化位置字符串（10字符宽度，3位小数）
    QString text = QString("%1").arg(pos, 10, 'f', 3);
    
    switch (axis) {
        case Axis::X: m_lblPosX->setText(text); break;
        case Axis::Y: m_lblPosY->setText(text); break;
        case Axis::Z: m_lblPosZ->setText(text); break;
    }
}

/**
 * @brief 状态更新槽函数 - 更新指定轴的状态显示
 * @param axis 目标轴
 * @param state 新状态
 * 
 * 状态颜色编码：
 * - IDLE：默认颜色
 * - MOVING：绿色
 * - HOMING：橙色
 * - ESTOP：红色加粗
 * - ALARM：红色
 */
void StatusPanel::onStateChanged(Axis axis, MotionState state) {
    QString stateStr;
    
    // 根据状态设置显示文本和颜色
    switch (state) {
        case MotionState::IDLE: 
            stateStr = "IDLE"; 
            break;
        case MotionState::MOVING: 
            stateStr = "<span style='color:green'>MOVING</span>"; 
            break;
        case MotionState::HOMING: 
            stateStr = "<span style='color:orange'>HOMING</span>"; 
            break;
        case MotionState::ESTOP: 
            stateStr = "<span style='color:red'><b>ESTOP</b></span>"; 
            break;
        case MotionState::ALARM: 
            stateStr = "<span style='color:red'>ALARM</span>"; 
            break;
    }
    
    // 更新对应轴的状态标签
    switch (axis) {
        case Axis::X: m_lblStateX->setText(stateStr); break;
        case Axis::Y: m_lblStateY->setText(stateStr); break;
        case Axis::Z: m_lblStateZ->setText(stateStr); break;
    }
}
