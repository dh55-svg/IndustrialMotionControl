#pragma once

#include <QtTest/QtTest>
#include "../core/MotionConfig.hh"
#include <QTemporaryFile>

/**
 * @brief MotionConfig单元测试类
 * 
 * 测试内容：
 * - 默认配置初始化
 * - 配置文件加载/保存
 * - 参数校验
 */
class TestMotionConfig : public QObject {
    Q_OBJECT

private slots:
    /**
     * @brief 测试默认配置初始化
     */
    void testDefaultConfig() {
        FileMotionConfig config;
        
        QCOMPARE(IMotionConfig::AXIS_COUNT, 3);
        QCOMPARE(config.getPollInterval(), 10);
        QCOMPARE(config.getCommTimeout(), 1000);
        QVERIFY(config.isSoftLimitEnabled());
    }
    
    /**
     * @brief 测试轴配置
     */
    void testAxisConfig() {
        FileMotionConfig config;
        
        const auto& xConfig = config.getAxisConfig(0);
        QCOMPARE(xConfig.name, QString("X"));
        QVERIFY(xConfig.softLimitPos > 0);
        QVERIFY(xConfig.softLimitNeg < 0);
        QVERIFY(xConfig.maxVelocity > 0);
        
        const auto& yConfig = config.getAxisConfig(1);
        QCOMPARE(yConfig.name, QString("Y"));
        
        const auto& zConfig = config.getAxisConfig(2);
        QCOMPARE(zConfig.name, QString("Z"));
    }
    
    /**
     * @brief 测试位置校验
     */
    void testPositionValidation() {
        FileMotionConfig config;
        const auto& xConfig = config.getAxisConfig(0);
        
        QVERIFY(xConfig.isPositionValid(0.0));
        QVERIFY(xConfig.isPositionValid(100.0));
        QVERIFY(xConfig.isPositionValid(-100.0));
        
        QVERIFY(!xConfig.isPositionValid(xConfig.softLimitPos + 100.0));
        QVERIFY(!xConfig.isPositionValid(xConfig.softLimitNeg - 100.0));
    }
    
    /**
     * @brief 测试速度校验
     */
    void testVelocityValidation() {
        FileMotionConfig config;
        const auto& xConfig = config.getAxisConfig(0);
        
        QVERIFY(xConfig.isVelocityValid(0.0));
        QVERIFY(xConfig.isVelocityValid(xConfig.maxVelocity));
        QVERIFY(xConfig.isVelocityValid(-xConfig.maxVelocity));
        
        QVERIFY(!xConfig.isVelocityValid(xConfig.maxVelocity + 1.0));
        QVERIFY(!xConfig.isVelocityValid(-xConfig.maxVelocity - 1.0));
    }
    
    /**
     * @brief 测试设置轴配置
     */
    void testSetAxisConfig() {
        FileMotionConfig config;
        
        AxisConfigImpl newConfig("Test", 100.0, -100.0, 200.0);
        newConfig.jogVelocity = 30.0;
        newConfig.homeVelocity = 20.0;
        
        config.setAxisConfig(0, newConfig);
        
        const auto& retrieved = config.getAxisConfig(0);
        QCOMPARE(retrieved.name, QString("Test"));
        QCOMPARE(retrieved.softLimitPos, 100.0);
        QCOMPARE(retrieved.softLimitNeg, -100.0);
        QCOMPARE(retrieved.maxVelocity, 200.0);
        QCOMPARE(retrieved.jogVelocity, 30.0);
        QCOMPARE(retrieved.homeVelocity, 20.0);
    }
    
    /**
     * @brief 测试全局配置设置
     */
    void testGlobalConfig() {
        FileMotionConfig config;
        
        config.setPollInterval(20);
        QCOMPARE(config.getPollInterval(), 20);
        
        config.setCommTimeout(2000);
        QCOMPARE(config.getCommTimeout(), 2000);
        
        config.setAutoHome(true);
        QVERIFY(config.isAutoHomeEnabled());
        
        config.setSoftLimitEnabled(false);
        QVERIFY(!config.isSoftLimitEnabled());
    }
    
    /**
     * @brief 测试配置文件保存和加载
     */
    void testSaveAndLoad() {
        FileMotionConfig config;
        
        config.setPollInterval(25);
        config.setCommTimeout(3000);
        config.setAutoHome(true);
        
        AxisConfigImpl xConfig("CustomX", 200.0, -200.0, 400.0);
        config.setAxisConfig(0, xConfig);
        
        QString tempPath = QDir::tempPath() + "/test_motion_config.json";
        QVERIFY(config.saveToFile(tempPath));
        
        FileMotionConfig loadedConfig;
        QVERIFY(loadedConfig.loadFromFile(tempPath));
        
        QCOMPARE(loadedConfig.getPollInterval(), 25);
        QCOMPARE(loadedConfig.getCommTimeout(), 3000);
        QVERIFY(loadedConfig.isAutoHomeEnabled());
        
        const auto& loadedX = loadedConfig.getAxisConfig(0);
        QCOMPARE(loadedX.name, QString("CustomX"));
        QCOMPARE(loadedX.softLimitPos, 200.0);
        QCOMPARE(loadedX.softLimitNeg, -200.0);
        QCOMPARE(loadedX.maxVelocity, 400.0);
        
        QFile::remove(tempPath);
    }
    
    /**
     * @brief 测试加载不存在的文件
     */
    void testLoadNonExistentFile() {
        FileMotionConfig config;
        QVERIFY(!config.loadFromFile("/non/existent/path/config.json"));
    }
    
    /**
     * @brief 测试无效轴索引
     */
    void testInvalidAxisIndex() {
        FileMotionConfig config;
        
        const auto& invalid = config.getAxisConfig(-1);
        QCOMPARE(invalid.name, QString());
        
        const auto& invalid2 = config.getAxisConfig(100);
        QCOMPARE(invalid2.name, QString());
    }
    
    /**
     * @brief 测试AxisConfig的JSON转换
     */
    void testAxisConfigJson() {
        AxisConfigImpl original("TestAxis", 150.0, -150.0, 250.0);
        original.jogVelocity = 40.0;
        original.homeVelocity = 25.0;
        original.homeOffset = 5.0;
        original.enabled = false;
        
        QJsonObject json = original.toJson();
        
        AxisConfigImpl loaded;
        loaded.fromJson(json);
        
        QCOMPARE(loaded.name, original.name);
        QCOMPARE(loaded.softLimitPos, original.softLimitPos);
        QCOMPARE(loaded.softLimitNeg, original.softLimitNeg);
        QCOMPARE(loaded.maxVelocity, original.maxVelocity);
        QCOMPARE(loaded.jogVelocity, original.jogVelocity);
        QCOMPARE(loaded.homeVelocity, original.homeVelocity);
        QCOMPARE(loaded.homeOffset, original.homeOffset);
        QCOMPARE(loaded.enabled, original.enabled);
    }
    
    /**
     * @brief 测试MemoryMotionConfig
     */
    void testMemoryConfig() {
        MemoryMotionConfig config;
        
        config.setPollInterval(50);
        QCOMPARE(config.getPollInterval(), 50);
        
        config.setSoftLimitEnabled(false);
        QVERIFY(!config.isSoftLimitEnabled());
        
        QVERIFY(!config.loadFromFile("test.json"));
        QVERIFY(!config.saveToFile("test.json"));
    }
};
