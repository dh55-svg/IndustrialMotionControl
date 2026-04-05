/**
 * @file main_test.cpp
 * @brief 单元测试入口
 * 
 * 使用Qt测试框架进行单元测试
 */

#include <QtTest/QtTest>
#include "TestSimMotionController.h"
#include "TestMotionConfig.h"
#include "TestErrorCodes.h"

/**
 * @brief 测试运行器
 */
int main(int argc, char *argv[]) {
    int status = 0;
    
    // 运行SimMotionController测试
    {
        TestSimMotionController tc;
        status |= QTest::qExec(&tc, argc, argv);
    }
    
    // 运行MotionConfig测试
    {
        TestMotionConfig tc;
        status |= QTest::qExec(&tc, argc, argv);
    }
    
    // 运行ErrorCodes测试
    {
        TestErrorCodes tc;
        status |= QTest::qExec(&tc, argc, argv);
    }
    
    return status;
}
