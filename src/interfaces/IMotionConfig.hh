#pragma once
#include <QString>
#include <QJsonObject>

/**
 * @brief 轴配置参数接口 - 单个轴的运动参数配置
 */
struct IAxisConfig {
    QString name;               // 轴名称
    double softLimitPos = 1000.0;   // 正向软限位 (mm)
    double softLimitNeg = -1000.0;  // 负向软限位 (mm)
    double maxVelocity = 500.0;     // 最大速度 (mm/s)
    double maxAcceleration = 1000.0; // 最大加速度 (mm/s²)
    double homeVelocity = 50.0;     // 回零速度 (mm/s)
    double jogVelocity = 50.0;      // 点动速度 (mm/s)
    double homeOffset = 0.0;        // 回零偏移 (mm)
    bool enabled = true;            // 轴使能状态
    
    virtual ~IAxisConfig() = default;
    
    /**
     * @brief 检查位置是否在软限位范围内
     */
    virtual bool isPositionValid(double pos) const {
        return pos >= softLimitNeg && pos <= softLimitPos;
    }
    
    /**
     * @brief 检查速度是否在有效范围内
     */
    virtual bool isVelocityValid(double vel) const {
        return std::abs(vel) <= maxVelocity;
    }
    
    /**
     * @brief 从JSON对象加载配置
     */
    virtual void fromJson(const QJsonObject& json) {
        name = json["name"].toString();
        softLimitPos = json["softLimitPos"].toDouble(1000.0);
        softLimitNeg = json["softLimitNeg"].toDouble(-1000.0);
        maxVelocity = json["maxVelocity"].toDouble(500.0);
        maxAcceleration = json["maxAcceleration"].toDouble(1000.0);
        homeVelocity = json["homeVelocity"].toDouble(50.0);
        jogVelocity = json["jogVelocity"].toDouble(50.0);
        homeOffset = json["homeOffset"].toDouble(0.0);
        enabled = json["enabled"].toBool(true);
    }
    
    /**
     * @brief 转换为JSON对象
     */
    virtual QJsonObject toJson() const {
        QJsonObject json;
        json["name"] = name;
        json["softLimitPos"] = softLimitPos;
        json["softLimitNeg"] = softLimitNeg;
        json["maxVelocity"] = maxVelocity;
        json["maxAcceleration"] = maxAcceleration;
        json["homeVelocity"] = homeVelocity;
        json["jogVelocity"] = jogVelocity;
        json["homeOffset"] = homeOffset;
        json["enabled"] = enabled;
        return json;
    }
};

/**
 * @brief 运动配置接口 - 抽象运动参数配置
 * 
 * 设计目的：
 * - 解耦配置实现，支持多种配置源（文件、数据库、网络等）
 * - 支持依赖注入，方便单元测试
 * 
 * 实现类：
 * - FileMotionConfig: 文件配置实现
 * - MemoryMotionConfig: 内存配置实现（测试用）
 * - DatabaseMotionConfig: 数据库配置实现
 */
class IMotionConfig {
public:
    static constexpr int AXIS_COUNT = 3;  // 轴数量
    
    virtual ~IMotionConfig() = default;
    
    /**
     * @brief 初始化默认配置
     */
    virtual void initDefaultConfig() = 0;
    
    /**
     * @brief 获取轴配置
     * @param axisIndex 轴索引 (0-2)
     * @return 轴配置引用
     */
    virtual const IAxisConfig& getAxisConfig(int axisIndex) const = 0;
    
    /**
     * @brief 设置轴配置
     */
    virtual void setAxisConfig(int axisIndex, const IAxisConfig& config) = 0;
    
    /**
     * @brief 从文件加载配置
     * @param filePath 配置文件路径
     * @return true=成功，false=失败
     */
    virtual bool loadFromFile(const QString& filePath) = 0;
    
    /**
     * @brief 保存配置到文件
     * @param filePath 配置文件路径
     * @return true=成功，false=失败
     */
    virtual bool saveToFile(const QString& filePath) const = 0;
    
    // ============ 全局配置访问器 ============
    
    virtual int getPollInterval() const = 0;
    virtual void setPollInterval(int ms) = 0;
    
    virtual int getCommTimeout() const = 0;
    virtual void setCommTimeout(int ms) = 0;
    
    virtual bool isAutoHomeEnabled() const = 0;
    virtual void setAutoHome(bool enable) = 0;
    
    virtual bool isSoftLimitEnabled() const = 0;
    virtual void setSoftLimitEnabled(bool enable) = 0;
};
