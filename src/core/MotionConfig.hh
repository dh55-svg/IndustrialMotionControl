#pragma once
#include "../interfaces/IMotionConfig.hh"
#include <QFile>
#include <QIODevice>
#include <QJsonDocument>
#include <QJsonArray>

/**
 * @brief 轴配置实现 - 继承自IAxisConfig
 */
struct AxisConfigImpl : public IAxisConfig {
    AxisConfigImpl() = default;
    
    AxisConfigImpl(const QString& n, double posLimit, double negLimit, double maxVel) {
        name = n;
        softLimitPos = posLimit;
        softLimitNeg = negLimit;
        maxVelocity = maxVel;
    }
};

/**
 * @brief 文件配置实现 - 从JSON文件加载配置
 * 
 * 功能：
 * - 管理三轴运动参数
 * - 支持从JSON文件加载/保存配置
 * - 提供参数校验接口
 */
class FileMotionConfig : public IMotionConfig {
public:
    FileMotionConfig() {
        initDefaultConfig();
    }
    
    void initDefaultConfig() override {
        m_axisConfigs[0] = AxisConfigImpl("X", 500.0, -500.0, 300.0);
        m_axisConfigs[1] = AxisConfigImpl("Y", 500.0, -500.0, 300.0);
        m_axisConfigs[2] = AxisConfigImpl("Z", 200.0, -200.0, 200.0);
        
        m_pollInterval = 10;
        m_commTimeout = 1000;
        m_autoHome = false;
        m_softLimitEnabled = true;
    }
    
    const IAxisConfig& getAxisConfig(int axisIndex) const override {
        static AxisConfigImpl invalidConfig;
        if (axisIndex < 0 || axisIndex >= AXIS_COUNT) {
            return invalidConfig;
        }
        return m_axisConfigs[axisIndex];
    }
    
    void setAxisConfig(int axisIndex, const IAxisConfig& config) override {
        if (axisIndex >= 0 && axisIndex < AXIS_COUNT) {
            m_axisConfigs[axisIndex] = static_cast<const AxisConfigImpl&>(config);
        }
    }
    
    bool loadFromFile(const QString& filePath) override {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            return false;
        }
        
        QByteArray data = file.readAll();
        file.close();
        
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
        if (parseError.error != QJsonParseError::NoError) {
            return false;
        }
        
        QJsonObject root = doc.object();
        
        QJsonArray axes = root["axes"].toArray();
        for (int i = 0; i < axes.size() && i < AXIS_COUNT; ++i) {
            m_axisConfigs[i].fromJson(axes[i].toObject());
        }
        
        m_pollInterval = root["pollInterval"].toInt(10);
        m_commTimeout = root["commTimeout"].toInt(1000);
        m_autoHome = root["autoHome"].toBool(false);
        m_softLimitEnabled = root["softLimitEnabled"].toBool(true);
        
        return true;
    }
    
    bool saveToFile(const QString& filePath) const override {
        QJsonObject root;
        
        QJsonArray axes;
        for (int i = 0; i < AXIS_COUNT; ++i) {
            axes.append(m_axisConfigs[i].toJson());
        }
        root["axes"] = axes;
        
        root["pollInterval"] = m_pollInterval;
        root["commTimeout"] = m_commTimeout;
        root["autoHome"] = m_autoHome;
        root["softLimitEnabled"] = m_softLimitEnabled;
        
        QJsonDocument doc(root);
        
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly)) {
            return false;
        }
        
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
        
        return true;
    }
    
    int getPollInterval() const override { return m_pollInterval; }
    void setPollInterval(int ms) override { m_pollInterval = ms; }
    
    int getCommTimeout() const override { return m_commTimeout; }
    void setCommTimeout(int ms) override { m_commTimeout = ms; }
    
    bool isAutoHomeEnabled() const override { return m_autoHome; }
    void setAutoHome(bool enable) override { m_autoHome = enable; }
    
    bool isSoftLimitEnabled() const override { return m_softLimitEnabled; }
    void setSoftLimitEnabled(bool enable) override { m_softLimitEnabled = enable; }

private:
    AxisConfigImpl m_axisConfigs[AXIS_COUNT];
    
    int m_pollInterval;
    int m_commTimeout;
    bool m_autoHome;
    bool m_softLimitEnabled;
};

/**
 * @brief 内存配置实现 - 用于测试
 */
class MemoryMotionConfig : public IMotionConfig {
public:
    MemoryMotionConfig() {
        initDefaultConfig();
    }
    
    void initDefaultConfig() override {
        m_axisConfigs[0] = AxisConfigImpl("X", 500.0, -500.0, 300.0);
        m_axisConfigs[1] = AxisConfigImpl("Y", 500.0, -500.0, 300.0);
        m_axisConfigs[2] = AxisConfigImpl("Z", 200.0, -200.0, 200.0);
        
        m_pollInterval = 10;
        m_commTimeout = 1000;
        m_autoHome = false;
        m_softLimitEnabled = true;
    }
    
    const IAxisConfig& getAxisConfig(int axisIndex) const override {
        static AxisConfigImpl invalidConfig;
        if (axisIndex < 0 || axisIndex >= AXIS_COUNT) {
            return invalidConfig;
        }
        return m_axisConfigs[axisIndex];
    }
    
    void setAxisConfig(int axisIndex, const IAxisConfig& config) override {
        if (axisIndex >= 0 && axisIndex < AXIS_COUNT) {
            m_axisConfigs[axisIndex] = static_cast<const AxisConfigImpl&>(config);
        }
    }
    
    bool loadFromFile(const QString& /*filePath*/) override { return false; }
    bool saveToFile(const QString& /*filePath*/) const override { return false; }
    
    int getPollInterval() const override { return m_pollInterval; }
    void setPollInterval(int ms) override { m_pollInterval = ms; }
    
    int getCommTimeout() const override { return m_commTimeout; }
    void setCommTimeout(int ms) override { m_commTimeout = ms; }
    
    bool isAutoHomeEnabled() const override { return m_autoHome; }
    void setAutoHome(bool enable) override { m_autoHome = enable; }
    
    bool isSoftLimitEnabled() const override { return m_softLimitEnabled; }
    void setSoftLimitEnabled(bool enable) override { m_softLimitEnabled = enable; }

private:
    AxisConfigImpl m_axisConfigs[AXIS_COUNT];
    int m_pollInterval;
    int m_commTimeout;
    bool m_autoHome;
    bool m_softLimitEnabled;
};

/**
 * @brief 类型别名，保持向后兼容
 */
using MotionConfig = FileMotionConfig;
using AxisConfig = AxisConfigImpl;
