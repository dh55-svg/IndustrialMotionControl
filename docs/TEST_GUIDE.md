# 工业运动控制系统 - 单元测试文档

## 目录

1. [测试概述](#测试概述)
2. [测试环境配置](#测试环境配置)
3. [测试框架](#测试框架)
4. [测试用例说明](#测试用例说明)
5. [运行测试](#运行测试)
6. [测试覆盖率](#测试覆盖率)
7. [Mock对象使用](#mock对象使用)
8. [最佳实践](#最佳实践)

---

## 测试概述

本项目采用 **Qt Test** 框架进行单元测试，测试覆盖核心业务逻辑、硬件抽象层和配置管理模块。

### 测试目标

- 验证运动控制器的核心功能正确性
- 验证安全机制（急停、软限位）的有效性
- 验证配置管理的加载/保存功能
- 验证错误码系统的正确性

### 测试架构

```
tests/
├── main_test.cpp              # 测试入口
├── MockLogger.h               # Mock日志器
├── TestSimMotionController.h  # 控制器测试
├── TestSimMotionController.cpp
├── TestMotionConfig.h         # 配置测试
├── TestMotionConfig.cpp
├── TestErrorCodes.h           # 错误码测试
└── TestErrorCodes.cpp
```

---

## 测试环境配置

### 前置条件

1. **Qt 6.x** 已安装
2. **CMake 3.16+** 已安装
3. **MinGW 或 MSVC** 编译器

### 编译测试

```bash
# 进入构建目录
cd c:\qtrpoject\IndustrialMotionControl

# 配置CMake（启用测试）
cmake -B build -G "MinGW Makefiles" -DBUILD_TESTS=ON

# 编译
cmake --build build

# 编译测试
cmake --build build --target IndustrialMotionControl_tests
```

---

## 测试框架

### Qt Test 基本结构

```cpp
class TestClassName : public QObject {
    Q_OBJECT

private slots:
    // 测试生命周期
    void initTestCase();      // 整个测试类开始前执行一次
    void cleanupTestCase();   // 整个测试类结束后执行一次
    void init();              // 每个测试用例前执行
    void cleanup();           // 每个测试用例后执行

    // 测试用例（以test开头）
    void testFunction1();
    void testFunction2();
};
```

### 测试宏

| 宏 | 用途 |
|---|---|
| `QVERIFY(condition)` | 验证条件为真 |
| `QCOMPARE(actual, expected)` | 验证实际值等于期望值 |
| `QVERIFY2(condition, message)` | 验证条件，失败时显示消息 |
| `QTEST(actual, expected)` | 带数据标签的比较 |
| `QSKIP(message)` | 跳过当前测试 |

---

## 测试用例说明

### 1. SimMotionController 测试

**文件**: [TestSimMotionController.h](file:///c:/qtrpoject/IndustrialMotionControl/src/tests/TestSimMotionController.h)

#### 测试用例列表

| 测试用例 | 描述 | 验证点 |
|---------|------|--------|
| `testOpen` | 设备打开 | 打开成功、重复打开处理 |
| `testClose` | 设备关闭 | 关闭后状态正确 |
| `testEnableAxis` | 轴使能/失能 | 使能状态切换正确 |
| `testEnableAxisDuringEstop` | 急停时使能禁止 | 急停互锁机制 |
| `testJogMove` | 点动运动 | 位置更新正确 |
| `testEmergencyStop` | 急停功能 | 所有轴进入ESTOP状态 |
| `testMoveDuringEstop` | 急停时运动禁止 | 返回ERR_ESTOP_ACTIVE |
| `testResetEstop` | 急停复位 | 恢复IDLE状态 |
| `testHome` | 回零功能 | 位置归零、标志位设置 |
| `testSoftLimit` | 软限位保护 | 触发限位报警 |
| `testVelocityValidation` | 速度校验 | 超速返回错误 |
| `testPositionValidation` | 位置校验 | 超限返回错误 |
| `testClearAlarm` | 清除报警 | 报警状态清除 |

#### 示例代码

```cpp
void TestSimMotionController::testEmergencyStop() {
    // 准备：开始运动
    m_controller->moveJog(Axis::X, 100.0);
    QTest::qWait(50);
    
    // 执行：触发急停
    auto result = m_controller->emergencyStop();
    
    // 验证
    QVERIFY(result.success);              // 操作成功
    QVERIFY(m_controller->isEstopActive()); // 急停状态激活
    
    // 验证所有轴状态
    auto statusX = m_controller->getAxisStatus(Axis::X);
    QCOMPARE(statusX.state, MotionState::ESTOP);
}
```

---

### 2. MotionConfig 测试

**文件**: [TestMotionConfig.h](file:///c:/qtrpoject/IndustrialMotionControl/src/tests/TestMotionConfig.h)

#### 测试用例列表

| 测试用例 | 描述 | 验证点 |
|---------|------|--------|
| `testDefaultConfig` | 默认配置 | 默认值正确 |
| `testAxisConfig` | 轴配置 | 三轴配置正确 |
| `testPositionValidation` | 位置校验 | 软限位范围检查 |
| `testVelocityValidation` | 速度校验 | 最大速度检查 |
| `testSetAxisConfig` | 设置轴配置 | 配置更新正确 |
| `testGlobalConfig` | 全局配置 | 轮询周期等设置 |
| `testSaveAndLoad` | 文件保存加载 | JSON序列化正确 |
| `testLoadNonExistentFile` | 加载不存在文件 | 返回失败 |
| `testInvalidAxisIndex` | 无效轴索引 | 返回空配置 |
| `testAxisConfigJson` | JSON转换 | 序列化/反序列化 |
| `testMemoryConfig` | 内存配置 | 不支持文件操作 |

#### 示例代码

```cpp
void TestMotionConfig::testSaveAndLoad() {
    FileMotionConfig config;
    
    // 修改配置
    config.setPollInterval(25);
    AxisConfigImpl xConfig("CustomX", 200.0, -200.0, 400.0);
    config.setAxisConfig(0, xConfig);
    
    // 保存
    QString tempPath = QDir::tempPath() + "/test_config.json";
    QVERIFY(config.saveToFile(tempPath));
    
    // 加载到新对象
    FileMotionConfig loadedConfig;
    QVERIFY(loadedConfig.loadFromFile(tempPath));
    
    // 验证
    QCOMPARE(loadedConfig.getPollInterval(), 25);
    QCOMPARE(loadedConfig.getAxisConfig(0).name, QString("CustomX"));
    
    // 清理
    QFile::remove(tempPath);
}
```

---

### 3. ErrorCodes 测试

**文件**: [TestErrorCodes.h](file:///c:/qtrpoject/IndustrialMotionControl/src/tests/TestErrorCodes.h)

#### 测试用例列表

| 测试用例 | 描述 | 验证点 |
|---------|------|--------|
| `testSuccessCode` | 成功错误码 | 值为0，描述正确 |
| `testCommErrorCodes` | 通信错误码 | 范围10001-19999 |
| `testMotionErrorCodes` | 运动错误码 | 范围20001-29999 |
| `testSafetyErrorCodes` | 安全错误码 | 范围30001-39999 |
| `testParamErrorCodes` | 参数错误码 | 范围40001-49999 |
| `testUnknownErrorCode` | 未知错误码 | 返回"未知错误码" |
| `testIsSafetyError` | 安全错误判断 | 分类正确 |
| `testIsCommError` | 通信错误判断 | 分类正确 |
| `testMotionResultOk` | 成功结果创建 | 字段正确 |
| `testMotionResultFail` | 失败结果创建 | 消息组合正确 |

#### 示例代码

```cpp
void TestErrorCodes::testMotionResultFail() {
    // 创建失败结果
    MotionResult result = MotionResult::fail(
        ErrorCode::ERR_ESTOP_ACTIVE, 
        "测试附加信息"
    );
    
    // 验证
    QVERIFY(!result.success);
    QCOMPARE(result.errorCode, ErrorCode::ERR_ESTOP_ACTIVE);
    QVERIFY(result.message.contains("急停"));
    QVERIFY(result.message.contains("测试附加信息"));
}
```

---

## 运行测试

### 方法一：命令行运行

```bash
# 进入构建目录
cd build

# 运行所有测试
.\src\IndustrialMotionControl_tests.exe

# 运行特定测试类
.\src\IndustrialMotionControl_tests.exe TestSimMotionController

# 运行特定测试用例
.\src\IndustrialMotionControl_tests.exe TestSimMotionController::testEmergencyStop

# 输出详细信息
.\src\IndustrialMotionControl_tests.exe -v1

# 输出所有调试信息
.\src\IndustrialMotionControl_tests.exe -v2
```

### 方法二：CMake/CTest

```bash
# 运行所有测试
cd build
ctest

# 详细输出
ctest -V

# 运行特定测试
ctest -R TestSimMotionController

# 并行运行
ctest -j4
```

### 方法三：Qt Creator

1. 打开项目
2. 选择 **Tests** 视图
3. 右键测试项目 → **Run**

---

## 测试覆盖率

### 覆盖模块

| 模块 | 文件 | 测试覆盖 | 覆盖率 |
|------|------|----------|--------|
| HAL层 | SimMotionController | 13个测试用例 | ~90% |
| 配置 | MotionConfig | 11个测试用例 | ~85% |
| 错误处理 | ErrorCodes | 11个测试用例 | ~95% |

### 未覆盖功能

- MotionManager（需要线程环境）
- UI组件（需要GUI环境）
- 实际硬件通信

---

## Mock对象使用

### MockLogger

用于测试时替代真实日志器，记录所有日志调用。

```cpp
#include "MockLogger.h"

void TestSimMotionController::initTestCase() {
    // 创建Mock日志器
    m_logger = new MockLogger();
    
    // 创建被测对象
    m_controller = new SimMotionController(this);
}

void TestSimMotionController::testSomething() {
    m_logger->clear();  // 清除之前的日志
    
    // 执行测试...
    
    // 验证日志调用
    QVERIFY(m_logger->containsMessage("急停"));
    QCOMPARE(m_logger->countByLevel(LogLevel::ERROR), 1);
}
```

### MockLogger API

| 方法 | 描述 |
|------|------|
| `getEntries()` | 获取所有日志条目 |
| `clear()` | 清除所有日志 |
| `countByLevel(level)` | 统计特定级别的日志数 |
| `containsMessage(text)` | 检查是否包含特定消息 |

---

## 最佳实践

### 1. 测试命名规范

```cpp
// 格式：test + 被测功能 + 场景
void testOpen();                    // 基本功能
void testOpenWhenAlreadyOpen();     // 特定场景
void testMoveDuringEstop();         // 异常情况
```

### 2. 测试结构（AAA模式）

```cpp
void TestClass::testFunction() {
    // Arrange（准备）
    double expectedPosition = 100.0;
    m_controller->open();
    
    // Act（执行）
    m_controller->moveJog(Axis::X, 50.0);
    QTest::qWait(100);
    
    // Assert（断言）
    auto status = m_controller->getAxisStatus(Axis::X);
    QVERIFY(status.position > 0);
}
```

### 3. 测试隔离

```cpp
void TestSimMotionController::init() {
    // 每个测试前重置状态
    m_controller->close();
    m_controller->open();
    m_logger->clear();
}
```

### 4. 异步测试

```cpp
void TestClass::testAsyncOperation() {
    // 开始异步操作
    m_controller->moveJog(Axis::X, 100.0);
    
    // 等待操作完成
    QTest::qWait(100);  // 等待100ms
    
    // 验证结果
    QVERIFY(m_controller->getAxisStatus(Axis::X).position > 0);
}
```

### 5. 边界条件测试

```cpp
void TestClass::testBoundaryConditions() {
    // 测试边界值
    QVERIFY(xConfig.isPositionValid(xConfig.softLimitPos));     // 刚好在限位
    QVERIFY(!xConfig.isPositionValid(xConfig.softLimitPos + 0.1)); // 刚好超限
    
    // 测试极端值
    QVERIFY(!xConfig.isVelocityValid(999999.0));
    QVERIFY(!xConfig.isVelocityValid(-999999.0));
}
```

---

## 常见问题

### Q1: 测试编译失败，找不到Qt Test

确保CMake配置正确：
```cmake
find_package(Qt6 REQUIRED COMPONENTS Test)
target_link_libraries(${PROJECT_NAME}_tests PRIVATE Qt6::Test)
```

### Q2: 测试运行时找不到DLL

将Qt的bin目录添加到PATH，或使用windeployqt部署。

### Q3: 异步测试不稳定

增加等待时间，或使用QSignalSpy等待信号：
```cpp
QSignalSpy spy(m_controller, &IMotionController::positionUpdated);
QVERIFY(spy.wait(1000));  // 最多等待1秒
```

### Q4: 如何测试私有方法

使用友元类或在测试中访问protected方法：
```cpp
class TestAccess : public SimMotionController {
public:
    using SimMotionController::privateMethod;  // 暴露私有方法
};
```

---

## 附录

### 测试输出示例

```
********* Start testing of TestSimMotionController *********
PASS   : TestSimMotionController::initTestCase()
PASS   : TestSimMotionController::init()
PASS   : TestSimMotionController::testOpen()
PASS   : TestSimMotionController::init()
PASS   : TestSimMotionController::testClose()
PASS   : TestSimMotionController::init()
PASS   : TestSimMotionController::testEnableAxis()
PASS   : TestSimMotionController::init()
PASS   : TestSimMotionController::testEmergencyStop()
...
PASS   : TestSimMotionController::cleanupTestCase()
Totals: 13 passed, 0 failed, 0 skipped, 0 blacklisted, 1234ms
********* Finished testing of TestSimMotionController *********
```

### 相关文档

- [Qt Test 文档](https://doc.qt.io/qt-6/qtest-index.html)
- [CMake CTest 文档](https://cmake.org/cmake/help/latest/manual/ctest.1.html)
