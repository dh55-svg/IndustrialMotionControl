#pragma once
#include <QString>
#include <map>

/**
 * @brief 错误码定义 - 工业级错误码标准
 * 
 * 错误码分类：
 * - 0xxxx: 通用错误
 * - 1xxxx: 通信错误
 * - 2xxxx: 运动错误
 * - 3xxxx: 安全错误
 * - 4xxxx: 参数错误
 */
enum class ErrorCode : int {
    // ============ 通用错误 (0xxxx) ============
    SUCCESS = 0,                    // 操作成功
    ERR_UNKNOWN = 1,                // 未知错误
    ERR_NOT_INITIALIZED = 2,        // 设备未初始化
    ERR_ALREADY_INITIALIZED = 3,    // 设备已初始化
    ERR_TIMEOUT = 4,                // 操作超时
    
    // ============ 通信错误 (1xxxx) ============
    ERR_COMM_FAILED = 10001,        // 通信失败
    ERR_COMM_TIMEOUT = 10002,       // 通信超时
    ERR_COMM_DISCONNECTED = 10003,  // 连接断开
    ERR_COMM_BUFFER_OVERFLOW = 10004, // 缓冲区溢出
    
    // ============ 运动错误 (2xxxx) ============
    ERR_AXIS_NOT_ENABLED = 20001,   // 轴未使能
    ERR_AXIS_MOVING = 20002,        // 轴正在运动
    ERR_AXIS_HOMING_FAILED = 20003, // 回零失败
    ERR_AXIS_NOT_HOMED = 20004,     // 轴未回零
    ERR_MOVE_FAILED = 20005,        // 运动失败
    ERR_STOP_FAILED = 20006,        // 停止失败
    
    // ============ 安全错误 (3xxxx) ============
    ERR_ESTOP_ACTIVE = 30001,       // 急停激活
    ERR_SOFT_LIMIT_POS = 30002,     // 正向软限位
    ERR_SOFT_LIMIT_NEG = 30003,     // 负向软限位
    ERR_HARD_LIMIT_POS = 30004,     // 正向硬限位
    ERR_HARD_LIMIT_NEG = 30005,     // 负向硬限位
    ERR_ALARM_ACTIVE = 30006,       // 报警激活
    ERR_INTERLOCK_FAILED = 30007,   // 互锁检查失败
    
    // ============ 参数错误 (4xxxx) ============
    ERR_INVALID_AXIS = 40001,       // 无效轴号
    ERR_INVALID_POSITION = 40002,   // 无效位置
    ERR_INVALID_VELOCITY = 40003,   // 无效速度
    ERR_INVALID_PARAM = 40004,      // 无效参数
    ERR_PARAM_OUT_OF_RANGE = 40005, // 参数超出范围
};

/**
 * @brief 错误码工具类 - 提供错误码转换和描述功能
 */
class ErrorCodes {
public:
    /**
     * @brief 获取错误码描述
     * @param code 错误码
     * @return 错误描述字符串
     */
    static QString toString(ErrorCode code) {
        static const std::map<ErrorCode, QString> descriptions = {
            // 通用错误
            {ErrorCode::SUCCESS, QStringLiteral("操作成功")},
            {ErrorCode::ERR_UNKNOWN, QStringLiteral("未知错误")},
            {ErrorCode::ERR_NOT_INITIALIZED, QStringLiteral("设备未初始化")},
            {ErrorCode::ERR_ALREADY_INITIALIZED, QStringLiteral("设备已初始化")},
            {ErrorCode::ERR_TIMEOUT, QStringLiteral("操作超时")},
            
            // 通信错误
            {ErrorCode::ERR_COMM_FAILED, QStringLiteral("通信失败")},
            {ErrorCode::ERR_COMM_TIMEOUT, QStringLiteral("通信超时")},
            {ErrorCode::ERR_COMM_DISCONNECTED, QStringLiteral("连接断开")},
            {ErrorCode::ERR_COMM_BUFFER_OVERFLOW, QStringLiteral("缓冲区溢出")},
            
            // 运动错误
            {ErrorCode::ERR_AXIS_NOT_ENABLED, QStringLiteral("轴未使能")},
            {ErrorCode::ERR_AXIS_MOVING, QStringLiteral("轴正在运动")},
            {ErrorCode::ERR_AXIS_HOMING_FAILED, QStringLiteral("回零失败")},
            {ErrorCode::ERR_AXIS_NOT_HOMED, QStringLiteral("轴未回零")},
            {ErrorCode::ERR_MOVE_FAILED, QStringLiteral("运动失败")},
            {ErrorCode::ERR_STOP_FAILED, QStringLiteral("停止失败")},
            
            // 安全错误
            {ErrorCode::ERR_ESTOP_ACTIVE, QStringLiteral("急停激活")},
            {ErrorCode::ERR_SOFT_LIMIT_POS, QStringLiteral("正向软限位触发")},
            {ErrorCode::ERR_SOFT_LIMIT_NEG, QStringLiteral("负向软限位触发")},
            {ErrorCode::ERR_HARD_LIMIT_POS, QStringLiteral("正向硬限位触发")},
            {ErrorCode::ERR_HARD_LIMIT_NEG, QStringLiteral("负向硬限位触发")},
            {ErrorCode::ERR_ALARM_ACTIVE, QStringLiteral("报警激活")},
            {ErrorCode::ERR_INTERLOCK_FAILED, QStringLiteral("互锁检查失败")},
            
            // 参数错误
            {ErrorCode::ERR_INVALID_AXIS, QStringLiteral("无效轴号")},
            {ErrorCode::ERR_INVALID_POSITION, QStringLiteral("无效位置")},
            {ErrorCode::ERR_INVALID_VELOCITY, QStringLiteral("无效速度")},
            {ErrorCode::ERR_INVALID_PARAM, QStringLiteral("无效参数")},
            {ErrorCode::ERR_PARAM_OUT_OF_RANGE, QStringLiteral("参数超出范围")},
        };
        
        auto it = descriptions.find(code);
        if (it != descriptions.end()) {
            return it->second;
        }
        return QStringLiteral("未知错误码: ") + QString::number(static_cast<int>(code));
    }
    
    /**
     * @brief 判断是否为安全相关错误
     * @param code 错误码
     * @return true=安全错误，false=非安全错误
     */
    static bool isSafetyError(ErrorCode code) {
        int codeValue = static_cast<int>(code);
        return codeValue >= 30001 && codeValue < 40000;
    }
    
    /**
     * @brief 判断是否为通信错误
     * @param code 错误码
     * @return true=通信错误，false=非通信错误
     */
    static bool isCommError(ErrorCode code) {
        int codeValue = static_cast<int>(code);
        return codeValue >= 10001 && codeValue < 20000;
    }
};

/**
 * @brief 运动结果结构体 - 封装操作结果
 */
struct MotionResult {
    bool success;           // 操作是否成功
    ErrorCode errorCode;    // 错误码
    QString message;        // 结果消息
    
    MotionResult(bool ok = true, ErrorCode code = ErrorCode::SUCCESS, const QString& msg = "")
        : success(ok), errorCode(code), message(msg) {}
    
    static MotionResult ok() {
        return MotionResult(true, ErrorCode::SUCCESS, QStringLiteral("操作成功"));
    }
    
    static MotionResult fail(ErrorCode code, const QString& extraMsg = "") {
        QString msg = ErrorCodes::toString(code);
        if (!extraMsg.isEmpty()) {
            msg += " - " + extraMsg;
        }
        return MotionResult(false, code, msg);
    }
};
