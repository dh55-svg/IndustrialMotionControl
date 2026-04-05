#include "MainWindow.h"
#include "../ui/JogPanel.hh"
#include "../ui/StatusPanel.hh"
#include <QHBoxLayout>
#include <QMessageBox>

/**
 * @brief 构造函数 - 初始化主窗口
 * 
 * 初始化流程：
 * 1. 创建中央部件和布局
 * 2. 创建UI组件
 * 3. 等待依赖注入设置业务逻辑层
 */
MainWindow::MainWindow(QWidget *parent) 
    : QMainWindow(parent)
    , m_jogPanel(nullptr)
    , m_statusPanel(nullptr)
    , m_manager(nullptr) {
    
    setupUi();
}

/**
 * @brief 初始化UI布局
 */
void MainWindow::setupUi() {
    auto* centralWidget = new QWidget(this);
    auto* mainLayout = new QHBoxLayout(centralWidget);

    m_jogPanel = new JogPanel(this);
    m_statusPanel = new StatusPanel(this);

    mainLayout->addWidget(m_jogPanel, 1);
    mainLayout->addWidget(m_statusPanel, 2);

    setCentralWidget(centralWidget);
    setWindowTitle("Industrial Motion Control System V1.0");
    resize(800, 400);
}

/**
 * @brief 设置运动管理器（依赖注入）
 * 
 * 通过依赖注入设置业务逻辑层，实现UI与业务逻辑的解耦
 */
void MainWindow::setMotionManager(IMotionManager* manager) {
    if (m_manager) {
        disconnect(m_manager, nullptr, this, nullptr);
        disconnect(m_jogPanel, nullptr, m_manager, nullptr);
    }
    
    m_manager = manager;
    
    if (m_manager) {
        connectSignals();
    }
}

/**
 * @brief 连接信号槽
 */
void MainWindow::connectSignals() {
    if (!m_manager || !m_jogPanel || !m_statusPanel) {
        return;
    }
    
    // UI -> Manager 信号连接
    connect(m_jogPanel, &JogPanel::jogRequested, m_manager, &IMotionManager::onJog);
    connect(m_jogPanel, &JogPanel::stopJogRequested, m_manager, &IMotionManager::onStopJog);
    connect(m_jogPanel, &JogPanel::homeRequested, m_manager, &IMotionManager::onHome);
    connect(m_jogPanel, &JogPanel::estopRequested, m_manager, &IMotionManager::onEstop);

    // Manager -> UI 信号连接
    connect(m_manager, &IMotionManager::axisPositionUpdated, 
            m_statusPanel, &StatusPanel::onPositionUpdated);
    connect(m_manager, &IMotionManager::axisStateChanged, 
            m_statusPanel, &StatusPanel::onStateChanged);

    // 错误处理
    connect(m_manager, &IMotionManager::errorRaised, this, [this](QString msg) {
        QMessageBox::critical(this, "Hardware Error", msg);
    });
    
    // 限位触发
    connect(m_manager, &IMotionManager::limitTriggered, this, [this](Axis axis, LimitStatus status) {
        QString msg = QString("轴%1触发限位: %2")
            .arg(static_cast<int>(axis))
            .arg(static_cast<int>(status));
        QMessageBox::warning(this, "Limit Triggered", msg);
    });
    
    // 急停状态变化
    connect(m_manager, &IMotionManager::estopChanged, this, [this](bool active) {
        if (active) {
            QMessageBox::critical(this, "Emergency Stop", "急停已激活！");
        }
    });
}

/**
 * @brief 析构函数
 */
MainWindow::~MainWindow() {}
