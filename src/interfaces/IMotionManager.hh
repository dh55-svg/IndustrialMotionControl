#pragma once
#include <QObject>
#include "../hal/IMotionController.hh"
#include "IMotionConfig.hh"
#include "ILogger.hh"

/**
 * @brief 运动管理器接口 - 抽象业务逻辑层
 * 
 * 设计目的：
 * - 解耦UI层与业务逻辑层的直接依赖
 * - 支持依赖注入，便于单元测试和替换实现
 * - 定义业务逻辑层的标准接口
 * 
 * 实现类：
 * - MotionManager: 标准实现
 * - MockMotionManager: 测试用Mock实现
 * 
 * 使用方式：
 * - UI层通过此接口调用业务逻辑
 * - 通过依赖注入获取具体实现
 */
class IMotionManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针
     */
    explicit IMotionManager(QObject* parent = nullptr) : QObject(parent) {}
    
    virtual ~IMotionManager() = default;

    // ============ 配置接口 ============
    
    /**
     * @brief 设置运动配置
     * @param config 运动配置接口指针
     */
    virtual void setConfig(IMotionConfig* config) = 0;
    
    /**
     * @brief 获取当前配置
     * @return 运动配置接口指针
     */
    virtual IMotionConfig* getConfig() const = 0;
    
    /**
     * @brief 设置日志器
     * @param logger 日志器接口指针
     */
    virtual void setLogger(ILogger* logger) = 0;

    // ============ 运动控制槽函数 ============
    
public slots:
    /**
     * @brief 使能/失能轴
     */
    virtual void onEnableAxis(Axis axis, bool enable) = 0;
    
    /**
     * @brief 轴回零
     */
    virtual void onHome(Axis axis) = 0;
    
    /**
     * @brief 点动运动
     */
    virtual void onJog(Axis axis, bool positive) = 0;
    
    /**
     * @brief 停止点动
     */
    virtual void onStopJog(Axis axis) = 0;
    
    /**
     * @brief 紧急停止
     */
    virtual void onEstop() = 0;
    
    /**
     * @brief 急停复位
     */
    virtual void onResetEstop() = 0;
    
    /**
     * @brief 清除报警
     */
    virtual void onClearAlarm(Axis axis) = 0;

    // ============ 状态通知信号 ============
    
signals:
    /**
     * @brief 轴位置更新信号
     */
    void axisPositionUpdated(Axis axis, double pos);
    
    /**
     * @brief 轴状态变化信号
     */
    void axisStateChanged(Axis axis, MotionState state);
    
    /**
     * @brief 错误报告信号
     */
    void errorRaised(QString msg);
    
    /**
     * @brief 限位触发信号
     */
    void limitTriggered(Axis axis, LimitStatus status);
    
    /**
     * @brief 急停状态变化信号
     */
    void estopChanged(bool active);
};
