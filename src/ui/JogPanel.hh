#pragma once
#include <QWidget>
#include "../hal/IMotionController.hh"

class JogPanel : public QWidget {
    Q_OBJECT
public:
    explicit JogPanel(QWidget *parent = nullptr);

signals:
    void jogRequested(Axis axis, bool positive);
    void stopJogRequested(Axis axis);
    void homeRequested(Axis axis);
    void estopRequested();
};
