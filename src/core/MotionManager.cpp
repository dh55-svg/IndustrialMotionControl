#include "MotionManager.hh"
#include <QDebug>

MotionManager::MotionManager(IMotionController* hal, QObject* parent) 
    : QObject(parent), m_hal(hal) {
    
    m_workerThread = new QThread(this);
    m_pollTimer = new QTimer(); 
    m_pollTimer->setInterval(10);

    // 将定时器和管理器本身移动到工作线程
    m_pollTimer->moveToThread(m_workerThread);
    this->moveToThread(m_workerThread);

    // 线程启动逻辑
    connect(m_workerThread, &QThread::started, this, [this]() {
        if(m_hal->open()) {
            m_pollTimer->start(); 
        } else {
            qCritical() << "Failed to open motion hardware";
        }
    });

    // 线程结束清理
    connect(m_workerThread, &QThread::finished, m_pollTimer, &QTimer::deleteLater);

    // 轮询硬件
    connect(m_pollTimer, &QTimer::timeout, this, &MotionManager::pollHardwareStatus);

    // 转发底层信号到 UI 线程
    connect(m_hal, &IMotionController::positionUpdated, this, &MotionManager::axisPositionUpdated);
    connect(m_hal, &IMotionController::stateChanged, this, &MotionManager::axisStateChanged);
    connect(m_hal, &IMotionController::errorOccurred, this, [this](Axis, int err, const QString& msg){
        emit errorRaised(QString("Error %1: %2").arg(err).arg(msg));
    });

    m_workerThread->start();
}

MotionManager::~MotionManager() {
    m_workerThread->quit();
    m_workerThread->wait();
}

void MotionManager::pollHardwareStatus() {
    // 工业级应用中，这里可以读取限位信号、IO状态等
}

void MotionManager::onEnableAxis(Axis axis, bool enable) { m_hal->enableAxis(axis, enable); }
void MotionManager::onHome(Axis axis) { m_hal->home(axis); }

void MotionManager::onJog(Axis axis, bool positive) {
    double vel = positive ? 50.0 : -50.0; // 默认点动速度 50mm/s
    m_hal->moveJog(axis, vel);
}

void MotionManager::onStopJog(Axis axis) {
    m_hal->moveJog(axis, 0.0);
}

void MotionManager::onEstop() {
    m_hal->emergencyStop();
}
