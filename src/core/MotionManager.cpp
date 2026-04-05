#include "MotionManager.hh"
#include "Logger.hh"
#include <QDebug>

/**
 * @brief 默认构造函数 - 用于依赖注入
 */
MotionManager::MotionManager(QObject* parent) 
    : IMotionManager(parent)
    , m_hal(nullptr)
    , m_factory(nullptr)
    , m_workerThread(nullptr)
    , m_pollTimer(nullptr)
    , m_config(nullptr)
    , m_logger(LoggerService::instance())
    , m_ownController(false) {
}

/**
 * @brief 兼容构造函数 - 直接传入控制器
 */
MotionManager::MotionManager(IMotionController* hal, QObject* parent) 
    : IMotionManager(parent)
    , m_hal(hal)
    , m_factory(nullptr)
    , m_config(nullptr)
    , m_logger(LoggerService::instance())
    , m_ownController(false) {
    
    m_workerThread = new QThread(this);
    m_pollTimer = new QTimer();
    m_pollTimer->setInterval(10);

    m_pollTimer->moveToThread(m_workerThread);
    this->moveToThread(m_workerThread);

    connect(m_workerThread, &QThread::started, this, [this]() {
        if (m_hal && m_hal->open()) {
            m_pollTimer->start();
            logInfo("运动管理器启动成功");
        } else {
            logError("运动硬件打开失败");
            emit errorRaised("无法打开运动硬件");
        }
    });

    connect(m_workerThread, &QThread::finished, m_pollTimer, &QTimer::deleteLater);
    if (m_hal) {
        connect(m_workerThread, &QThread::finished, m_hal, &IMotionController::close);
    }
    connect(m_pollTimer, &QTimer::timeout, this, &MotionManager::pollHardwareStatus);

    if (m_hal) {
        connect(m_hal, &IMotionController::positionUpdated, this, &MotionManager::axisPositionUpdated);
        connect(m_hal, &IMotionController::stateChanged, this, &MotionManager::axisStateChanged);
        connect(m_hal, &IMotionController::errorOccurred, this, &MotionManager::onHalError);
        connect(m_hal, &IMotionController::limitTriggered, this, &MotionManager::onHalLimitTriggered);
        connect(m_hal, &IMotionController::estopChanged, this, &MotionManager::onHalEstopChanged);
    }

    m_workerThread->start();
}

/**
 * @brief 析构函数 - 清理资源
 */
MotionManager::~MotionManager() {
    logInfo("运动管理器关闭中...");
    
    if (m_pollTimer) {
        m_pollTimer->stop();
    }
    
    if (m_workerThread) {
        m_workerThread->quit();
        m_workerThread->wait();
    }
    
    if (m_ownController && m_hal) {
        delete m_hal;
        m_hal = nullptr;
    }
    
    logInfo("运动管理器已关闭");
}

/**
 * @brief 设置控制器工厂
 */
void MotionManager::setControllerFactory(IControllerFactory* factory) {
    m_factory = factory;
}

/**
 * @brief 初始化控制器（使用工厂创建）
 */
bool MotionManager::initController() {
    if (!m_factory) {
        logError("控制器工厂未设置");
        return false;
    }
    
    if (m_hal) {
        if (m_ownController) {
            delete m_hal;
        }
        m_hal = nullptr;
    }
    
    m_hal = m_factory->createController(this);
    m_ownController = true;
    
    if (!m_hal) {
        logError("控制器创建失败");
        return false;
    }
    
    m_workerThread = new QThread(this);
    m_pollTimer = new QTimer();
    m_pollTimer->setInterval(m_config ? m_config->getPollInterval() : 10);

    m_pollTimer->moveToThread(m_workerThread);
    this->moveToThread(m_workerThread);

    connect(m_workerThread, &QThread::started, this, [this]() {
        if (m_hal->open()) {
            m_pollTimer->start();
            logInfo("运动管理器启动成功");
        } else {
            logError("运动硬件打开失败");
            emit errorRaised("无法打开运动硬件");
        }
    });

    connect(m_workerThread, &QThread::finished, m_pollTimer, &QTimer::deleteLater);
    connect(m_workerThread, &QThread::finished, m_hal, &IMotionController::close);
    connect(m_pollTimer, &QTimer::timeout, this, &MotionManager::pollHardwareStatus);

    connect(m_hal, &IMotionController::positionUpdated, this, &MotionManager::axisPositionUpdated);
    connect(m_hal, &IMotionController::stateChanged, this, &MotionManager::axisStateChanged);
    connect(m_hal, &IMotionController::errorOccurred, this, &MotionManager::onHalError);
    connect(m_hal, &IMotionController::limitTriggered, this, &MotionManager::onHalLimitTriggered);
    connect(m_hal, &IMotionController::estopChanged, this, &MotionManager::onHalEstopChanged);

    m_workerThread->start();
    
    return true;
}

/**
 * @brief 设置运动配置
 */
void MotionManager::setConfig(IMotionConfig* config) {
    m_config = config;
    if (m_pollTimer && m_config) {
        m_pollTimer->setInterval(m_config->getPollInterval());
    }
    logInfo("运动配置已更新");
}

/**
 * @brief 轮询硬件状态
 */
void MotionManager::pollHardwareStatus() {
}

/**
 * @brief 处理HAL层错误
 */
void MotionManager::onHalError(Axis axis, int errorCode, const QString& msg) {
    QString fullMsg = QString("轴%1错误[%2]: %3")
        .arg(static_cast<int>(axis))
        .arg(errorCode)
        .arg(msg);
    
    logError(fullMsg);
    emit errorRaised(fullMsg);
}

/**
 * @brief 处理限位触发
 */
void MotionManager::onHalLimitTriggered(Axis axis, LimitStatus status) {
    logWarning(QString("轴%1触发限位: %2")
        .arg(static_cast<int>(axis))
        .arg(static_cast<int>(status)));
    emit limitTriggered(axis, status);
}

/**
 * @brief 处理急停状态变化
 */
void MotionManager::onHalEstopChanged(bool active) {
    if (active) {
        logError("急停激活");
    } else {
        logInfo("急停已复位");
    }
    emit estopChanged(active);
}

/**
 * @brief 使能/失能轴
 */
void MotionManager::onEnableAxis(Axis axis, bool enable) { 
    if (!m_hal) {
        emit errorRaised("控制器未初始化");
        return;
    }
    
    auto result = m_hal->enableAxis(axis, enable);
    if (!result.success) {
        logWarning(QString("轴%1使能失败: %2")
            .arg(static_cast<int>(axis)).arg(result.message));
        emit errorRaised(result.message);
    }
}

/**
 * @brief 轴回零
 */
void MotionManager::onHome(Axis axis) { 
    if (!m_hal) {
        emit errorRaised("控制器未初始化");
        return;
    }
    
    auto result = m_hal->home(axis);
    if (!result.success) {
        logWarning(QString("轴%1回零失败: %2")
            .arg(static_cast<int>(axis)).arg(result.message));
        emit errorRaised(result.message);
    }
}

/**
 * @brief 点动运动
 */
void MotionManager::onJog(Axis axis, bool positive) {
    if (!m_hal) {
        emit errorRaised("控制器未初始化");
        return;
    }
    
    double jogVel = 50.0;
    if (m_config) {
        int axisIndex = static_cast<int>(axis);
        jogVel = m_config->getAxisConfig(axisIndex).jogVelocity;
    }
    
    double vel = positive ? jogVel : -jogVel;
    
    auto result = m_hal->moveJog(axis, vel);
    if (!result.success) {
        logWarning(QString("轴%1点动失败: %2")
            .arg(static_cast<int>(axis)).arg(result.message));
        emit errorRaised(result.message);
    }
}

/**
 * @brief 停止点动
 */
void MotionManager::onStopJog(Axis axis) {
    if (!m_hal) {
        emit errorRaised("控制器未初始化");
        return;
    }
    
    auto result = m_hal->moveJog(axis, 0.0);
    if (!result.success) {
        logWarning(QString("轴%1停止失败: %2")
            .arg(static_cast<int>(axis)).arg(result.message));
        emit errorRaised(result.message);
    }
}

/**
 * @brief 紧急停止
 */
void MotionManager::onEstop() {
    if (!m_hal) {
        emit errorRaised("控制器未初始化");
        return;
    }
    
    auto result = m_hal->emergencyStop();
    if (!result.success) {
        logError("急停执行失败: " + result.message);
        emit errorRaised(result.message);
    }
}

/**
 * @brief 急停复位
 */
void MotionManager::onResetEstop() {
    if (!m_hal) {
        emit errorRaised("控制器未初始化");
        return;
    }
    
    auto result = m_hal->resetEstop();
    if (!result.success) {
        logWarning("急停复位失败: " + result.message);
        emit errorRaised(result.message);
    }
}

/**
 * @brief 清除报警
 */
void MotionManager::onClearAlarm(Axis axis) {
    if (!m_hal) {
        emit errorRaised("控制器未初始化");
        return;
    }
    
    auto result = m_hal->clearAlarm(axis);
    if (!result.success) {
        logWarning(QString("轴%1清除报警失败: %2")
            .arg(static_cast<int>(axis)).arg(result.message));
        emit errorRaised(result.message);
    }
}

/**
 * @brief 记录信息日志
 */
void MotionManager::logInfo(const QString& msg) {
    if (m_logger) {
        m_logger->info(msg, "MotionManager");
    }
}

/**
 * @brief 记录错误日志
 */
void MotionManager::logError(const QString& msg) {
    if (m_logger) {
        m_logger->error(msg, "MotionManager");
    }
}

/**
 * @brief 记录警告日志
 */
void MotionManager::logWarning(const QString& msg) {
    if (m_logger) {
        m_logger->warning(msg, "MotionManager");
    }
}
