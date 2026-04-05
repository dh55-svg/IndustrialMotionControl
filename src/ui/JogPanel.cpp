#include "JogPanel.hh"
#include <QPushButton>
#include <QGridLayout>
#include <QSizePolicy>

/**
 * @brief 构造函数 - 创建点动控制面板UI
 * 
 * 布局结构：
 * ┌─────────┬─────────┬─────────┐
 * │         │  Y +    │         │
 * ├─────────┼─────────┼─────────┤
 * │  X -    │ E-STOP  │  X +    │
 * ├─────────┼─────────┼─────────┤
 * │         │  Y -    │         │
 * ├─────────┴─────────┴─────────┤
 * │     Z +      │     Z -      │
 * ├──────────────┼──────────────┤
 * │ X Home│Y Home│ Z Home       │
 * └──────────────┴──────────────┘
 */
JogPanel::JogPanel(QWidget *parent) : QWidget(parent) {
    auto* layout = new QGridLayout(this);

    // ========== 创建点动按钮 ==========
    auto* btnXPos = new QPushButton("X +");
    auto* btnXNeg = new QPushButton("X -");
    auto* btnYPos = new QPushButton("Y +");
    auto* btnYNeg = new QPushButton("Y -");
    auto* btnZPos = new QPushButton("Z +");
    auto* btnZNeg = new QPushButton("Z -");
    
    // ========== 创建回零按钮 ==========
    auto* btnHomeX = new QPushButton("X Home");
    auto* btnHomeY = new QPushButton("Y Home");
    auto* btnHomeZ = new QPushButton("Z Home");
    
    // ========== 创建急停按钮 ==========
    auto* btnEstop = new QPushButton("E-STOP");

    // ========== 设置按钮样式 ==========
    // 点动按钮样式
    QString jogStyle = "QPushButton { font-size: 16px; padding: 15px; }";
    btnXPos->setStyleSheet(jogStyle); btnXNeg->setStyleSheet(jogStyle);
    btnYPos->setStyleSheet(jogStyle); btnYNeg->setStyleSheet(jogStyle);
    btnZPos->setStyleSheet(jogStyle); btnZNeg->setStyleSheet(jogStyle);
    
    // 急停按钮样式（红色背景，白色文字）
    btnEstop->setStyleSheet("QPushButton { background-color: red; color: white; font-size: 20px; font-weight: bold; padding: 20px; border-radius: 10px; }");
    btnEstop->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // ========== 布局排列 ==========
    // 第一行：Y+
    layout->addWidget(btnYPos, 0, 1);
    // 第二行：X-、急停、X+
    layout->addWidget(btnXNeg, 1, 0);
    layout->addWidget(btnEstop, 1, 1);
    layout->addWidget(btnXPos, 1, 2);
    // 第三行：Y-
    layout->addWidget(btnYNeg, 2, 1);
    // 第四行：Z+、Z-
    layout->addWidget(btnZPos, 3, 0, 1, 2);
    layout->addWidget(btnZNeg, 3, 2, 1, 2);
    // 第五行：回零按钮
    layout->addWidget(btnHomeX, 4, 0);
    layout->addWidget(btnHomeY, 4, 1);
    layout->addWidget(btnHomeZ, 4, 2);

    // ========== 绑定点动按钮事件 ==========
    // 工业点动逻辑：按下开始运动，松开停止
    auto bindJog = [&](QPushButton* btnPos, QPushButton* btnNeg, Axis axis) {
        // 正向按钮：按下触发正向点动，松开停止
        connect(btnPos, &QPushButton::pressed, this, [this, axis]() { 
            emit jogRequested(axis, true); 
        });
        connect(btnPos, &QPushButton::released, this, [this, axis]() { 
            emit stopJogRequested(axis); 
        });
        // 反向按钮：按下触发反向点动，松开停止
        connect(btnNeg, &QPushButton::pressed, this, [this, axis]() { 
            emit jogRequested(axis, false); 
        });
        connect(btnNeg, &QPushButton::released, this, [this, axis]() { 
            emit stopJogRequested(axis); 
        });
    };

    // 绑定三轴点动按钮
    bindJog(btnXPos, btnXNeg, Axis::X);
    bindJog(btnYPos, btnYNeg, Axis::Y);
    bindJog(btnZPos, btnZNeg, Axis::Z);

    // ========== 绑定回零按钮事件 ==========
    connect(btnHomeX, &QPushButton::clicked, this, [this]() { emit homeRequested(Axis::X); });
    connect(btnHomeY, &QPushButton::clicked, this, [this]() { emit homeRequested(Axis::Y); });
    connect(btnHomeZ, &QPushButton::clicked, this, [this]() { emit homeRequested(Axis::Z); });

    // ========== 绑定急停按钮事件 ==========
    connect(btnEstop, &QPushButton::clicked, this, &JogPanel::estopRequested);
}
