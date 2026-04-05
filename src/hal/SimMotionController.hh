#pragma once
#include "IMotionController.hh"
#include "../core/MotionConfig.hh"
#include "../core/Logger.hh"
#include <QTimer>
#include <QMap>

/**
 * @brief 模拟运动控制器 - 用于开发和测试的虚拟运动控制器
 * 
 * 功能：
 * - 模拟三轴运动控制卡的行为
 * - 无需真实硬件即可测试应用程序
 * - 使用定时器模拟物理运动过程
 * - 实现完整的安全机制（软限位、急停互锁）
 * 
 * 安全机制：
 * - 软限位检测：位置超出范围时停止运动
 * - 急停互锁：急停状态下禁止所有运动
 * - 参数校验：速度和位置范围检查
 * 
 * 使用场景：
 * - 开发阶段UI调试
 * - 功能演示
 * - 单元测试
 */
class SimMotionController : public IMotionController {
    Q_OBJECT

public:
    explicit SimMotionController(QObject* parent = nullptr);
    ~SimMotionController() override;
    
    // ============ 重写基类接口 ============
    bool open() override;
    void close() override;
    bool isOpen() const override;
    
    MotionResult enableAxis(Axis axis, bool enable) override;
    MotionResult home(Axis axis) override;
    MotionResult moveAbsolute(Axis axis, double targetPos, double velocity) override;
    MotionResult moveJog(Axis axis, double velocity) override;
    MotionResult stopMove(Axis axis) override;
    MotionResult emergencyStop() override;
    MotionResult resetEstop() override;
    MotionResult clearAlarm(Axis axis) override;
    
    AxisStatus getAxisStatus(Axis axis) const override;
    bool isAxisEnabled(Axis axis) const override;
    bool isAxisHomed(Axis axis) const override;
    bool isEstopActive() const override;
    
    LimitStatus getLimitStatus(Axis axis) const override;
    MotionResult setSoftLimits(Axis axis, double posLimit, double negLimit) override;

    // ============ 配置接口 ============
    
    /**
     * @brief 设置运动配置
     * @param config 运动配置对象
     */
    void setConfig(const MotionConfig& config);
    
    /**
     * @brief 获取当前配置
     * @return 运动配置对象
     */
    const MotionConfig& getConfig() const { return m_config; }

private slots:
    /**
     * @brief 模拟更新 - 定时器回调，模拟物理运动过程
     * 
     * 计算公式：位移 = 速度 × 时间间隔
     * 时间间隔：10ms（工业级控制周期）
     */
    void updateSimulation();

private:
    /**
     * @brief 检查运动前安全条件
     * @param axis 目标轴
     * @return 安全检查结果
     */
    MotionResult checkSafetyConditions(Axis axis);
    
    /**
     * @brief 检查软限位
     * @param axis 目标轴
     * @param position 待检查位置
     * @return 限位状态
     */
    LimitStatus checkSoftLimits(Axis axis, double position);
    
    /**
     * @brief 验证速度参数
     * @param axis 目标轴
     * @param velocity 速度值
     * @return 验证结果
     */
    MotionResult validateVelocity(Axis axis, double velocity);
    
    /**
     * @brief 验证位置参数
     * @param axis 目标轴
     * @param position 位置值
     * @return 验证结果
     */
    MotionResult validatePosition(Axis axis, double position);
    
    /**
     * @brief 设置轴状态并发射信号
     */
    void setAxisState(Axis axis, MotionState state);
    
    /**
     * @brief 设置轴错误
     */
    void setAxisError(Axis axis, ErrorCode code, const QString& msg);

private:
    QTimer* m_simTimer;                    // 模拟定时器（10ms周期）
    QMap<Axis, AxisStatus> m_states;       // 各轴当前状态
    QMap<Axis, double> m_jogVelocity;      // 各轴点动速度
    QMap<Axis, double> m_absTargetPos;     // 各轴绝对定位目标位置
    QMap<Axis, double> m_absVelocity;      // 各轴绝对定位速度
    QMap<Axis, std::pair<double, double>> m_softLimits;  // 各轴软限位
    
    MotionConfig m_config;                 // 运动配置
    bool m_isOpen;                         // 设备打开状态
    bool m_estopActive;                    // 急停激活状态
};
