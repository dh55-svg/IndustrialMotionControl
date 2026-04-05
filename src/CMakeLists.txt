set(SOURCES
    app/MainWindow.cpp
    ui/JogPanel.cpp
    ui/StatusPanel.cpp
    core/MotionManager.cpp
    hal/SimMotionController.cpp
    interfaces/IControllerFactory.cpp
)

set(HEADERS
    app/MainWindow.h
    ui/JogPanel.hh
    ui/StatusPanel.hh
    core/MotionManager.hh
    core/ErrorCodes.hh
    core/Logger.hh
    core/MotionConfig.hh
    hal/IMotionController.hh
    hal/SimMotionController.hh
    interfaces/ILogger.hh
    interfaces/IMotionConfig.hh
    interfaces/IMotionManager.hh
    interfaces/IControllerFactory.hh
)

# 主程序可执行文件（包含main.cpp）
add_executable(${PROJECT_NAME} WIN32 main.cpp ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

# ============ 单元测试配置 ============
option(BUILD_TESTS "构建单元测试" ON)

if(BUILD_TESTS)
    find_package(Qt6 REQUIRED COMPONENTS Test)
    
    set(TEST_SOURCES
        tests/TestSimMotionController.cpp
        tests/TestMotionConfig.cpp
        tests/TestErrorCodes.cpp
        tests/main_test.cpp
    )
    
    set(TEST_HEADERS
        tests/TestSimMotionController.h
        tests/TestMotionConfig.h
        tests/TestErrorCodes.h
        tests/MockLogger.h
    )
    
    # 测试可执行文件（不包含main.cpp）
    add_executable(${PROJECT_NAME}_tests ${TEST_SOURCES} ${TEST_HEADERS} ${SOURCES} ${HEADERS})
    
    target_link_libraries(${PROJECT_NAME}_tests PRIVATE 
        Qt6::Core 
        Qt6::Gui 
        Qt6::Widgets 
        Qt6::Test
    )
    
    target_compile_definitions(${PROJECT_NAME}_tests PRIVATE BUILD_TESTS)
    
    enable_testing()
    add_test(NAME MotionControlTests COMMAND ${PROJECT_NAME}_tests)
endif()
