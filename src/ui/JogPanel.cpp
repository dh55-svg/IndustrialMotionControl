#include "JogPanel.hh"
#include <QPushButton>
#include <QGridLayout>
#include <QSizePolicy>

JogPanel::JogPanel(QWidget *parent) : QWidget(parent) {
    auto* layout = new QGridLayout(this);

    auto* btnXPos = new QPushButton("X +");
    auto* btnXNeg = new QPushButton("X -");
    auto* btnYPos = new QPushButton("Y +");
    auto* btnYNeg = new QPushButton("Y -");
    auto* btnZPos = new QPushButton("Z +");
    auto* btnZNeg = new QPushButton("Z -");
    
    auto* btnHomeX = new QPushButton("X Home");
    auto* btnHomeY = new QPushButton("Y Home");
    auto* btnHomeZ = new QPushButton("Z Home"); // 修复：改为局部变量定义
    auto* btnEstop = new QPushButton("E-STOP");

    QString jogStyle = "QPushButton { font-size: 16px; padding: 15px; }";
    btnXPos->setStyleSheet(jogStyle); btnXNeg->setStyleSheet(jogStyle);
    btnYPos->setStyleSheet(jogStyle); btnYNeg->setStyleSheet(jogStyle);
    btnZPos->setStyleSheet(jogStyle); btnZNeg->setStyleSheet(jogStyle);
    
    btnEstop->setStyleSheet("QPushButton { background-color: red; color: white; font-size: 20px; font-weight: bold; padding: 20px; border-radius: 10px; }");
    btnEstop->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    layout->addWidget(btnYPos, 0, 1);
    layout->addWidget(btnXNeg, 1, 0);
    layout->addWidget(btnEstop, 1, 1);
    layout->addWidget(btnXPos, 1, 2);
    layout->addWidget(btnYNeg, 2, 1);
    
    layout->addWidget(btnZPos, 3, 0, 1, 2);
    layout->addWidget(btnZNeg, 3, 2, 1, 2);

    layout->addWidget(btnHomeX, 4, 0);
    layout->addWidget(btnHomeY, 4, 1);
    layout->addWidget(btnHomeZ, 4, 2); // 修复：直接传入局部变量

    // 工业点动逻辑：按下走，松开停
    auto bindJog = [&](QPushButton* btnPos, QPushButton* btnNeg, Axis axis) {
        connect(btnPos, &QPushButton::pressed, this, [this, axis](){ emit jogRequested(axis, true); });
        connect(btnPos, &QPushButton::released, this, [this, axis](){ emit stopJogRequested(axis); });
        connect(btnNeg, &QPushButton::pressed, this, [this, axis](){ emit jogRequested(axis, false); });
        connect(btnNeg, &QPushButton::released, this, [this, axis](){ emit stopJogRequested(axis); });
    };

    bindJog(btnXPos, btnXNeg, Axis::X);
    bindJog(btnYPos, btnYNeg, Axis::Y);
    bindJog(btnZPos, btnZNeg, Axis::Z);

    connect(btnHomeX, &QPushButton::clicked, this, [this](){ emit homeRequested(Axis::X); });
    connect(btnHomeY, &QPushButton::clicked, this, [this](){ emit homeRequested(Axis::Y); });
    connect(btnHomeZ, &QPushButton::clicked, this, [this](){ emit homeRequested(Axis::Z); }); // 修复

    connect(btnEstop, &QPushButton::clicked, this, &JogPanel::estopRequested);
}
