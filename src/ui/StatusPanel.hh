#pragma once
#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include "../hal/IMotionController.hh"

class StatusPanel : public QWidget {
    Q_OBJECT
public:
    explicit StatusPanel(QWidget *parent = nullptr);

public slots:
    void onPositionUpdated(Axis axis, double pos);
    void onStateChanged(Axis axis, MotionState state);

private:
    QGridLayout* m_layout;
    QLabel* m_lblPosX;
    QLabel* m_lblPosY;
    QLabel* m_lblPosZ;
    QLabel* m_lblStateX;
    QLabel* m_lblStateY;
    QLabel* m_lblStateZ;
};
