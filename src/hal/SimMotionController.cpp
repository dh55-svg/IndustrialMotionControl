#include "SimMotionController.hh"
#include "IMotionController.hh"
#include <QDebug>
SimMotionController::SimMotionController(QObject* parent) : IMotionController(parent){
    m_simTimer = new QTimer(this);
    m_simTimer->setInterval(10); // 10ms 工业级控制周期

    for(int i=0;i<3;i++)
    {
        Axis axis=static_cast<Axis>(i);
        m_states[axis].axis=axis;
        m_states[axis].position = 0.0;
        m_states[axis].state = MotionState::IDLE;
    }
    connect(m_simTimer,&QTimer::timeout, this, &SimMotionController::updateSimulation);

}
bool SimMotionController::open() {
    m_states[Axis::X].isEnabled = true;
    m_states[Axis::Y].isEnabled = true;
    m_states[Axis::Z].isEnabled = true;
    m_simTimer->start();
    qDebug() << "Simulated motion card opened.";
    return true;
}
void SimMotionController::close() { m_simTimer->stop(); }
bool SimMotionController::enableAxis(Axis axis, bool enable) {
    if (m_states[axis].state == MotionState::ESTOP) return false;
    m_states[axis].isEnabled = enable;
    return true;
}
bool SimMotionController::home(Axis axis) {
    if (!m_states[axis].isEnabled || m_states[axis].state == MotionState::ESTOP) return false;
    m_states[axis].state = MotionState::HOMING;
    m_states[axis].position = 0.0; // 模拟瞬间回零
    m_states[axis].isHomeDone = true;
    m_states[axis].state = MotionState::IDLE;
    emit positionUpdated(axis, 0.0);
    emit stateChanged(axis, MotionState::IDLE);
    return true;
}
bool SimMotionController::moveAbsolute(Axis axis, double targetPos, double velocity) {
    if (!m_states[axis].isEnabled || m_states[axis].state == MotionState::ESTOP) return false;
    m_absTargetPos[axis] = targetPos;
    m_states[axis].state = MotionState::MOVING;
    emit stateChanged(axis, MotionState::MOVING);
    return true;
}
bool SimMotionController::moveJog(Axis axis, double velocity){
     if (!m_states[axis].isEnabled || m_states[axis].state == MotionState::ESTOP) return false;
     m_jogVelocity[axis] = velocity;
    if (velocity != 0.0) {
        m_states[axis].state = MotionState::MOVING;
        emit stateChanged(axis, MotionState::MOVING);
    } else {
        m_states[axis].state = MotionState::IDLE;
        m_states[axis].velocity = 0.0;
        emit stateChanged(axis, MotionState::IDLE);
    }
    return true;
}
bool SimMotionController::stopMove(Axis axis) { return moveJog(axis, 0.0); }
bool SimMotionController::emergencyStop() {
    for (int i = 0; i < 3; ++i) {
        Axis axis = static_cast<Axis>(i);
        m_jogVelocity[axis] = 0.0;
        m_states[axis].velocity = 0.0;
        m_states[axis].state = MotionState::ESTOP;
        emit stateChanged(axis, MotionState::ESTOP);
    }
    return true;
}
AxisStatus SimMotionController::getAxisStatus(Axis axis) { return m_states[axis]; }

void SimMotionController::updateSimulation() {
    // 模拟物理运动：位移 = 速度 * 时间(0.01s)
    for (int i = 0; i < 3; ++i) {
        Axis axis = static_cast<Axis>(i);
        
        if (m_states[axis].state == MotionState::MOVING) {
            double step = m_jogVelocity[axis] * 0.01;
            m_states[axis].position += step;
            m_states[axis].velocity = m_jogVelocity[axis];
            emit positionUpdated(axis, m_states[axis].position);
        }
    }
}