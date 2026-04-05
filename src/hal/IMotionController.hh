#pragma once
#include <QObject>
#include <cstdint>
#include "../core/ErrorCodes.hh"

/**
 * @brief 轴枚举 - 定义三轴运动系统的轴编号
 */
enum class Axis : uint8_t { 
    X = 0,  // X轴（水平方向）
    Y = 1,  // Y轴（垂直方向）
    Z = 2   // Z轴（升降方向）
};

/**
 * @brief 运动状态枚举 - 定义轴的运动状态
 */
enum class MotionState : uint8_t { 
    IDLE,   // 空闲 - 轴停止，无运动
    MOVING, // 运动中 - 轴正在移动
    HOMING, // 回零中 - 轴正在执行回零操作
    ESTOP,  // 急停 - 紧急停止状态
    ALARM   // 报警 - 异常状态
};

/**
 * @brief 限位状态枚举
 */
enum class LimitStatus : uint8_t {
    NONE = 0,           // 无限位触发
    POSITIVE_SOFT = 1,  // 正向软限位
    NEGATIVE_SOFT = 2,  // 负向软限位
    POSITIVE_HARD = 3,  // 正向硬限位
    NEGATIVE_HARD = 4   // 负向硬限位
};

/**
 * @brief 轴状态结构体 - 存储单个轴的完整状态信息
 */
struct AxisStatus {
    Axis axis;                      // 轴编号
    double position = 0.0;          // 当前位置 (mm)
    double velocity = 0.0;          // 当前速度 (mm/s)
    bool isEnabled = false;         // 轴使能状态（true=使能，false=失能）
    bool isHomeDone = false;        // 回零完成标志
    int errorCode = 0;              // 错误码（0=无错误）
    MotionState state = MotionState::IDLE;  // 当前运动状态
    LimitStatus limitStatus = LimitStatus::NONE;  // 限位状态
    
    /**
     * @brief 检查轴是否可以运动
     * @return true=可运动，false=不可运动
     */
    bool canMove() const {
        return isEnabled && state != MotionState::ESTOP && state != MotionState::ALARM;
    }
    
    /**
     * @brief 检查是否处于安全状态
     * @return true=安全，false=不安全
     */
    bool isSafe() const {
        return state != MotionState::ESTOP && state != MotionState::ALARM;
    }
};

/**
 * @brief 运动控制器接口 - 抽象基类，定义运动控制的标准接口
 * 
 * 设计模式：策略模式
 * - SimMotionController：模拟控制器，用于开发测试
 * - RealMotionController：真实控制器，连接实际硬件
 * 
 * 安全机制：
 * - 急停互锁：急停状态下禁止所有运动操作
 * - 限位保护：软限位/硬限位检测
 * - 参数校验：位置和速度范围检查
 * 
 * 使用方式：
 * 1. 继承此类实现具体的控制器
 * 2. 重写所有纯虚函数
 * 3. 在适当位置发射信号通知状态变化
 */
class IMotionController : public QObject {
    Q_OBJECT

public:
    explicit IMotionController(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~IMotionController() = default;

    // ============ 设备生命周期管理 ============
    
    /**
     * @brief 打开设备连接
     * @return true=成功，false=失败
     */
    virtual bool open() = 0;
    
    /**
     * @brief 关闭设备连接
     */
    virtual void close() = 0;
    
    /**
     * @brief 检查设备是否已打开
     * @return true=已打开，false=未打开
     */
    virtual bool isOpen() const = 0;

    // ============ 轴控制接口 ============
    
    /**
     * @brief 使能/失能轴
     * @param axis 目标轴
     * @param enable true=使能，false=失能
     * @return 操作结果
     */
    virtual MotionResult enableAxis(Axis axis, bool enable) = 0;
    
    /**
     * @brief 轴回零
     * @param axis 目标轴
     * @return 操作结果
     */
    virtual MotionResult home(Axis axis) = 0;
    
    /**
     * @brief 绝对定位运动
     * @param axis 目标轴
     * @param targetPos 目标位置 (mm)
     * @param velocity 运动速度 (mm/s)
     * @return 操作结果
     */
    virtual MotionResult moveAbsolute(Axis axis, double targetPos, double velocity) = 0;
    
    /**
     * @brief 点动运动（连续运动）
     * @param axis 目标轴
     * @param velocity 运动速度 (mm/s)，正值正向，负值反向，0=停止
     * @return 操作结果
     */
    virtual MotionResult moveJog(Axis axis, double velocity) = 0;
    
    /**
     * @brief 停止轴运动
     * @param axis 目标轴
     * @return 操作结果
     */
    virtual MotionResult stopMove(Axis axis) = 0;
    
    /**
     * @brief 紧急停止 - 立即停止所有轴
     * @return 操作结果
     */
    virtual MotionResult emergencyStop() = 0;
    
    /**
     * @brief 急停复位 - 清除急停状态
     * @return 操作结果
     * @note 急停复位后需要重新使能轴才能运动
     */
    virtual MotionResult resetEstop() = 0;
    
    /**
     * @brief 清除报警
     * @param axis 目标轴
     * @return 操作结果
     */
    virtual MotionResult clearAlarm(Axis axis) = 0;

    // ============ 状态查询接口 ============
    
    /**
     * @brief 获取轴状态
     * @param axis 目标轴
     * @return 轴状态结构体
     */
    virtual AxisStatus getAxisStatus(Axis axis) const = 0;
    
    /**
     * @brief 检查轴是否使能
     * @param axis 目标轴
     * @return true=使能，false=失能
     */
    virtual bool isAxisEnabled(Axis axis) const = 0;
    
    /**
     * @brief 检查轴是否已回零
     * @param axis 目标轴
     * @return true=已回零，false=未回零
     */
    virtual bool isAxisHomed(Axis axis) const = 0;
    
    /**
     * @brief 检查是否处于急停状态
     * @return true=急停，false=正常
     */
    virtual bool isEstopActive() const = 0;

    // ============ 安全接口 ============
    
    /**
     * @brief 获取轴的限位状态
     * @param axis 目标轴
     * @return 限位状态
     */
    virtual LimitStatus getLimitStatus(Axis axis) const = 0;
    
    /**
     * @brief 设置软限位
     * @param axis 目标轴
     * @param posLimit 正向限位 (mm)
     * @param negLimit 负向限位 (mm)
     * @return 操作结果
     */
    virtual MotionResult setSoftLimits(Axis axis, double posLimit, double negLimit) = 0;

signals:
    // ============ 状态变化信号 ============
    
    /**
     * @brief 运动状态变化信号
     * @param axis 变化的轴
     * @param state 新状态
     */
    void stateChanged(Axis axis, MotionState state);
    
    /**
     * @brief 位置更新信号
     * @param axis 更新的轴
     * @param position 新位置 (mm)
     */
    void positionUpdated(Axis axis, double position);
    
    /**
     * @brief 错误发生信号
     * @param axis 发生错误的轴
     * @param errorCode 错误码
     * @param msg 错误描述
     */
    void errorOccurred(Axis axis, int errorCode, const QString& msg);
    
    /**
     * @brief 限位触发信号
     * @param axis 触发限位的轴
     * @param status 限位状态
     */
    void limitTriggered(Axis axis, LimitStatus status);
    
    /**
     * @brief 急停状态变化信号
     * @param active true=急停激活，false=急停解除
     */
    void estopChanged(bool active);
};
