#!/bin/bash
set -e

# ================================================
# 🎮 飞机大战一站式启动脚本 (macOS/Linux)
# 版本: 1.0.0
# 功能: 自动检测环境、安装依赖、编译并启动游戏
# ================================================

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# 项目配置
PROJECT_NAME="飞机大战"
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
GAME_DIR="$PROJECT_DIR/game"
BUILD_DIR="$GAME_DIR/build"

# ================================================
# 工具函数
# ================================================

info() {
    echo -e "${BLUE}[信息]${NC} $1"
}

success() {
    echo -e "${GREEN}[成功]${NC} $1"
}

warning() {
    echo -e "${YELLOW}[警告]${NC} $1"
}

error() {
    echo -e "${RED}[错误]${NC} $1"
}

title() {
    echo -e "\n${PURPLE}========================================${NC}"
    echo -e "${PURPLE}$1${NC}"
    echo -e "${PURPLE}========================================${NC}\n"
}

# ================================================
# 检测操作系统
# ================================================

detect_os() {
    local os="unknown"
    
    if [[ "$OSTYPE" == "darwin"* ]]; then
        os="macos"
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        if command -v apt-get >/dev/null 2>&1; then
            os="ubuntu"
        elif command -v dnf >/dev/null 2>&1; then
            os="fedora"
        elif command -v pacman >/dev/null 2>&1; then
            os="arch"
        fi
    fi
    
    echo $os
}

# ================================================
# 检查依赖是否安装
# ================================================

check_command() {
    command -v "$1" >/dev/null 2>&1
}

# ================================================
# 安装 Homebrew (macOS)
# ================================================

install_homebrew() {
    title "🍺 安装 Homebrew 包管理器"
    
    if ! check_command brew; then
        info "正在安装 Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
        
        # 配置环境变量
        if [[ "$(uname -m)" == "arm64" ]]; then
            echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> ~/.zprofile
            eval "$(/opt/homebrew/bin/brew shellenv)"
        fi
        
        success "Homebrew 安装完成!"
    else
        success "Homebrew 已安装 ✓"
    fi
}

# ================================================
# 安装依赖
# ================================================

install_dependencies() {
    title "📦 安装项目依赖"
    
    local os=$(detect_os)
    local need_install=()
    
    info "检测操作系统: $os"
    echo ""
    
    # 检测缺失的依赖
    if ! check_command cmake; then need_install+=("cmake"); fi
    if ! check_command g++; then need_install+=("gcc"); fi
    
    # 检测 Qt6
    if ! check_command qmake6 && ! check_command qmake; then
        if [[ "$os" == "macos" ]]; then
            if ! brew list qt@6 >/dev/null 2>&1; then
                need_install+=("qt@6")
            fi
        else
            need_install+=("qt6-base-dev")
        fi
    fi
    
    if [[ ${#need_install[@]} -eq 0 ]]; then
        success "所有依赖已安装 ✓"
        return 0
    fi
    
    info "需要安装以下依赖: ${need_install[*]}"
    echo ""
    
    case $os in
        macos)
            install_homebrew
            info "使用 Homebrew 安装依赖..."
            brew install "${need_install[@]}"
            ;;
        ubuntu)
            info "使用 apt 安装依赖..."
            sudo apt update
            sudo apt install -y "${need_install[@]}"
            ;;
        fedora)
            info "使用 dnf 安装依赖..."
            sudo dnf install -y "${need_install[@]}"
            ;;
        arch)
            info "使用 pacman 安装依赖..."
            sudo pacman -S --noconfirm "${need_install[@]}"
            ;;
        *)
            error "不支持的操作系统: $os"
            exit 1
            ;;
    esac
    
    success "依赖安装完成!"
}

# ================================================
# 编译项目
# ================================================

build_project() {
    title "🔨 编译游戏"
    
    cd "$GAME_DIR"
    
    # 清理旧的构建目录（解决 CMake 缓存不匹配问题）
    if [ -d "$BUILD_DIR" ]; then
        info "清理旧的构建目录..."
        rm -rf "$BUILD_DIR"
    fi
    
    # 创建新的构建目录
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # 配置 CMake
    info "配置 CMake..."
    
    local qt_path=""
    if [[ "$(detect_os)" == "macos" ]]; then
        if brew list qt@6 >/dev/null 2>&1; then
            qt_path=$(brew --prefix qt@6)
        fi
    fi
    
    if [ -n "$qt_path" ]; then
        cmake .. -DCMAKE_PREFIX_PATH="$qt_path" -DCMAKE_BUILD_TYPE=Release
    else
        cmake .. -DCMAKE_BUILD_TYPE=Release
    fi
    
    if [ $? -ne 0 ]; then
        error "CMake 配置失败"
        exit 1
    fi
    
    # 编译
    info "开始编译 (使用 $(nproc) 个核心)..."
    cmake --build . --parallel $(nproc)
    
    if [ $? -ne 0 ]; then
        error "编译失败"
        exit 1
    fi
    
    success "编译完成!"
}

# ================================================
# 启动游戏
# ================================================

start_game() {
    title "🚀 启动 $PROJECT_NAME"
    
    cd "$BUILD_DIR"
    
    local executable=""
    local os=$(detect_os)
    
    if [[ "$os" == "macos" ]]; then
        executable="$BUILD_DIR/bin/PlaneWar.app/Contents/MacOS/PlaneWar"
        if [ -f "$executable" ]; then
            info "找到应用程序: PlaneWar.app"
            success "游戏正在启动..."
            open "$BUILD_DIR/bin/PlaneWar.app"
            return 0
        fi
    else
        executable="$BUILD_DIR/bin/PlaneWar"
        if [ -f "$executable" ]; then
            info "找到可执行文件: PlaneWar"
            success "游戏正在启动..."
            "$executable"
            return 0
        fi
    fi
    
    error "无法找到游戏可执行文件"
    exit 1
}

# ================================================
# 主程序
# ================================================

main() {
    clear
    
    echo -e "${CYAN}"
    echo "   ____  _                  __        __         "
    echo "  |  _ \| | __ _ _ __   ___ \ \      / /_ _ _ __  "
    echo "  | |_) | |/ _\` | '_ \ / _ \ \ \ /\ / / _\` | '__|"
    echo "  |  __/| | (_| | | | |  __/  \ V  V / (_| | |   "
    echo "  |_|   |_|\__,_|_| |_|\___|   \_/\_/ \__,_|_|   "
    echo -e "${NC}"
    
    echo -e "${YELLOW}"
    echo "  ==============================================="
    echo "           ✈️  飞机大战 - 一键启动脚本           "
    echo "                 版本 1.0.0                      "
    echo "  ==============================================="
    echo -e "${NC}\n"
    
    install_dependencies
    build_project
    start_game
    
    echo -e "\n${GREEN}========================================${NC}"
    echo -e "${GREEN}🎉 游戏已成功启动！${NC}"
    echo -e "${GREEN}========================================${NC}\n"
    
    echo -e "${CYAN}游戏操作说明:${NC}"
    echo -e "  WASD / 方向键  - 移动飞机"
    echo -e "  空格键         - 射击"
    echo -e "  B 键           - 使用炸弹"
    echo -e "  ESC / P 键     - 暂停游戏"
    echo -e "  Enter 键       - 确认/重新开始\n"
}

# 运行主程序
main "$@"
