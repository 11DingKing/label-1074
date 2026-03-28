# Windows PowerShell 一键启动脚本
# 飞机大战 - Plane War

# 设置编码为 UTF-8
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
$OutputEncoding = [System.Text.Encoding]::UTF8

# 颜色定义
$INFO = "INFO"
$SUCCESS = "SUCCESS"
$WARNING = "WARNING"
$ERROR = "ERROR"

# 颜色输出函数
function Write-Info { param($Message) Write-Host "[$INFO] $Message" -ForegroundColor Cyan }
function Write-Success { param($Message) Write-Host "[$SUCCESS] $Message" -ForegroundColor Green }
function Write-Warning { param($Message) Write-Host "[$WARNING] $Message" -ForegroundColor Yellow }
function Write-Error { param($Message) Write-Host "[$ERROR] $Message" -ForegroundColor Red }

# 显示欢迎信息
Write-Host ""
Write-Host "========================================" -ForegroundColor Blue
Write-Host "  飞机大战 - Plane War 一键启动脚本" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Blue
Write-Host ""

Start-Sleep -Seconds 1

# 检查是否以管理员权限运行
Write-Info "检查管理员权限..."
$currentUser = [Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()
$isAdmin = $currentUser.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)

if (-not $isAdmin) {
    Write-Warning "建议以管理员权限运行此脚本，否则可能无法安装依赖"
    Write-Warning "是否继续运行? (Y/N)"
    $choice = Read-Host
    if ($choice -ne "Y" -and $choice -ne "y") {
        Write-Info "脚本已退出"
        exit 0
    }
}
Write-Success "权限检查完成"
Write-Host ""

# 检查 winget
Write-Info "检查 winget 包管理器..."
try {
    $wingetExists = Get-Command winget -ErrorAction Stop
    Write-Success "winget 已安装"
} catch {
    Write-Error "未检测到 winget"
    Write-Warning "请确保您使用的是 Windows 10 1709 或更高版本"
    Write-Warning "您可以手动安装以下依赖:"
    Write-Warning "  1. CMake: https://cmake.org/download/"
    Write-Warning "  2. Visual Studio Build Tools: https://visualstudio.microsoft.com/downloads/"
    Write-Warning "  3. Qt6: https://www.qt.io/download-qt-installer"
    Write-Host ""
    Read-Host "按 Enter 键退出"
    exit 1
}
Write-Host ""

# 检查 CMake
Write-Info "检查 CMake..."
try {
    $cmakeExists = Get-Command cmake -ErrorAction Stop
    $cmakeVersion = (cmake --version | Select-Object -First 1)
    Write-Success "CMake 已安装: $cmakeVersion"
} catch {
    Write-Warning "未检测到 CMake，正在安装..."
    try {
        winget install Kitware.CMake -e --accept-package-agreements --accept-source-agreements
        Write-Success "CMake 安装完成"
        $env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
    } catch {
        Write-Error "CMake 安装失败"
        Read-Host "按 Enter 键退出"
        exit 1
    }
}
Write-Host ""

# 检查 Visual Studio Build Tools
Write-Info "检查 C++ 编译器..."
try {
    $clExists = Get-Command cl -ErrorAction Stop
    Write-Success "Visual Studio 编译器已安装"
} catch {
    Write-Warning "未检测到 C++ 编译器，正在安装 Visual Studio Build Tools..."
    Write-Warning "这可能需要几分钟时间..."
    try {
        winget install Microsoft.VisualStudio.2022.BuildTools -e --accept-package-agreements --accept-source-agreements --override "--wait --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended"
        Write-Success "Visual Studio Build Tools 安装完成"
    } catch {
        Write-Error "Visual Studio Build Tools 安装失败"
        Read-Host "按 Enter 键退出"
        exit 1
    }
}
Write-Host ""

# 检查 Qt6
Write-Info "检查 Qt6..."
$qtPaths = @(
    "C:\Qt\6.*\msvc2019_64",
    "C:\Qt\6.*\msvc2022_64"
)

$qtPath = $null
foreach ($pathPattern in $qtPaths) {
    $foundPaths = Get-Item $pathPattern -ErrorAction SilentlyContinue
    if ($foundPaths) {
        $qtPath = $foundPaths[-1].FullName
        break
    }
}

if (-not $qtPath) {
    Write-Error "未检测到 Qt6"
    Write-Warning "请手动安装 Qt6:"
    Write-Warning "  1. 访问 https://www.qt.io/download-qt-installer"
    Write-Warning "  2. 下载并运行 Qt 安装程序"
    Write-Warning "  3. 安装时选择 MSVC 2019/2022 64-bit 组件"
    Write-Warning "  4. 安装完成后重新运行此脚本"
    Write-Host ""
    Read-Host "按 Enter 键退出"
    exit 1
}

Write-Success "Qt6 已找到: $qtPath"
Write-Host ""

# 设置环境变量
$env:PATH = "$qtPath\bin;$env:PATH"

# 编译项目
Write-Info "开始编译项目..."
Start-Sleep -Seconds 1

$buildDir = "game\build"
$gameDir = "game"

# 创建构建目录
if (-not (Test-Path $buildDir)) {
    New-Item -ItemType Directory -Path $buildDir -Force | Out-Null
}

# 配置 CMake
Write-Info "配置 CMake..."
Push-Location $buildDir
try {
    cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="$qtPath"
    if ($LASTEXITCODE -ne 0) {
        throw "CMake 配置失败"
    }
    Write-Success "CMake 配置完成"

    # 编译
    Write-Info "编译项目 (Release 模式)..."
    cmake --build . --config Release --parallel
    if ($LASTEXITCODE -ne 0) {
        throw "编译失败"
    }
    Write-Success "编译完成"

    # 部署 Qt DLL
    Write-Info "部署 Qt 依赖库..."
    $windeployqt = Join-Path $qtPath "bin\windeployqt.exe"
    $exePath = ".\bin\Release\PlaneWar.exe"
    if (Test-Path $windeployqt) {
        & $windeployqt $exePath
        if ($LASTEXITCODE -ne 0) {
            Write-Warning "Qt 部署可能有问题，但游戏仍可能运行"
        }
        Write-Success "Qt 依赖库部署完成"
    } else {
        Write-Warning "未找到 windeployqt.exe"
    }
} finally {
    Pop-Location
}

Write-Host ""

# 显示游戏信息
Write-Host "========================================" -ForegroundColor Blue
Write-Host "  游戏操作说明" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Blue
Write-Host ""
Write-Host "  移动: W A S D 或 方向键 ↑↓←→" -ForegroundColor White
Write-Host "  射击: 空格键" -ForegroundColor White
Write-Host "  炸弹: B 键" -ForegroundColor White
Write-Host "  暂停: ESC 或 P 键" -ForegroundColor White
Write-Host "  确认: Enter 键" -ForegroundColor White
Write-Host ""
Write-Host "========================================" -ForegroundColor Blue
Write-Host ""

# 启动游戏
Write-Info "启动游戏..."
Start-Sleep -Seconds 1

$exeFullPath = Join-Path $buildDir "bin\Release\PlaneWar.exe"
if (Test-Path $exeFullPath) {
    Write-Success "游戏启动成功！"
    Write-Host ""
    Start-Process $exeFullPath
} else {
    Write-Error "未找到游戏可执行文件"
    Read-Host "按 Enter 键退出"
    exit 1
}

Write-Host ""
Write-Success "感谢使用飞机大战一键启动脚本！"
Write-Host ""
Start-Sleep -Seconds 2