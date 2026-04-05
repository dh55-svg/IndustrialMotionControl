#pragma once

#include <QtTest/QtTest>
#include "../hal/SimMotionController.hh"
#include "MockLogger.h"

/**
 * @brief SimMotionController单元测试类
 * 
 * 测试内容：
 * - 设备打开/关闭
 * - 轴使能/失能
 * - 点动运动
 * - 急停功能
 * - 软限位保护
 * - 安全互锁
 */
class TestSimMotionController : public QObject {
    Q_OBJECT

private slots:
    /**
     * @brief 测试初始化
     */
    void initTestCase() {
        m_logger = new MockLogger();
        m_controller = new SimMotionController(this);
    }
    
    /**
     * @brief 测试清理
     */
    void cleanupTestCase() {
        delete m_controller;
        delete m_logger;
    }
    
    /**
     * @brief 每个测试前的准备
     */
    void init() {
        m_controller->close();
        m_controller->open();
        m_logger->clear();
    }
    
    /**
     * @brief 测试设备打开
     */
    void testOpen() {
        m_controller->close();
        QVERIFY(!m_controller->isOpen());
        
        QVERIFY(m_controller->open());
        QVERIFY(m_controller->isOpen());
        
        QVERIFY(m_controller->open());
    }
    
    /**
     * @brief 测试设备关闭
     */
    void testClose() {
        QVERIFY(m_controller->isOpen());
        m_controller->close();
        QVERIFY(!m_controller->isOpen());
    }
    
    /**
     * @brief 测试轴使能
     */
    void testEnableAxis() {
        QVERIFY(m_controller->isAxisEnabled(Axis::X));
        
        auto result = m_controller->enableAxis(Axis::X, false);
        QVERIFY(result.success);
        QVERIFY(!m_controller->isAxisEnabled(Axis::X));
        
        result = m_controller->enableAxis(Axis::X, true);
        QVERIFY(result.success);
        QVERIFY(m_controller->isAxisEnabled(Axis::X));
    }
    
    /**
     * @brief 测试急停状态下禁止使能
     */
    void testEnableAxisDuringEstop() {
        m_controller->emergencyStop();
        QVERIFY(m_controller->isEstopActive());
        
        auto result = m_controller->enableAxis(Axis::X, true);
        QVERIFY(!result.success);
        QCOMPARE(result.errorCode, ErrorCode::ERR_ESTOP_ACTIVE);
        
        m_controller->resetEstop();
        QVERIFY(!m_controller->isEstopActive());
        
        result = m_controller->enableAxis(Axis::X, true);
        QVERIFY(result.success);
    }
    
    /**
     * @brief 测试点动运动
     */
    void testJogMove() {
        auto status = m_controller->getAxisStatus(Axis::X);
        QCOMPARE(status.position, 0.0);
        
        auto result = m_controller->moveJog(Axis::X, 50.0);
        QVERIFY(result.success);
        
        QTest::qWait(100);
        
        status = m_controller->getAxisStatus(Axis::X);
        QVERIFY(status.position > 0.0);
        
        result = m_controller->moveJog(Axis::X, 0.0);
        QVERIFY(result.success);
        
        double stoppedPos = m_controller->getAxisStatus(Axis::X).position;
        
        QTest::qWait(50);
        status = m_controller->getAxisStatus(Axis::X);
        QCOMPARE(status.position, stoppedPos);
    }
    
    /**
     * @brief 测试急停功能
     */
    void testEmergencyStop() {
        m_controller->moveJog(Axis::X, 100.0);
        QTest::qWait(50);
        
        auto result = m_controller->emergencyStop();
        QVERIFY(result.success);
        QVERIFY(m_controller->isEstopActive());
        
        auto statusX = m_controller->getAxisStatus(Axis::X);
        QCOMPARE(statusX.state, MotionState::ESTOP);
        
        auto statusY = m_controller->getAxisStatus(Axis::Y);
        QCOMPARE(statusY.state, MotionState::ESTOP);
    }
    
    /**
     * @brief 测试急停状态下禁止运动
     */
    void testMoveDuringEstop() {
        m_controller->emergencyStop();
        
        auto result = m_controller->moveJog(Axis::X, 50.0);
        QVERIFY(!result.success);
        QCOMPARE(result.errorCode, ErrorCode::ERR_ESTOP_ACTIVE);
        
        result = m_controller->home(Axis::X);
        QVERIFY(!result.success);
    }
    
    /**
     * @brief 测试急停复位
     */
    void testResetEstop() {
        m_controller->emergencyStop();
        QVERIFY(m_controller->isEstopActive());
        
        auto result = m_controller->resetEstop();
        QVERIFY(result.success);
        QVERIFY(!m_controller->isEstopActive());
        
        auto status = m_controller->getAxisStatus(Axis::X);
        QCOMPARE(status.state, MotionState::IDLE);
    }
    
    /**
     * @brief 测试回零功能
     */
    void testHome() {
        m_controller->moveJog(Axis::X, 100.0);
        QTest::qWait(100);
        
        auto status = m_controller->getAxisStatus(Axis::X);
        QVERIFY(status.position != 0.0);
        QVERIFY(!status.isHomeDone);
        
        m_controller->moveJog(Axis::X, 0.0);
        auto result = m_controller->home(Axis::X);
        QVERIFY(result.success);
        
        status = m_controller->getAxisStatus(Axis::X);
        QCOMPARE(status.position, 0.0);
        QVERIFY(status.isHomeDone);
    }
    
    /**
     * @brief 测试软限位保护
     */
    void testSoftLimit() {
        m_controller->setSoftLimits(Axis::X, 10.0, -10.0);
        
        m_controller->moveJog(Axis::X, 100.0);
        
        QTest::qWait(200);
        
        auto status = m_controller->getAxisStatus(Axis::X);
        
        QVERIFY(status.limitStatus == LimitStatus::POSITIVE_SOFT || 
                status.state == MotionState::ALARM);
    }
    
    /**
     * @brief 测试速度校验
     */
    void testVelocityValidation() {
        auto result = m_controller->moveJog(Axis::X, 10000.0);
        QVERIFY(!result.success);
        QCOMPARE(result.errorCode, ErrorCode::ERR_INVALID_VELOCITY);
    }
    
    /**
     * @brief 测试位置校验
     */
    void testPositionValidation() {
        auto result = m_controller->moveAbsolute(Axis::X, 10000.0, 100.0);
        QVERIFY(!result.success);
        QCOMPARE(result.errorCode, ErrorCode::ERR_INVALID_POSITION);
    }
    
    /**
     * @brief 测试清除报警
     */
    void testClearAlarm() {
        m_controller->setSoftLimits(Axis::X, 5.0, -5.0);
        m_controller->moveJog(Axis::X, 100.0);
        QTest::qWait(100);
        
        auto status = m_controller->getAxisStatus(Axis::X);
        if (status.state == MotionState::ALARM) {
            auto result = m_controller->clearAlarm(Axis::X);
            QVERIFY(result.success);
            
            status = m_controller->getAxisStatus(Axis::X);
            QCOMPARE(status.state, MotionState::IDLE);
            QCOMPARE(status.limitStatus, LimitStatus::NONE);
        }
    }

private:
    SimMotionController* m_controller;
    MockLogger* m_logger;
};
