#pragma once
#include "../interfaces/ILogger.hh"
#include <QVector>

/**
 * @brief Mock日志器 - 用于单元测试
 * 
 * 功能：
 * - 记录所有日志调用，便于测试验证
 * - 不实际输出日志
 */
class MockLogger : public ILogger {
public:
    struct LogEntry {
        LogLevel level;
        QString message;
        QString module;
    };
    
    bool init(const QString& /*logFilePath*/, LogLevel level = LogLevel::INFO) override {
        m_minLevel = level;
        return true;
    }
    
    void log(LogLevel level, const QString& message, const QString& module = "System") override {
        if (level >= m_minLevel) {
            m_entries.append({level, message, module});
        }
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
    
    /**
     * @brief 获取所有日志条目
     */
    const QVector<LogEntry>& getEntries() const { return m_entries; }
    
    /**
     * @brief 清除所有日志条目
     */
    void clear() { m_entries.clear(); }
    
    /**
     * @brief 获取特定级别的日志数量
     */
    int countByLevel(LogLevel level) const {
        int count = 0;
        for (const auto& entry : m_entries) {
            if (entry.level == level) count++;
        }
        return count;
    }
    
    /**
     * @brief 检查是否包含特定消息
     */
    bool containsMessage(const QString& text) const {
        for (const auto& entry : m_entries) {
            if (entry.message.contains(text)) return true;
        }
        return false;
    }

private:
    LogLevel m_minLevel = LogLevel::DEBUG;
    QVector<LogEntry> m_entries;
};
