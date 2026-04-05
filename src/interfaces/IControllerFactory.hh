#pragma once
#include "../hal/IMotionController.hh"
#include "IMotionConfig.hh"
#include "ILogger.hh"

/**
 * @brief 控制器工厂接口 - 抽象控制器创建逻辑
 * 
 * 设计目的：
 * - 解耦控制器创建逻辑
 * - 支持运行时切换不同的控制器实现
 * - 便于单元测试时注入Mock控制器
 * 
 * 使用方式：
 * - 通过工厂创建控制器实例
 * - 支持配置和日志器的依赖注入
 */
class IControllerFactory {
public:
    virtual ~IControllerFactory() = default;
    
    /**
     * @brief 创建运动控制器
     * @param parent 父对象
     * @return 控制器接口指针
     */
    virtual IMotionController* createController(QObject* parent = nullptr) = 0;
    
    /**
     * @brief 设置控制器配置
     * @param config 配置接口指针
     */
    virtual void setConfig(IMotionConfig* config) = 0;
    
    /**
     * @brief 设置日志器
     * @param logger 日志器接口指针
     */
    virtual void setLogger(ILogger* logger) = 0;
    
    /**
     * @brief 获取工厂类型名称
     * @return 工厂类型名称（如"Simulator", "RealHardware"等）
     */
    virtual QString getFactoryType() const = 0;
};

/**
 * @brief 模拟控制器工厂 - 创建模拟控制器
 * 
 * 注意：SimMotionController的前向声明在头文件中，
 * 实际使用时需要在cpp文件中包含完整头文件
 */
class SimControllerFactory : public IControllerFactory {
public:
    SimControllerFactory() : m_config(nullptr), m_logger(nullptr) {}
    
    IMotionController* createController(QObject* parent = nullptr) override;
    
    void setConfig(IMotionConfig* config) override {
        m_config = config;
    }
    
    void setLogger(ILogger* logger) override {
        m_logger = logger;
    }
    
    QString getFactoryType() const override {
        return "Simulator";
    }

private:
    IMotionConfig* m_config;
    ILogger* m_logger;
};
