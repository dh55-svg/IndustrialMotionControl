#pragma once
#include "../interfaces/ILogger.hh"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>
#include <QMutexLocker>
#include <QDebug>
#include <QStringConverter>

/**
 * @brief 文件日志实现 - 将日志写入文件
 * 
 * 功能：
 * - 支持多级别日志记录
 * - 线程安全
 * - 支持文件输出和控制台输出
 * - 自动添加时间戳和日志级别
 * 
 * 使用方式：
 * @code
 * ILogger* logger = new FileLogger();
 * logger->init("app.log", LogLevel::INFO);
 * logger->info("系统启动", "App");
 * @endcode
 */
class FileLogger : public ILogger {
public:
    FileLogger() : m_minLevel(LogLevel::INFO) {}
    
    ~FileLogger() override { close(); }
    
    bool init(const QString& logFilePath, LogLevel level = LogLevel::INFO) override {
        QMutexLocker locker(&m_mutex);
        
        m_minLevel = level;
        
        if (m_logFile.isOpen()) {
            m_logFile.close();
        }
        
        m_logFile.setFileName(logFilePath);
        if (!m_logFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
            qWarning() << "无法打开日志文件:" << logFilePath;
            return false;
        }
        
        m_textStream.setDevice(&m_logFile);
        m_textStream.setEncoding(QStringConverter::Utf8);
        
        logInternal(LogLevel::INFO, "Logger", "日志系统初始化成功");
        return true;
    }
    
    void log(LogLevel level, const QString& message, const QString& module = "System") override {
        if (level < m_minLevel) {
            return;
        }
        
        QMutexLocker locker(&m_mutex);
        logInternal(level, module, message);
    }
    
    void debug(const QString& message, const QString& module = "System") override {
        log(LogLevel::DEBUG, message, module);
    }
    
    void info(const QString& message, const QString& module = "System") override {
        log(LogLevel::INFO, message, module);
    }
    
    void warning(const QString& message, const QString& module = "System") override {
        log(LogLevel::WARNING, message, module);
    }
    
    void error(const QString& message, const QString& module = "System") override {
        log(LogLevel::ERROR, message, module);
    }
    
    void critical(const QString& message, const QString& module = "System") override {
        log(LogLevel::CRITICAL, message, module);
    }
    
    void setMinLevel(LogLevel level) override {
        QMutexLocker locker(&m_mutex);
        m_minLevel = level;
    }
    
    void close() override {
        QMutexLocker locker(&m_mutex);
        if (m_logFile.isOpen()) {
            logInternal(LogLevel::INFO, "Logger", "日志系统关闭");
            m_logFile.close();
        }
    }

private:
    /**
     * @brief 内部日志记录函数（非线程安全，调用前需加锁）
     */
    void logInternal(LogLevel level, const QString& module, const QString& message) {
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        QString levelStr = levelToString(level);
        
        QString logLine = QString("[%1] [%2] [%3] %4\n")
            .arg(timestamp)
            .arg(levelStr)
            .arg(module)
            .arg(message);
        
        if (m_logFile.isOpen()) {
            m_textStream << logLine;
            m_textStream.flush();
        }
        
        qDebug() << logLine.trimmed();
    }
    
    /**
     * @brief 日志级别转字符串
     */
    QString levelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::DEBUG:    return "DEBUG   ";
            case LogLevel::INFO:     return "INFO    ";
            case LogLevel::WARNING:  return "WARNING ";
            case LogLevel::ERROR:    return "ERROR   ";
            case LogLevel::CRITICAL: return "CRITICAL";
            default:                 return "UNKNOWN ";
        }
    }

private:
    QFile m_logFile;            // 日志文件
    QTextStream m_textStream;   // 文本流
    LogLevel m_minLevel;        // 最低日志级别
    QMutex m_mutex;             // 线程安全互斥锁
};

/**
 * @brief 控制台日志实现 - 仅输出到控制台
 */
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger() : m_minLevel(LogLevel::INFO) {}
    
    bool init(const QString& /*logFilePath*/, LogLevel level = LogLevel::INFO) override {
        m_minLevel = level;
        return true;
    }
    
    void log(LogLevel level, const QString& message, const QString& module = "System") override {
        if (level < m_minLevel) return;
        
        QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
        qDebug() << QString("[%1] [%2] [%3] %4")
            .arg(timestamp)
            .arg(levelToString(level))
            .arg(module)
            .arg(message);
    }
    
    void debug(const QString& message, const QString& module = "System") override {
        log(LogLevel::DEBUG, message, module);
    }
    
    void info(const QString& message, const QString& module = "System") override {
        log(LogLevel::INFO, message, module);
    }
    
    void warning(const QString& message, const QString& module = "System") override {
        log(LogLevel::WARNING, message, module);
    }
    
    void error(const QString& message, const QString& module = "System") override {
        log(LogLevel::ERROR, message, module);
    }
    
    void critical(const QString& message, const QString& module = "System") override {
        log(LogLevel::CRITICAL, message, module);
    }
    
    void setMinLevel(LogLevel level) override { m_minLevel = level; }
    void close() override {}

private:
    QString levelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::DEBUG:    return "DEBUG";
            case LogLevel::INFO:     return "INFO";
            case LogLevel::WARNING:  return "WARN";
            case LogLevel::ERROR:    return "ERROR";
            case LogLevel::CRITICAL: return "CRIT";
            default:                 return "?????";
        }
    }

private:
    LogLevel m_minLevel;
};

/**
 * @brief 全局日志器访问点
 * 
 * 为了向后兼容，保留全局日志器访问
 */
namespace LoggerService {
    inline ILogger* instance() {
        static FileLogger logger;
        return &logger;
    }
}

/**
 * @brief 便捷日志宏（使用全局日志器）
 */
#define LOG_DEBUG(msg)   LoggerService::instance()->debug(msg, "App")
#define LOG_INFO(msg)    LoggerService::instance()->info(msg, "App")
#define LOG_WARNING(msg) LoggerService::instance()->warning(msg, "App")
#define LOG_ERROR(msg)   LoggerService::instance()->error(msg, "App")
#define LOG_CRITICAL(msg) LoggerService::instance()->critical(msg, "App")

#define LOG_MODULE_DEBUG(msg, module)   LoggerService::instance()->debug(msg, module)
#define LOG_MODULE_INFO(msg, module)    LoggerService::instance()->info(msg, module)
#define LOG_MODULE_WARNING(msg, module) LoggerService::instance()->warning(msg, module)
#define LOG_MODULE_ERROR(msg, module)   LoggerService::instance()->error(msg, module)
#define LOG_MODULE_CRITICAL(msg, module) LoggerService::instance()->critical(msg, module)
