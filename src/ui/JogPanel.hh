#pragma once
#include <QWidget>
#include "../hal/IMotionController.hh"

/**
 * @brief 点动控制面板 - 提供三轴点动操作界面
 * 
 * 功能：
 * - X/Y/Z三轴正反向点动按钮
 * - 三轴回零按钮
 * - 紧急停止按钮
 * 
 * 交互方式：
 * - 点动按钮：按下开始运动，松开停止（工业标准操作方式）
 * - 回零按钮：点击触发回零
 * - 急停按钮：点击触发紧急停止
 */
class JogPanel : public QWidget {
    Q_OBJECT

public:
    explicit JogPanel(QWidget *parent = nullptr);

signals:
    /**
     * @brief 点动请求信号
     * @param axis 目标轴
     * @param positive true=正向，false=反向
     */
    void jogRequested(Axis axis, bool positive);
    
    /**
     * @brief 停止点动信号
     * @param axis 目标轴
     */
    void stopJogRequested(Axis axis);
    
    /**
     * @brief 回零请求信号
     * @param axis 目标轴
     */
    void homeRequested(Axis axis);
    
    /**
     * @brief 急停请求信号
     */
    void estopRequested();
};
