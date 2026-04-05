#pragma once
#include "../interfaces/IMotionManager.hh"
#include "../interfaces/IControllerFactory.hh"
#include <QThread>
#include <QTimer>

/**
 * @brief 运动管理器实现 - 业务逻辑核心类
 * 
 * 职责：
 * 1. 管理硬件抽象层(HAL)的生命周期
 * 2. 处理UI层的操作请求
 * 3. 轮询硬件状态并通知UI更新
 * 4. 在独立线程中运行，避免阻塞UI
 * 5. 提供运动参数配置接口
 * 
 * 线程模型：
 * - 运行在独立的工作线程中
 * - 通过Qt信号槽实现线程间安全通信
 * - 定时轮询硬件状态（可配置周期）
 * 
 * 安全机制：
 * - 所有运动操作都经过安全检查
 * - 急停状态下禁止运动
 * - 参数范围校验
 * 
 * 依赖注入：
 * - 通过setControllerFactory注入控制器工厂
 * - 通过setConfig注入配置对象
 * - 通过setLogger注入日志器
 */
class MotionManager : public IMotionManager {
    Q_OBJECT

public:
    /**
     * @brief 默认构造函数 - 用于依赖注入
     * @param parent 父对象
     */
    explicit MotionManager(QObject* parent = nullptr);
    
    /**
     * @brief 兼容构造函数 - 直接传入控制器
     * @param hal 运动控制器接口指针（由外部创建，本类不负责销毁）
     * @param parent 父对象
     */
    explicit MotionManager(IMotionController* hal, QObject* parent = nullptr);
    
    ~MotionManager();

    // ============ 依赖注入接口 ============
    
    /**
     * @brief 设置控制器工厂
     * @param factory 工厂接口指针
     */
    void setControllerFactory(IControllerFactory* factory);
    
    /**
     * @brief 初始化控制器（使用工厂创建）
     * @return true=成功，false=失败
     */
    bool initController();
    
    // ============ IMotionManager接口实现 ============
    
    void setConfig(IMotionConfig* config) override;
    IMotionConfig* getConfig() const override { return m_config; }
    void setLogger(ILogger* logger) override { m_logger = logger; }

    // ============ UI操作请求槽函数 ============
    
public slots:
    void onEnableAxis(Axis axis, bool enable) override;
    void onHome(Axis axis) override;
    void onJog(Axis axis, bool positive) override;
    void onStopJog(Axis axis) override;
    void onEstop() override;
    void onResetEstop() override;
    void onClearAlarm(Axis axis) override;

private slots:
    /**
     * @brief 轮询硬件状态 - 定时器回调
     */
    void pollHardwareStatus();
    
    /**
     * @brief 处理HAL层错误
     */
    void onHalError(Axis axis, int errorCode, const QString& msg);
    
    /**
     * @brief 处理限位触发
     */
    void onHalLimitTriggered(Axis axis, LimitStatus status);
    
    /**
     * @brief 处理急停状态变化
     */
    void onHalEstopChanged(bool active);

private:
    /**
     * @brief 记录日志
     */
    void logInfo(const QString& msg);
    void logError(const QString& msg);
    void logWarning(const QString& msg);

private:
    IMotionController* m_hal;           // 硬件抽象层指针
    IControllerFactory* m_factory;      // 控制器工厂
    QThread* m_workerThread;            // 工作线程
    QTimer* m_pollTimer;                // 状态轮询定时器
    IMotionConfig* m_config;            // 运动配置接口
    ILogger* m_logger;                  // 日志器接口
    bool m_ownController;               // 是否拥有控制器（需要销毁）
};
