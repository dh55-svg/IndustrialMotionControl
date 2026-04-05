#pragma once
#include <QMainWindow>
#include "../interfaces/IMotionManager.hh"

// 前向声明，减少头文件依赖
class JogPanel;
class StatusPanel;

/**
 * @brief 主窗口 - 应用程序的主界面
 * 
 * 职责：
 * 1. 创建和管理UI组件（JogPanel、StatusPanel）
 * 2. 通过依赖注入接收业务逻辑层（IMotionManager）
 * 3. 连接UI信号到业务逻辑槽
 * 4. 连接业务逻辑信号到UI更新槽
 * 
 * 架构位置：
 * - 作为UI层的顶层容器
 * - 协调UI组件与业务逻辑的交互
 * 
 * 依赖注入：
 * - 通过setMotionManager注入业务逻辑
 * - UI层不负责创建业务逻辑对象
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    explicit MainWindow(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~MainWindow();
    
    /**
     * @brief 设置运动管理器（依赖注入）
     * @param manager 运动管理器接口指针
     */
    void setMotionManager(IMotionManager* manager);
    
    /**
     * @brief 获取运动管理器
     * @return 运动管理器接口指针
     */
    IMotionManager* getMotionManager() const { return m_manager; }

private:
    /**
     * @brief 初始化UI布局
     */
    void setupUi();
    
    /**
     * @brief 连接信号槽
     */
    void connectSignals();

private:
    JogPanel* m_jogPanel;           // 点动控制面板
    StatusPanel* m_statusPanel;     // 状态显示面板
    IMotionManager* m_manager;      // 运动管理器接口（业务逻辑）
};
