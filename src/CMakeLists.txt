# 定义源文件
set(SOURCES
    main.cpp
    app/MainWindow.cpp
    ui/JogPanel.cpp
    ui/StatusPanel.cpp
    core/MotionManager.cpp
    hal/SimMotionController.cpp
    # hal/RealMotionController.cpp # 真实硬件时取消注释
)

set(HEADERS
    app/MainWindow.h
    ui/JogPanel.hh
    ui/StatusPanel.hh
    core/MotionManager.hh
    hal/IMotionController.hh
    hal/SimMotionController.hh
    # hal/RealMotionController.h
)

# 创建可执行文件
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})

# 链接 Qt 库
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets)
# 真实硬件时，链接供应商提供的 .lib 文件
# target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/libs/vendor_sdk.lib)
