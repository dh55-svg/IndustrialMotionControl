/**
 * @file IControllerFactory.cpp
 * @brief 控制器工厂实现
 */

#include "IControllerFactory.hh"
#include "../hal/SimMotionController.hh"
#include "../core/MotionConfig.hh"

/**
 * @brief 创建模拟控制器实例
 */
IMotionController* SimControllerFactory::createController(QObject* parent) {
    auto* controller = new SimMotionController(parent);
    if (m_config) {
        FileMotionConfig* fileConfig = dynamic_cast<FileMotionConfig*>(m_config);
        if (fileConfig) {
            controller->setConfig(*fileConfig);
        }
    }
    return controller;
}
