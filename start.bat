@echo off
setlocal enabledelayedexpansion

:: ================================================
:: 🎮 飞机大战一站式启动脚本 (Windows CMD)
:: 版本: 1.0.0
:: 功能: 自动检测环境、安装依赖、编译并启动游戏
:: ================================================

echo.
echo    ____  _                  __        __         
echo   ^|  _ \^| ^| __ _ _ __   ___ \ \      / /_ _ _ __  
echo   ^| ^|_) ^| ^|/ _` ^| '_ \ / _ \ \ \ /\ / / _` ^| '__^|
echo   ^|  __/^| ^| (_^| ^| ^| ^| ^|  __/  \ V  V / (_^| ^| ^|   
echo   ^|_^|   ^|_^\__,_^|_^| ^|_^|\___^|   \_/\_/ \__,_^|_^|   
echo.
echo   ===============================================
echo            ✈️  飞机大战 - 一键启动脚本           
echo                  版本 1.0.0                      
echo   ===============================================
echo.

:: 项目配置
set "PROJECT_NAME=飞机大战"
set "PROJECT_DIR=%~dp0"
set "GAME_DIR=%PROJECT_DIR%game"
set "BUILD_DIR=%GAME_DIR%\build"

:: 检查 PowerShell 是否可用
powershell -Command "exit 0" >nul 2>&1
if %errorlevel% equ 0 (
    echo [信息] 检测到 PowerShell，正在启动 PowerShell 版本...
    echo.
    powershell -ExecutionPolicy Bypass -File "%~dp0start.ps1"
    goto :end
)

:: 如果没有 PowerShell，使用纯批处理
echo [警告] 未检测到 PowerShell，使用简化版启动脚本
echo.

:: 检查依赖
echo [信息] 检查依赖...

where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo [错误] 未找到 CMake
    echo [信息] 请从 https://cmake.org/download/ 下载安装 CMake
    pause
    exit /b 1
)

:: 检测 Visual Studio 2022
set "VCVARS_PATH="
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
    set "VCVARS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" (
    set "VCVARS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" (
    set "VCVARS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
)

if not defined VCVARS_PATH (
    echo [错误] 未找到 Visual Studio 2022
    echo [信息] 请从 https://visualstudio.microsoft.com/ 下载安装 Visual Studio 2022
    pause
    exit /b 1
)

:: 检测 Qt 6
set "QT_PATH="
if exist "C:\Qt\6.7.0\msvc2019_64" set "QT_PATH=C:\Qt\6.7.0\msvc2019_64"
if exist "C:\Qt\6.6.3\msvc2019_64" set "QT_PATH=C:\Qt\6.6.3\msvc2019_64"
if exist "C:\Qt\6.6.2\msvc2019_64" set "QT_PATH=C:\Qt\6.6.2\msvc2019_64"
if exist "C:\Qt\6.5.3\msvc2019_64" set "QT_PATH=C:\Qt\6.5.3\msvc2019_64"
if exist "C:\Qt\6.5.2\msvc2019_64" set "QT_PATH=C:\Qt\6.5.2\msvc2019_64"

if not defined QT_PATH (
    echo [错误] 未找到 Qt 6
    echo [信息] 请从 https://www.qt.io/download-qt-installer 下载安装 Qt 6.5+
    pause
    exit /b 1
)

echo [成功] 所有依赖已检测 ✓
echo.

:: 清理旧的构建目录（解决 CMake 缓存不匹配问题）
if exist "%BUILD_DIR%" (
    echo [信息] 清理旧的构建目录...
    rmdir /s /q "%BUILD_DIR%"
)

:: 创建新的构建目录
mkdir "%BUILD_DIR%"

:: 配置编译环境
echo [信息] 配置 Visual Studio 2022 编译环境...
call "%VCVARS_PATH%"

:: 配置 CMake
echo [信息] 配置 CMake...
cd "%BUILD_DIR%"
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="%QT_PATH%" -DCMAKE_BUILD_TYPE=Release

if %errorlevel% neq 0 (
    echo [错误] CMake 配置失败
    pause
    exit /b 1
)

:: 编译
echo [信息] 开始编译...
cmake --build . --config Release --parallel %NUMBER_OF_PROCESSORS%

if %errorlevel% neq 0 (
    echo [错误] 编译失败
    pause
    exit /b 1
)

:: 启动游戏
echo [成功] 编译完成!
echo [信息] 启动游戏...

set "EXECUTABLE=%BUILD_DIR%\bin\Release\PlaneWar.exe"
if exist "%EXECUTABLE%" (
    start "" "%EXECUTABLE%"
    echo.
    echo ========================================
    echo 🎉 游戏已成功启动！
    echo ========================================
    echo.
    echo 游戏操作说明:
    echo   WASD / 方向键  - 移动飞机
    echo   空格键         - 射击
    echo   B 键           - 使用炸弹
    echo   ESC / P 键     - 暂停游戏
    echo   Enter 键       - 确认/重新开始
    echo.
) else (
    echo [错误] 无法找到游戏可执行文件
    pause
    exit /b 1
)

:end
pause
