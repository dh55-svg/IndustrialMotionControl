#pragma once
#include <QMainWindow>

class JogPanel;
class StatusPanel;
class MotionManager;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    JogPanel* m_jogPanel;
    StatusPanel* m_statusPanel;
    MotionManager* m_manager;
};
