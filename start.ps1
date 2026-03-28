<#
================================================
🎮 飞机大战一站式启动脚本 (Windows PowerShell)
版本: 1.0.0
功能: 自动检测环境、安装依赖、编译并启动游戏
================================================
#>

param([switch]$Help)

# 颜色定义
$Red = "Red"
$Green = "Green"
$Yellow = "Yellow"
$Blue = "Blue"
$Cyan = "Cyan"
$Magenta = "Magenta"

# 项目配置
$PROJECT_NAME = "飞机大战"
$PROJECT_DIR = Split-Path -Parent $MyInvocation.MyCommand.Definition
$GAME_DIR = Join-Path $PROJECT_DIR "game"
$BUILD_DIR = Join-Path $GAME_DIR "build"

# ================================================
# 工具函数
# ================================================

function Write-Info { param($Message) Write-Host "[信息] " -ForegroundColor $Blue -NoNewline; Write-Host $Message }
function Write-Success { param($Message) Write-Host "[成功] " -ForegroundColor $Green -NoNewline; Write-Host $Message }
function Write-Warning { param($Message) Write-Host "[警告] " -ForegroundColor $Yellow -NoNewline; Write-Host $Message }
function Write-Error { param($Message) Write-Host "[错误] " -ForegroundColor $Red -NoNewline; Write-Host $Message }

function Write-Title {
    param($Message)
    Write-Host "`n========================================" -ForegroundColor $Magenta
    Write-Host $Message -ForegroundColor $Magenta
    Write-Host "========================================`n" -ForegroundColor $Magenta
}

function Test-Command {
    param($Command)
    return [bool](Get-Command $Command -ErrorAction SilentlyContinue)
}

# ================================================
# 显示帮助
# ================================================

if ($Help) {
    Write-Host "`n✈️  $PROJECT_NAME 一键启动脚本`n" -ForegroundColor $Cyan
    Write-Host "使用方法:"
    Write-Host "  .\start.ps1           # 一键启动游戏"
    Write-Host "  .\start.ps1 -Help     # 显示此帮助信息`n"
    Write-Host "游戏操作说明:"
    Write-Host "  WASD / 方向键  - 移动飞机"
    Write-Host "  空格键         - 射击"
    Write-Host "  B 键           - 使用炸弹"
    Write-Host "  ESC / P 键     - 暂停游戏"
    Write-Host "  Enter 键       - 确认/重新开始`n"
    exit 0
}

# ================================================
# 检查 winget
# ================================================

function Test-Winget {
    if (Test-Command winget) {
        return $true
    }
    
    Write-Warning "未检测到 winget 包管理器"
    Write-Info "winget 是 Windows 10 1709+ 的官方包管理器"
    Write-Info "请从 Microsoft Store 安装 'App Installer' 以获取 winget"
    Write-Info "或者手动安装以下依赖: CMake, Visual Studio 2022, Qt 6`n"
    
    $choice = Read-Host "是否继续手动配置？(Y/N)"
    if ($choice -notmatch "^[Yy]") {
        exit 1
    }
    
    return $false
}

# ================================================
# 安装依赖
# ================================================

function Install-Dependencies {
    Write-Title "📦 安装项目依赖"
    
    $hasWinget = Test-Winget
    $needInstall = @()
    
    # 检测缺失的依赖
    if (-not (Test-Command cmake)) { $needInstall += "Kitware.CMake" }
    
    # 检测 Visual Studio 2022
    $vsPaths = @(
        "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat",
        "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat",
        "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
    )
    
    $vsFound = $false
    foreach ($path in $vsPaths) {
        if (Test-Path $path) {
            $vsFound = $true
            $global:VCVARS_PATH = $path
            break
        }
    }
    
    if (-not $vsFound) {
        if ($hasWinget) {
            $needInstall += "Microsoft.VisualStudio.2022.BuildTools"
        } else {
            Write-Warning "需要 Visual Studio 2022"
            Write-Info "请从 https://visualstudio.microsoft.com/ 下载并安装"
            Write-Info "安装时请选择 'C++ 桌面开发' 工作负载`n"
        }
    } else {
        Write-Success "Visual Studio 2022 已安装 ✓"
    }
    
    # 检测 Qt 6
    $qtPaths = @(
        "C:\Qt\6.7.0\msvc2019_64",
        "C:\Qt\6.6.3\msvc2019_64",
        "C:\Qt\6.6.2\msvc2019_64",
        "C:\Qt\6.5.3\msvc2019_64",
        "C:\Qt\6.5.2\msvc2019_64"
    )
    
    $qtFound = $false
    foreach ($path in $qtPaths) {
        if (Test-Path (Join-Path $path "lib\cmake\Qt6\Qt6Config.cmake")) {
            $qtFound = $true
            $global:QT_PATH = $path
            break
        }
    }
    
    if (-not $qtFound) {
        Write-Warning "未检测到 Qt 6"
        Write-Info "请从 https://www.qt.io/download-qt-installer 下载 Qt 6.5+"
        Write-Info "安装时请选择 MSVC 2019 64-bit 组件`n"
    } else {
        Write-Success "Qt 6 已安装 ✓"
    }
    
    if ($needInstall.Count -gt 0 -and $hasWinget) {
        Write-Info "需要安装以下依赖: $($needInstall -join ', ')`n"
        
        foreach ($package in $needInstall) {
            Write-Info "正在安装 $package..."
            winget install --id $package --silent --accept-package-agreements --accept-source-agreements
        }
        
        # 刷新环境变量
        $env:Path = [System.Environment]::GetEnvironmentVariable("Path", "Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path", "User")
    }
    
    Write-Success "依赖检查完成!"
}

# ================================================
# 编译项目
# ================================================

function Build-Project {
    Write-Title "🔨 编译游戏"
    
    if (-not $global:VCVARS_PATH) {
        Write-Error "未找到 Visual Studio 2022 编译环境"
        exit 1
    }
    
    if (-not $global:QT_PATH) {
        Write-Error "未找到 Qt 6 安装路径"
        Write-Info "请确保 Qt 6 已安装在默认路径 (C:\Qt\6.x.x\msvc2019_64)"
        exit 1
    }
    
    # 清理旧的构建目录（解决 CMake 缓存不匹配问题）
    if (Test-Path $BUILD_DIR) {
        Write-Info "清理旧的构建目录..."
        Remove-Item -Path $BUILD_DIR -Recurse -Force | Out-Null
    }
    
    # 创建新的构建目录
    New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null
    Set-Location $BUILD_DIR
    
    # 设置编译环境
    Write-Info "配置 Visual Studio 2022 编译环境..."
    cmd /c "`"$global:VCVARS_PATH`" && cmake .. -G `"Visual Studio 17 2022`" -A x64 -DCMAKE_PREFIX_PATH=`"$global:QT_PATH`" -DCMAKE_BUILD_TYPE=Release"
    
    if ($LASTEXITCODE -ne 0) {
        Write-Error "CMake 配置失败"
        exit 1
    }
    
    # 编译
    Write-Info "开始编译..."
    cmake --build . --config Release --parallel $env:NUMBER_OF_PROCESSORS
    
    if ($LASTEXITCODE -ne 0) {
        Write-Error "编译失败"
        exit 1
    }
    
    # 部署 Qt DLL
    $executable = Join-Path $BUILD_DIR "bin\Release\PlaneWar.exe"
    if (Test-Path $executable) {
        Write-Info "部署 Qt 依赖..."
        $windeployqt = Join-Path $global:QT_PATH "bin\windeployqt.exe"
        if (Test-Path $windeployqt) {
            & $windeployqt $executable --release --no-compiler-runtime
        }
    }
    
    Write-Success "编译完成!"
}

# ================================================
# 启动游戏
# ================================================

function Start-Game {
    Write-Title "🚀 启动 $PROJECT_NAME"
    
    $executable = Join-Path $BUILD_DIR "bin\Release\PlaneWar.exe"
    
    if (Test-Path $executable) {
        Write-Success "游戏正在启动..."
        Start-Process $executable
        return 0
    }
    
    Write-Error "无法找到游戏可执行文件"
    exit 1
}

# ================================================
# 主程序
# ================================================

function Main {
    Clear-Host
    
    Write-Host "`n   ____  _                  __        __         " -ForegroundColor $Cyan
    Write-Host "  |  _ \| | __ _ _ __   ___ \ \      / /_ _ _ __  " -ForegroundColor $Cyan
    Write-Host "  | |_) | |/ _`` | '_ \ / _ \ \ \ /\ / / _`` | '__|" -ForegroundColor $Cyan
    Write-Host "  |  __/| | (_| | | | |  __/  \ V  V / (_| | |   " -ForegroundColor $Cyan
    Write-Host "  |_|   |_|\__,_|_| |_|\___|   \_/\_/ \__,_|_|   " -ForegroundColor $Cyan
    Write-Host "`n"
    
    Write-Host "  ===============================================" -ForegroundColor $Yellow
    Write-Host "           ✈️  飞机大战 - 一键启动脚本           " -ForegroundColor $Yellow
    Write-Host "                 版本 1.0.0                      " -ForegroundColor $Yellow
    Write-Host "  ===============================================`n" -ForegroundColor $Yellow
    
    Install-Dependencies
    Build-Project
    Start-Game
    
    Write-Host "`n========================================" -ForegroundColor $Green
    Write-Host "🎉 游戏已成功启动！" -ForegroundColor $Green
    Write-Host "========================================`n" -ForegroundColor $Green
    
    Write-Host "游戏操作说明:" -ForegroundColor $Cyan
    Write-Host "  WASD / 方向键  - 移动飞机"
    Write-Host "  空格键         - 射击"
    Write-Host "  B 键           - 使用炸弹"
    Write-Host "  ESC / P 键     - 暂停游戏"
    Write-Host "  Enter 键       - 确认/重新开始`n"
}

# 运行主程序
Main
