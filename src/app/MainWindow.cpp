#include "MainWindow.h"
#include "../ui/JogPanel.hh"
#include "../ui/StatusPanel.hh"
#include "../core/MotionManager.hh"
#include "../hal/SimMotionController.hh"
#include <QHBoxLayout>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    auto* centralWidget = new QWidget(this);
    auto* mainLayout = new QHBoxLayout(centralWidget);

    // 1. 实例化底层硬件 (当前为模拟器)
    auto* hal = new SimMotionController(this);
    
    // 2. 启动核心管理器 (内部自动创建后台工作线程)
    m_manager = new MotionManager(hal, this);

    // 3. 实例化 UI
    m_jogPanel = new JogPanel(this);
    m_statusPanel = new StatusPanel(this);

    mainLayout->addWidget(m_jogPanel, 1);
    mainLayout->addWidget(m_statusPanel, 2);

    setCentralWidget(centralWidget);
    setWindowTitle("Industrial Motion Control System V1.0");
    resize(800, 400);

    // 4. 信号连接：UI -> Manager (自动跨线程安全)
    connect(m_jogPanel, &JogPanel::jogRequested, m_manager, &MotionManager::onJog);
    connect(m_jogPanel, &JogPanel::stopJogRequested, m_manager, &MotionManager::onStopJog);
    connect(m_jogPanel, &JogPanel::homeRequested, m_manager, &MotionManager::onHome);
    connect(m_jogPanel, &JogPanel::estopRequested, m_manager, &MotionManager::onEstop);

    // 5. 信号连接：Manager -> UI
    connect(m_manager, &MotionManager::axisPositionUpdated, 
            m_statusPanel, &StatusPanel::onPositionUpdated);
    connect(m_manager, &MotionManager::axisStateChanged, 
            m_statusPanel, &StatusPanel::onStateChanged);

    connect(m_manager, &MotionManager::errorRaised, this, [this](QString msg){
        QMessageBox::critical(this, "Hardware Error", msg);
    });
}

MainWindow::~MainWindow() {}
