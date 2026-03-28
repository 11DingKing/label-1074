@echo off
setlocal enabledelayedexpansion

REM Windows CMD 一键启动脚本
REM Plane War - 飞机大战

echo.
echo ========================================
echo     Plane War - One Click Start
echo ========================================
echo.

REM Check admin privileges
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo [WARNING] It is recommended to run this script as administrator
    echo.
)

REM Check winget
where winget >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] winget not found
    echo.
    echo Please ensure you are using Windows 10 1709 or later
    echo.
    pause
    exit /b 1
)

echo [INFO] Checking dependencies...
echo.

REM Check CMake
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo [WARNING] CMake not found, installing...
    winget install --id=Kitware.CMake --silent --accept-package-agreements --accept-source-agreements
    echo [SUCCESS] CMake installed
) else (
    echo [SUCCESS] CMake found
)

REM Check Visual Studio compiler
where cl >nul 2>&1
if %errorlevel% neq 0 (
    echo [WARNING] Visual Studio Build Tools not found, installing...
    echo [INFO] This may take a few minutes...
    winget install --id=Microsoft.VisualStudio.2022.BuildTools --silent --accept-package-agreements --accept-source-agreements --override "--wait --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended"
    echo [SUCCESS] Visual Studio Build Tools installed
) else (
    echo [SUCCESS] C++ compiler found
)

REM Check Qt6
set QT_PATH=
for /d %%d in (C:\Qt\6.*) do (
    if exist "%%d\msvc2019_64\bin\qmake.exe" (
        set QT_PATH=%%d\msvc2019_64
    )
    if exist "%%d\msvc2022_64\bin\qmake.exe" (
        set QT_PATH=%%d\msvc2022_64
    )
)

if "%QT_PATH%"=="" (
    echo [ERROR] Qt6 not found
    echo.
    echo Please install Qt6 manually:
    echo 1. Visit https://www.qt.io/download-qt-installer
    echo 2. Download and run Qt installer
    echo 3. Select MSVC 2019/2022 64-bit component
    echo 4. Rerun this script after installation
    echo.
    pause
    exit /b 1
)

echo [SUCCESS] Qt6 found: %QT_PATH%
echo.

REM Build project
echo [INFO] Building project...
echo.

cd /d "%~dp0game"

if not exist "build" (
    mkdir build
)
cd build

echo [INFO] Configuring CMake...
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="%QT_PATH%"
if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed
    pause
    exit /b 1
)

echo [INFO] Building project (Release mode)...
cmake --build . --config Release --parallel
if %errorlevel% neq 0 (
    echo [ERROR] Build failed
    pause
    exit /b 1
)

echo [SUCCESS] Build completed
echo.

REM Deploy Qt DLLs
echo [INFO] Deploying Qt dependencies...
if exist "%QT_PATH%\bin\windeployqt.exe" (
    "%QT_PATH%\bin\windeployqt.exe" ".\bin\Release\PlaneWar.exe"
    echo [SUCCESS] Qt dependencies deployed
) else (
    echo [WARNING] windeployqt.exe not found
)
echo.

REM Show game info
echo ========================================
echo          Game Controls
echo ========================================
echo   Move: W A S D or Arrow Keys
echo   Shoot: Spacebar
echo   Bomb: B Key
echo   Pause: ESC or P Key
echo   Confirm: Enter Key
echo ========================================
echo.

REM Start the game
echo [INFO] Starting game...
if exist ".\bin\Release\PlaneWar.exe" (
    echo [SUCCESS] Game started!
    echo.
    start "" ".\bin\Release\PlaneWar.exe"
) else (
    echo [ERROR] Game executable not found
    pause
    exit /b 1
)

echo.
echo [SUCCESS] Thank you for using Plane War One Click Starter!
echo.
timeout /t 3 >nul