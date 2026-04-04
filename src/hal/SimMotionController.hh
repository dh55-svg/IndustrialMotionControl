#pragma once
#include "IMotionController.hh"
#include <QTimer>
#include <QMap>

class SimMotionController : public IMotionController {
    Q_OBJECT
public:
    explicit SimMotionController(QObject* parent = nullptr);
    bool open() override;
    void close() override;
    bool enableAxis(Axis axis, bool enable) override;
    bool home(Axis axis) override;
    bool moveAbsolute(Axis axis, double targetPos, double velocity) override;
    bool moveJog(Axis axis, double velocity) override;
    bool stopMove(Axis axis) override;
    bool emergencyStop() override;
    AxisStatus getAxisStatus(Axis axis) override;

private slots:
    void updateSimulation();

private:
    QTimer* m_simTimer;
    QMap<Axis, AxisStatus> m_states;
    QMap<Axis, double> m_jogVelocity;
    QMap<Axis, double> m_absTargetPos;
};
