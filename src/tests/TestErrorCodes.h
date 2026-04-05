#pragma once

#include <QtTest/QtTest>
#include "../core/ErrorCodes.hh"

/**
 * @brief ErrorCodes单元测试类
 * 
 * 测试内容：
 * - 错误码描述
 * - 错误分类
 * - MotionResult操作
 */
class TestErrorCodes : public QObject {
    Q_OBJECT

private slots:
    /**
     * @brief 测试成功错误码
     */
    void testSuccessCode() {
        QCOMPARE(static_cast<int>(ErrorCode::SUCCESS), 0);
        QCOMPARE(ErrorCodes::toString(ErrorCode::SUCCESS), QString("操作成功"));
    }
    
    /**
     * @brief 测试通信错误码
     */
    void testCommErrorCodes() {
        QVERIFY(static_cast<int>(ErrorCode::ERR_COMM_FAILED) >= 10001);
        QVERIFY(static_cast<int>(ErrorCode::ERR_COMM_FAILED) < 20000);
        
        QVERIFY(!ErrorCodes::toString(ErrorCode::ERR_COMM_FAILED).isEmpty());
        QVERIFY(!ErrorCodes::toString(ErrorCode::ERR_COMM_TIMEOUT).isEmpty());
        QVERIFY(!ErrorCodes::toString(ErrorCode::ERR_COMM_DISCONNECTED).isEmpty());
    }
    
    /**
     * @brief 测试运动错误码
     */
    void testMotionErrorCodes() {
        QVERIFY(static_cast<int>(ErrorCode::ERR_AXIS_NOT_ENABLED) >= 20001);
        QVERIFY(static_cast<int>(ErrorCode::ERR_AXIS_NOT_ENABLED) < 30000);
        
        QCOMPARE(ErrorCodes::toString(ErrorCode::ERR_AXIS_NOT_ENABLED), QString("轴未使能"));
        QCOMPARE(ErrorCodes::toString(ErrorCode::ERR_AXIS_MOVING), QString("轴正在运动"));
    }
    
    /**
     * @brief 测试安全错误码
     */
    void testSafetyErrorCodes() {
        QVERIFY(static_cast<int>(ErrorCode::ERR_ESTOP_ACTIVE) >= 30001);
        QVERIFY(static_cast<int>(ErrorCode::ERR_ESTOP_ACTIVE) < 40000);
        
        QCOMPARE(ErrorCodes::toString(ErrorCode::ERR_ESTOP_ACTIVE), QString("急停激活"));
        QCOMPARE(ErrorCodes::toString(ErrorCode::ERR_SOFT_LIMIT_POS), QString("正向软限位触发"));
    }
    
    /**
     * @brief 测试参数错误码
     */
    void testParamErrorCodes() {
        QVERIFY(static_cast<int>(ErrorCode::ERR_INVALID_AXIS) >= 40001);
        QVERIFY(static_cast<int>(ErrorCode::ERR_INVALID_AXIS) < 50000);
        
        QCOMPARE(ErrorCodes::toString(ErrorCode::ERR_INVALID_AXIS), QString("无效轴号"));
        QCOMPARE(ErrorCodes::toString(ErrorCode::ERR_INVALID_VELOCITY), QString("无效速度"));
    }
    
    /**
     * @brief 测试未知错误码
     */
    void testUnknownErrorCode() {
        ErrorCode unknown = static_cast<ErrorCode>(99999);
        QString desc = ErrorCodes::toString(unknown);
        QVERIFY(desc.contains("未知错误码"));
    }
    
    /**
     * @brief 测试安全错误判断
     */
    void testIsSafetyError() {
        QVERIFY(ErrorCodes::isSafetyError(ErrorCode::ERR_ESTOP_ACTIVE));
        QVERIFY(ErrorCodes::isSafetyError(ErrorCode::ERR_SOFT_LIMIT_POS));
        QVERIFY(ErrorCodes::isSafetyError(ErrorCode::ERR_ALARM_ACTIVE));
        
        QVERIFY(!ErrorCodes::isSafetyError(ErrorCode::SUCCESS));
        QVERIFY(!ErrorCodes::isSafetyError(ErrorCode::ERR_COMM_FAILED));
        QVERIFY(!ErrorCodes::isSafetyError(ErrorCode::ERR_AXIS_NOT_ENABLED));
    }
    
    /**
     * @brief 测试通信错误判断
     */
    void testIsCommError() {
        QVERIFY(ErrorCodes::isCommError(ErrorCode::ERR_COMM_FAILED));
        QVERIFY(ErrorCodes::isCommError(ErrorCode::ERR_COMM_TIMEOUT));
        QVERIFY(ErrorCodes::isCommError(ErrorCode::ERR_COMM_DISCONNECTED));
        
        QVERIFY(!ErrorCodes::isCommError(ErrorCode::SUCCESS));
        QVERIFY(!ErrorCodes::isCommError(ErrorCode::ERR_ESTOP_ACTIVE));
    }
    
    /**
     * @brief 测试MotionResult成功创建
     */
    void testMotionResultOk() {
        MotionResult result = MotionResult::ok();
        
        QVERIFY(result.success);
        QCOMPARE(result.errorCode, ErrorCode::SUCCESS);
        QVERIFY(!result.message.isEmpty());
    }
    
    /**
     * @brief 测试MotionResult失败创建
     */
    void testMotionResultFail() {
        MotionResult result = MotionResult::fail(ErrorCode::ERR_ESTOP_ACTIVE, "测试附加信息");
        
        QVERIFY(!result.success);
        QCOMPARE(result.errorCode, ErrorCode::ERR_ESTOP_ACTIVE);
        QVERIFY(result.message.contains("急停"));
        QVERIFY(result.message.contains("测试附加信息"));
    }
    
    /**
     * @brief 测试MotionResult默认构造
     */
    void testMotionResultDefault() {
        MotionResult result;
        
        QVERIFY(result.success);
        QCOMPARE(result.errorCode, ErrorCode::SUCCESS);
        QCOMPARE(result.message, QString(""));
    }
    
    /**
     * @brief 测试MotionResult自定义构造
     */
    void testMotionResultCustom() {
        MotionResult result(false, ErrorCode::ERR_COMM_FAILED, "自定义消息");
        
        QVERIFY(!result.success);
        QCOMPARE(result.errorCode, ErrorCode::ERR_COMM_FAILED);
        QCOMPARE(result.message, QString("自定义消息"));
    }
};
