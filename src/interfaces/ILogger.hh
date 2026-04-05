#pragma once
#include <QString>

/**
 * @brief 日志级别定义
 */
enum class LogLevel : int {
    DEBUG = 0,      // 调试信息
    INFO = 1,       // 一般信息
    WARNING = 2,    // 警告信息
    ERROR = 3,      // 错误信息
    CRITICAL = 4    // 严重错误
};

/**
 * @brief 日志接口 - 抽象日志记录功能
 * 
 * 设计目的：
 * - 解耦日志实现，便于替换不同的日志后端
 * - 支持依赖注入，方便单元测试时使用Mock
 * 
 * 实现类：
 * - FileLogger: 文件日志实现
 * - ConsoleLogger: 控制台日志实现
 * - MockLogger: 测试用Mock实现
 */
class ILogger {
public:
    virtual ~ILogger() = default;
    
    /**
     * @brief 初始化日志系统
     * @param logFilePath 日志文件路径
     * @param level 最低日志级别
     * @return true=成功，false=失败
     */
    virtual bool init(const QString& logFilePath, LogLevel level = LogLevel::INFO) = 0;
    
    /**
     * @brief 记录日志
     * @param level 日志级别
     * @param message 日志消息
     * @param module 模块名称
     */
    virtual void log(LogLevel level, const QString& message, const QString& module = "System") = 0;
    
    /**
     * @brief 调试日志
     */
    virtual void debug(const QString& message, const QString& module = "System") = 0;
    
    /**
     * @brief 信息日志
     */
    virtual void info(const QString& message, const QString& module = "System") = 0;
    
    /**
     * @brief 警告日志
     */
    virtual void warning(const QString& message, const QString& module = "System") = 0;
    
    /**
     * @brief 错误日志
     */
    virtual void error(const QString& message, const QString& module = "System") = 0;
    
    /**
     * @brief 严重错误日志
     */
    virtual void critical(const QString& message, const QString& module = "System") = 0;
    
    /**
     * @brief 设置最低日志级别
     */
    virtual void setMinLevel(LogLevel level) = 0;
    
    /**
     * @brief 关闭日志系统
     */
    virtual void close() = 0;
};
