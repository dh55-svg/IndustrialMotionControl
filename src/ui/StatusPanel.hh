#pragma once
#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include "../hal/IMotionController.hh"

/**
 * @brief 状态显示面板 - 显示三轴位置和状态信息
 * 
 * 功能：
 * - 实时显示X/Y/Z三轴当前位置 (mm)
 * - 实时显示三轴运动状态 (IDLE/MOVING/HOMING/ESTOP/ALARM)
 * 
 * 显示格式：
 * ┌──────┬─────────────┬─────────┐
 * │ Axis │ Position(mm)│  State  │
 * ├──────┼─────────────┼─────────┤
 * │  X   │   0.000     │  IDLE   │
 * │  Y   │   0.000     │  IDLE   │
 * │  Z   │   0.000     │  IDLE   │
 * └──────┴─────────────┴─────────┘
 */
class StatusPanel : public QWidget {
    Q_OBJECT

public:
    explicit StatusPanel(QWidget *parent = nullptr);

public slots:
    /**
     * @brief 位置更新槽函数
     * @param axis 更新的轴
     * @param pos 新位置 (mm)
     */
    void onPositionUpdated(Axis axis, double pos);
    
    /**
     * @brief 状态更新槽函数
     * @param axis 更新的轴
     * @param state 新状态
     */
    void onStateChanged(Axis axis, MotionState state);

private:
    QGridLayout* m_layout;      // 网格布局
    
    // 位置标签
    QLabel* m_lblPosX;          // X轴位置
    QLabel* m_lblPosY;          // Y轴位置
    QLabel* m_lblPosZ;          // Z轴位置
    
    // 状态标签
    QLabel* m_lblStateX;        // X轴状态
    QLabel* m_lblStateY;        // Y轴状态
    QLabel* m_lblStateZ;        // Z轴状态
};
