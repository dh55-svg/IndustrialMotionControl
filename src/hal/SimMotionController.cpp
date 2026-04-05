#include "SimMotionController.hh"
#include <QDebug>
#include <cmath>

/**
 * @brief 构造函数 - 初始化模拟控制器
 * 
 * 初始化内容：
 * 1. 创建10ms周期的模拟定时器
 * 2. 初始化三轴状态为IDLE
 * 3. 设置默认软限位
 * 4. 连接定时器到模拟更新函数
 */
SimMotionController::SimMotionController(QObject* parent) 
    : IMotionController(parent), m_isOpen(false), m_estopActive(false) {
    
    // 创建模拟定时器，设置10ms周期（工业级控制周期）
    m_simTimer = new QTimer(this);
    m_simTimer->setInterval(10);

    // 初始化三轴状态和软限位
    for (int i = 0; i < 3; i++) {
        Axis axis = static_cast<Axis>(i);
        m_states[axis].axis = axis;
        m_states[axis].position = 0.0;
        m_states[axis].state = MotionState::IDLE;
        m_states[axis].isEnabled = false;
        m_states[axis].isHomeDone = false;
        m_states[axis].limitStatus = LimitStatus::NONE;
        
        // 从配置获取软限位
        const auto& axisConfig = m_config.getAxisConfig(i);
        m_softLimits[axis] = {axisConfig.softLimitNeg, axisConfig.softLimitPos};
    }
    
    // 连接定时器超时信号到模拟更新槽函数
    connect(m_simTimer, &QTimer::timeout, this, &SimMotionController::updateSimulation);
    
    LOG_MODULE_INFO("模拟控制器创建完成", "SimMotionController");
}

/**
 * @brief 析构函数
 */
SimMotionController::~SimMotionController() {
    close();
    LOG_MODULE_INFO("模拟控制器销毁", "SimMotionController");
}

/**
 * @brief 打开设备 - 模拟打开运动控制卡
 * @return true=成功
 */
bool SimMotionController::open() {
    if (m_isOpen) {
        LOG_MODULE_WARNING("设备已打开", "SimMotionController");
        return true;
    }
    
    // 模拟使能所有轴
    m_states[Axis::X].isEnabled = true;
    m_states[Axis::Y].isEnabled = true;
    m_states[Axis::Z].isEnabled = true;
    
    m_isOpen = true;
    m_estopActive = false;
    
    // 启动模拟定时器
    m_simTimer->start();
    
    LOG_MODULE_INFO("模拟运动控制卡已打开", "SimMotionController");
    return true;
}

/**
 * @brief 关闭设备 - 停止模拟
 */
void SimMotionController::close() { 
    if (!m_isOpen) return;
    
    m_simTimer->stop();
    m_isOpen = false;
    
    LOG_MODULE_INFO("模拟运动控制卡已关闭", "SimMotionController");
}

/**
 * @brief 检查设备是否已打开
 */
bool SimMotionController::isOpen() const {
    return m_isOpen;
}

// ============ 安全检查函数 ============

/**
 * @brief 检查运动前安全条件
 */
MotionResult SimMotionController::checkSafetyConditions(Axis axis) {
    // 检查急停状态
    if (m_estopActive) {
        return MotionResult::fail(ErrorCode::ERR_ESTOP_ACTIVE, "急停激活中，禁止运动");
    }
    
    // 检查轴是否使能
    if (!m_states[axis].isEnabled) {
        return MotionResult::fail(ErrorCode::ERR_AXIS_NOT_ENABLED);
    }
    
    // 检查报警状态
    if (m_states[axis].state == MotionState::ALARM) {
        return MotionResult::fail(ErrorCode::ERR_ALARM_ACTIVE);
    }
    
    return MotionResult::ok();
}

/**
 * @brief 检查软限位
 */
LimitStatus SimMotionController::checkSoftLimits(Axis axis, double position) {
    const auto& limits = m_softLimits[axis];
    
    if (position > limits.second) {
        return LimitStatus::POSITIVE_SOFT;
    }
    if (position < limits.first) {
        return LimitStatus::NEGATIVE_SOFT;
    }
    
    return LimitStatus::NONE;
}

/**
 * @brief 验证速度参数
 */
MotionResult SimMotionController::validateVelocity(Axis axis, double velocity) {
    int axisIndex = static_cast<int>(axis);
    const auto& axisConfig = m_config.getAxisConfig(axisIndex);
    
    if (std::abs(velocity) > axisConfig.maxVelocity) {
        return MotionResult::fail(ErrorCode::ERR_INVALID_VELOCITY, 
            QString("速度 %1 超过最大值 %2").arg(velocity).arg(axisConfig.maxVelocity));
    }
    
    return MotionResult::ok();
}

/**
 * @brief 验证位置参数
 */
MotionResult SimMotionController::validatePosition(Axis axis, double position) {
    int axisIndex = static_cast<int>(axis);
    const auto& axisConfig = m_config.getAxisConfig(axisIndex);
    
    if (!axisConfig.isPositionValid(position)) {
        return MotionResult::fail(ErrorCode::ERR_INVALID_POSITION,
            QString("位置 %1 超出范围 [%2, %3]")
                .arg(position).arg(axisConfig.softLimitNeg).arg(axisConfig.softLimitPos));
    }
    
    return MotionResult::ok();
}

/**
 * @brief 设置轴状态并发射信号
 */
void SimMotionController::setAxisState(Axis axis, MotionState state) {
    if (m_states[axis].state != state) {
        m_states[axis].state = state;
        emit stateChanged(axis, state);
    }
}

/**
 * @brief 设置轴错误
 */
void SimMotionController::setAxisError(Axis axis, ErrorCode code, const QString& msg) {
    m_states[axis].errorCode = static_cast<int>(code);
    emit errorOccurred(axis, static_cast<int>(code), msg);
    LOG_MODULE_ERROR(QString("轴%1错误: %2").arg(static_cast<int>(axis)).arg(msg), "SimMotionController");
}

// ============ 轴控制接口实现 ============

/**
 * @brief 使能/失能轴
 */
MotionResult SimMotionController::enableAxis(Axis axis, bool enable) {
    // 急停状态下只能失能，不能使能
    if (enable && m_estopActive) {
        return MotionResult::fail(ErrorCode::ERR_ESTOP_ACTIVE, "急停状态下禁止使能轴");
    }
    
    m_states[axis].isEnabled = enable;
    
    QString msg = enable ? "轴使能" : "轴失能";
    LOG_MODULE_INFO(QString("%1: 轴%2").arg(msg).arg(static_cast<int>(axis)), "SimMotionController");
    
    return MotionResult::ok();
}

/**
 * @brief 轴回零 - 模拟回零过程
 */
MotionResult SimMotionController::home(Axis axis) {
    // 安全检查
    auto safetyResult = checkSafetyConditions(axis);
    if (!safetyResult.success) {
        return safetyResult;
    }
    
    // 模拟回零过程（实际硬件需要移动到原点开关）
    m_states[axis].state = MotionState::HOMING;
    emit stateChanged(axis, MotionState::HOMING);
    
    // 模拟瞬间回零（实际需要时间）
    m_states[axis].position = 0.0;
    m_states[axis].isHomeDone = true;
    m_states[axis].state = MotionState::IDLE;
    m_states[axis].limitStatus = LimitStatus::NONE;
    
    emit positionUpdated(axis, 0.0);
    emit stateChanged(axis, MotionState::IDLE);
    
    LOG_MODULE_INFO(QString("轴%1回零完成").arg(static_cast<int>(axis)), "SimMotionController");
    return MotionResult::ok();
}

/**
 * @brief 绝对定位运动
 */
MotionResult SimMotionController::moveAbsolute(Axis axis, double targetPos, double velocity) {
    // 安全检查
    auto safetyResult = checkSafetyConditions(axis);
    if (!safetyResult.success) {
        return safetyResult;
    }
    
    // 参数校验
    auto velResult = validateVelocity(axis, velocity);
    if (!velResult.success) {
        return velResult;
    }
    
    auto posResult = validatePosition(axis, targetPos);
    if (!posResult.success) {
        return posResult;
    }
    
    // 设置目标位置和速度
    m_absTargetPos[axis] = targetPos;
    m_absVelocity[axis] = velocity;
    setAxisState(axis, MotionState::MOVING);
    
    LOG_MODULE_INFO(QString("轴%1绝对定位: 目标=%2mm, 速度=%3mm/s")
        .arg(static_cast<int>(axis)).arg(targetPos).arg(velocity), "SimMotionController");
    
    return MotionResult::ok();
}

/**
 * @brief 点动运动 - 设置轴的连续运动速度
 */
MotionResult SimMotionController::moveJog(Axis axis, double velocity) {
    // 停止运动（速度为0）时跳过安全检查
    if (velocity != 0.0) {
        // 安全检查
        auto safetyResult = checkSafetyConditions(axis);
        if (!safetyResult.success) {
            return safetyResult;
        }
        
        // 速度校验
        auto velResult = validateVelocity(axis, velocity);
        if (!velResult.success) {
            return velResult;
        }
    }
    
    m_jogVelocity[axis] = velocity;
    
    if (velocity != 0.0) {
        setAxisState(axis, MotionState::MOVING);
        LOG_MODULE_DEBUG(QString("轴%1点动: 速度=%2mm/s")
            .arg(static_cast<int>(axis)).arg(velocity), "SimMotionController");
    } else {
        m_states[axis].velocity = 0.0;
        setAxisState(axis, MotionState::IDLE);
        LOG_MODULE_DEBUG(QString("轴%1点动停止").arg(static_cast<int>(axis)), "SimMotionController");
    }
    
    return MotionResult::ok();
}

/**
 * @brief 停止轴运动
 */
MotionResult SimMotionController::stopMove(Axis axis) { 
    m_jogVelocity[axis] = 0.0;
    m_states[axis].velocity = 0.0;
    setAxisState(axis, MotionState::IDLE);
    
    LOG_MODULE_INFO(QString("轴%1停止").arg(static_cast<int>(axis)), "SimMotionController");
    return MotionResult::ok();
}

/**
 * @brief 紧急停止 - 立即停止所有轴
 */
MotionResult SimMotionController::emergencyStop() {
    m_estopActive = true;
    
    // 遍历所有轴执行急停
    for (int i = 0; i < 3; ++i) {
        Axis axis = static_cast<Axis>(i);
        m_jogVelocity[axis] = 0.0;
        m_states[axis].velocity = 0.0;
        setAxisState(axis, MotionState::ESTOP);
    }
    
    emit estopChanged(true);
    LOG_MODULE_CRITICAL("急停激活！所有轴已停止", "SimMotionController");
    
    return MotionResult::ok();
}

/**
 * @brief 急停复位 - 清除急停状态
 */
MotionResult SimMotionController::resetEstop() {
    if (!m_estopActive) {
        return MotionResult::ok();
    }
    
    m_estopActive = false;
    
    // 将所有轴从ESTOP状态恢复到IDLE
    for (int i = 0; i < 3; ++i) {
        Axis axis = static_cast<Axis>(i);
        setAxisState(axis, MotionState::IDLE);
    }
    
    emit estopChanged(false);
    LOG_MODULE_INFO("急停已复位", "SimMotionController");
    
    return MotionResult::ok();
}

/**
 * @brief 清除报警
 */
MotionResult SimMotionController::clearAlarm(Axis axis) {
    if (m_states[axis].state != MotionState::ALARM) {
        return MotionResult::ok();
    }
    
    m_states[axis].errorCode = 0;
    m_states[axis].limitStatus = LimitStatus::NONE;
    setAxisState(axis, MotionState::IDLE);
    
    LOG_MODULE_INFO(QString("轴%1报警已清除").arg(static_cast<int>(axis)), "SimMotionController");
    return MotionResult::ok();
}

// ============ 状态查询接口实现 ============

AxisStatus SimMotionController::getAxisStatus(Axis axis) const { 
    return m_states[axis]; 
}

bool SimMotionController::isAxisEnabled(Axis axis) const {
    return m_states[axis].isEnabled;
}

bool SimMotionController::isAxisHomed(Axis axis) const {
    return m_states[axis].isHomeDone;
}

bool SimMotionController::isEstopActive() const {
    return m_estopActive;
}

LimitStatus SimMotionController::getLimitStatus(Axis axis) const {
    return m_states[axis].limitStatus;
}

/**
 * @brief 设置软限位
 */
MotionResult SimMotionController::setSoftLimits(Axis axis, double posLimit, double negLimit) {
    if (negLimit >= posLimit) {
        return MotionResult::fail(ErrorCode::ERR_INVALID_PARAM, "负向限位必须小于正向限位");
    }
    
    m_softLimits[axis] = {negLimit, posLimit};
    
    LOG_MODULE_INFO(QString("轴%1软限位设置: [%2, %3]")
        .arg(static_cast<int>(axis)).arg(negLimit).arg(posLimit), "SimMotionController");
    
    return MotionResult::ok();
}

/**
 * @brief 设置运动配置
 */
void SimMotionController::setConfig(const MotionConfig& config) {
    m_config = config;
    
    // 更新软限位
    for (int i = 0; i < 3; ++i) {
        Axis axis = static_cast<Axis>(i);
        const auto& axisConfig = m_config.getAxisConfig(i);
        m_softLimits[axis] = {axisConfig.softLimitNeg, axisConfig.softLimitPos};
    }
    
    LOG_MODULE_INFO("运动配置已更新", "SimMotionController");
}

/**
 * @brief 模拟更新 - 定时器回调函数
 * 
 * 每10ms调用一次，模拟物理运动过程
 * 计算公式：位移增量 = 速度 × 时间间隔(0.01s)
 */
void SimMotionController::updateSimulation() {
    if (!m_isOpen) return;
    
    for (int i = 0; i < 3; ++i) {
        Axis axis = static_cast<Axis>(i);
        
        if (m_states[axis].state == MotionState::MOVING) {
            // 计算位移增量：速度(mm/s) × 时间间隔(0.01s)
            double velocity = m_jogVelocity[axis];
            double step = velocity * 0.01;
            double newPos = m_states[axis].position + step;
            
            // 检查软限位
            LimitStatus limitStatus = checkSoftLimits(axis, newPos);
            if (limitStatus != LimitStatus::NONE) {
                // 触发限位，停止运动
                m_jogVelocity[axis] = 0.0;
                m_states[axis].velocity = 0.0;
                m_states[axis].limitStatus = limitStatus;
                m_states[axis].errorCode = static_cast<int>(
                    limitStatus == LimitStatus::POSITIVE_SOFT ? 
                    ErrorCode::ERR_SOFT_LIMIT_POS : ErrorCode::ERR_SOFT_LIMIT_NEG);
                
                setAxisState(axis, MotionState::ALARM);
                emit limitTriggered(axis, limitStatus);
                
                LOG_MODULE_WARNING(QString("轴%1触发软限位，位置=%2")
                    .arg(static_cast<int>(axis)).arg(newPos), "SimMotionController");
                continue;
            }
            
            // 更新位置
            m_states[axis].position = newPos;
            m_states[axis].velocity = velocity;
            
            // 发射位置更新信号
            emit positionUpdated(axis, m_states[axis].position);
        }
    }
}
