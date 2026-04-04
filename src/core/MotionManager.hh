#pragma once
#include <QObject>
#include <QThread>
#include <QTimer>
#include "../hal/IMotionController.hh"

class MotionManager : public QObject {
    Q_OBJECT
public:
    explicit MotionManager(IMotionController* hal, QObject* parent = nullptr);
    ~MotionManager();

public slots:
    void onEnableAxis(Axis axis, bool enable);
    void onHome(Axis axis);
    void onJog(Axis axis, bool positive);
    void onStopJog(Axis axis);
    void onEstop();

signals:
    void axisPositionUpdated(Axis axis, double pos);
    void axisStateChanged(Axis axis, MotionState state);
    void errorRaised(QString msg);

private slots:
    void pollHardwareStatus();

private:
    IMotionController* m_hal;
    QThread* m_workerThread;
    QTimer* m_pollTimer;
};
