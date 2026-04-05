/**
 * @file main.cpp
 * @brief 应用程序入口点
 * 
 * 工业运动控制系统 - 主入口
 * 
 * 系统架构（依赖注入版本）：
 * ┌─────────────────────────────────────────────┐
 * │                  UI Layer                    │
 * │  ┌─────────────┐    ┌──────────────────┐   │
 * │  │  JogPanel   │    │   StatusPanel    │   │
 * │  └──────┬──────┘    └────────┬─────────┘   │
 * │         │                    │              │
 * │         └────────┬───────────┘              │
 * │                  ▼                          │
 * │           ┌──────────────┐                  │
 * │           │  MainWindow  │◄── 注入 IMotionManager
 * │           └──────────────┘                  │
 * └─────────────────────────────────────────────┘
 *                    │
 * ┌──────────────────┼──────────────────────────┐
 * │         Business Logic Layer                │
 * │                  ▼                          │
 * │           ┌──────────────┐                  │
 * │           │ MotionManager│◄── 注入 IControllerFactory
 * │           │(IMotionManager)                 │
 * │           └──────────────┘                  │
 * └─────────────────────────────────────────────┘
 *                    │
 * ┌──────────────────┼──────────────────────────┐
 * │      Hardware Abstraction Layer (HAL)       │
 * │                  ▼                          │
 * │        ┌─────────────────────┐              │
 * │        │ IMotionController   │◄── 工厂创建
 * │        │  (Interface)        │              │
 * │        └─────────┬───────────┘              │
 * │            ┌─────┴─────┐                    │
 * │            ▼           ▼                    │
 * │   SimMotionController  RealMotionController │
 * │     (Simulation)         (Hardware)         │
 * └─────────────────────────────────────────────┘
 * 
 * 依赖注入流程：
 * 1. 创建基础设施（日志器、配置）
 * 2. 创建控制器工厂
 * 3. 创建业务逻辑层，注入工厂和配置
 * 4. 创建UI层，注入业务逻辑
 */

#include <QApplication>
#include "app/MainWindow.h"
#include "core/MotionManager.hh"
#include "core/Logger.hh"
#include "core/MotionConfig.hh"
#include "interfaces/IControllerFactory.hh"

/**
 * @brief 应用程序组装器 - 负责依赖注入和对象组装
 * 
 * 职责：
 * - 创建所有依赖对象
 * - 配置依赖关系
 * - 管理对象生命周期
 */
class ApplicationAssembler {
public:
    /**
     * @brief 组装应用程序
     * @return 配置好的MainWindow指针
     */
    MainWindow* assemble() {
        // 1. 创建日志器
        m_logger = new FileLogger();
        m_logger->init("motion_control.log", LogLevel::INFO);
        
        // 2. 创建配置
        m_config = new FileMotionConfig();
        
        // 3. 创建控制器工厂
        m_factory = new SimControllerFactory();
        m_factory->setConfig(m_config);
        m_factory->setLogger(m_logger);
        
        // 4. 创建业务逻辑层
        m_manager = new MotionManager();
        m_manager->setLogger(m_logger);
        m_manager->setConfig(m_config);
        m_manager->setControllerFactory(m_factory);
        m_manager->initController();
        
        // 5. 创建UI层
        m_mainWindow = new MainWindow();
        m_mainWindow->setMotionManager(m_manager);
        
        return m_mainWindow;
    }
    
    /**
     * @brief 清理资源
     */
    ~ApplicationAssembler() {
        delete m_mainWindow;
        delete m_manager;
        delete m_factory;
        delete m_config;
        delete m_logger;
    }

private:
    FileLogger* m_logger = nullptr;
    FileMotionConfig* m_config = nullptr;
    SimControllerFactory* m_factory = nullptr;
    MotionManager* m_manager = nullptr;
    MainWindow* m_mainWindow = nullptr;
};

/**
 * @brief 主函数 - 应用程序入口
 * @param argc 命令行参数个数
 * @param argv 命令行参数数组
 * @return 应用程序退出码
 */
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    
    // 使用组装器创建应用程序
    ApplicationAssembler assembler;
    MainWindow* w = assembler.assemble();
    w->show();
    
    // 进入Qt事件循环
    int result = a.exec();
    
    // assembler析构时自动清理资源
    return result;
}
