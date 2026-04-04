#pragma once
#include <QObject>
#include <cstdint>

enum class Axis : uint8_t { X = 0, Y = 1, Z = 2 };
enum class MotionState : uint8_t { IDLE, MOVING, HOMING, ESTOP, ALARM };

struct AxisStatus {
    Axis axis;
    double position = 0.0;
    double velocity = 0.0;
    bool isEnabled = false;
    bool isHomeDone = false;
    int errorCode = 0;
    MotionState state = MotionState::IDLE;
};

class IMotionController : public QObject {
    Q_OBJECT
public:
    explicit IMotionController(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~IMotionController() = default;

    virtual bool open() = 0;
    virtual void close() = 0;
    virtual bool enableAxis(Axis axis, bool enable) = 0;
    virtual bool home(Axis axis) = 0;
    virtual bool moveAbsolute(Axis axis, double targetPos, double velocity) = 0;
    virtual bool moveJog(Axis axis, double velocity) = 0;
    virtual bool stopMove(Axis axis) = 0;
    virtual bool emergencyStop() = 0;
    virtual AxisStatus getAxisStatus(Axis axis) = 0;

signals:
    void stateChanged(Axis axis, MotionState state);
    void positionUpdated(Axis axis, double position);
    void errorOccurred(Axis axis, int errorCode, const QString& msg);
};
