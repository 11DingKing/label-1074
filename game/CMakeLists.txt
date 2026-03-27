cmake_minimum_required(VERSION 3.16)

project(PlaneWar VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# 构建选项
option(BUILD_TESTS "Build unit tests" OFF)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
if(BUILD_TESTS)
    find_package(Qt6 REQUIRED COMPONENTS Test)
endif()

# Source files
set(SOURCES
    src/core/GameWindow.cpp
    src/core/GameScene.cpp
    src/core/AnimationManager.cpp
    src/core/Logger.cpp
    src/entities/Player.cpp
    src/entities/Enemy.cpp
    src/entities/Bullet.cpp
    src/entities/Explosion.cpp
    src/entities/PowerUp.cpp
    src/ui/StartScreen.cpp
    src/ui/GameHUD.cpp
    src/ui/PauseScreen.cpp
    src/ui/GameOverScreen.cpp
)

# Header files
set(HEADERS
    src/config/GameConfig.h
    src/core/GameObject.h
    src/core/GameWindow.h
    src/core/GameScene.h
    src/core/AnimationManager.h
    src/core/Logger.h
    src/entities/Player.h
    src/entities/Enemy.h
    src/entities/Bullet.h
    src/entities/Explosion.h
    src/entities/PowerUp.h
    src/ui/StartScreen.h
    src/ui/GameHUD.h
    src/ui/PauseScreen.h
    src/ui/GameOverScreen.h
)

# Common include directories
set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/config
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/entities
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
)

# Create main executable
# macOS: 创建 .app bundle 以确保窗口正确显示
if(APPLE)
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE
        main.cpp
        ${SOURCES}
        ${HEADERS}
    )
    
    # macOS bundle 配置
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "PlaneWar"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.planewar.game"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
    )
else()
    add_executable(${PROJECT_NAME}
        main.cpp
        ${SOURCES}
        ${HEADERS}
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Build tests if enabled
if(BUILD_TESTS)
    message(STATUS "Building tests...")
    
    # Test source files
    set(TEST_SOURCES
        tests/test_main.cpp
    )
    
    set(TEST_HEADERS
        tests/test_gameobject.h
        tests/test_player.h
        tests/test_enemy.h
        tests/test_collision.h
        tests/test_logger.h
    )
    
    # Create test executable
    add_executable(PlaneWarTests
        ${TEST_SOURCES}
        ${TEST_HEADERS}
        ${SOURCES}
        ${HEADERS}
    )
    
    target_include_directories(PlaneWarTests PRIVATE 
        ${INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
    )
    
    target_link_libraries(PlaneWarTests PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Test
    )
    
    set_target_properties(PlaneWarTests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
    
    # Enable testing
    enable_testing()
    add_test(NAME PlaneWarTests COMMAND PlaneWarTests)
endif()

# Installation
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)

# Print configuration summary
message(STATUS "")
message(STATUS "===== Plane War Build Configuration =====")
message(STATUS "  Version:      ${PROJECT_VERSION}")
message(STATUS "  Build Type:   ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Qt Version:   ${Qt6_VERSION}")
message(STATUS "  Build Tests:  ${BUILD_TESTS}")
message(STATUS "=========================================")
message(STATUS "")
