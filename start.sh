#!/bin/bash

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# 打印带颜色的消息
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 检测操作系统
detect_os() {
    if [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macOS"
        PACKAGE_MANAGER="brew"
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        if command -v apt-get &> /dev/null; then
            OS="Ubuntu/Debian"
            PACKAGE_MANAGER="apt"
        elif command -v dnf &> /dev/null; then
            OS="Fedora/CentOS"
            PACKAGE_MANAGER="dnf"
        elif command -v pacman &> /dev/null; then
            OS="Arch Linux"
            PACKAGE_MANAGER="pacman"
        else
            print_error "不支持的 Linux 发行版"
            exit 1
        fi
    else
        print_error "不支持的操作系统: $OSTYPE"
        exit 1
    fi
    print_info "检测到操作系统: $OS"
}

# 检查 Homebrew (macOS)
check_brew() {
    if [[ "$OS" == "macOS" ]]; then
        if ! command -v brew &> /dev/null; then
            print_warning "未检测到 Homebrew，正在安装..."
            /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
            print_success "Homebrew 安装完成"
        else
            print_success "Homebrew 已安装"
        fi
    fi
}

# 检查并安装依赖
install_dependencies() {
    print_info "检查依赖..."
    
    # 检查 CMake
    if ! command -v cmake &> /dev/null; then
        print_warning "未检测到 CMake，正在安装..."
        case $PACKAGE_MANAGER in
            brew)
                brew install cmake
                ;;
            apt)
                sudo apt-get update
                sudo apt-get install -y cmake
                ;;
            dnf)
                sudo dnf install -y cmake
                ;;
            pacman)
                sudo pacman -S --noconfirm cmake
                ;;
        esac
        print_success "CMake 安装完成"
    else
        print_success "CMake 已安装: $(cmake --version | head -n 1)"
    fi
    
    # 检查 Qt6
    if [[ "$OS" == "macOS" ]]; then
        if ! brew list qt@6 &> /dev/null 2>&1; then
            print_warning "未检测到 Qt6，正在安装..."
            brew install qt@6
            print_success "Qt6 安装完成"
        else
            print_success "Qt6 已安装"
        fi
    else
        if ! pkg-config --exists Qt6Widgets &> /dev/null; then
            print_warning "未检测到 Qt6，正在安装..."
            case $PACKAGE_MANAGER in
                apt)
                    sudo apt-get update
                    sudo apt-get install -y qt6-base-dev libgl1-mesa-dev libxkbcommon-dev libxkbcommon-x11-0
                    ;;
                dnf)
                    sudo dnf install -y qt6-qtbase-devel
                    ;;
                pacman)
                    sudo pacman -S --noconfirm qt6-base
                    ;;
            esac
            print_success "Qt6 安装完成"
        else
            print_success "Qt6 已安装"
        fi
    fi
    
    # 检查 C++ 编译器
    if ! command -v c++ &> /dev/null; then
        print_warning "未检测到 C++ 编译器，正在安装..."
        case $PACKAGE_MANAGER in
            brew)
                xcode-select --install 2>/dev/null || true
                ;;
            apt)
                sudo apt-get install -y build-essential
                ;;
            dnf)
                sudo dnf install -y gcc-c++ make
                ;;
            pacman)
                sudo pacman -S --noconfirm base-devel
                ;;
        esac
        print_success "C++ 编译器安装完成"
    else
        print_success "C++ 编译器已安装: $(c++ --version | head -n 1)"
    fi
}

# 编译项目
build_project() {
    print_info "开始编译项目..."
    
    cd game
    
    # 创建 build 目录
    if [ ! -d "build" ]; then
        mkdir build
    fi
    cd build
    
    # 配置 CMake
    print_info "配置 CMake..."
    if [[ "$OS" == "macOS" ]]; then
        cmake .. -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6) -DCMAKE_BUILD_TYPE=Release
    else
        cmake .. -DCMAKE_BUILD_TYPE=Release
    fi
    
    # 编译
    print_info "编译项目 (使用所有 CPU 核心)..."
    cmake --build . --parallel
    
    print_success "项目编译完成"
}

# 启动游戏
run_game() {
    print_info "启动游戏..."
    
    if [[ "$OS" == "macOS" ]]; then
        # macOS 上打开 .app 包
        if [ -d "bin/PlaneWar.app" ]; then
            open bin/PlaneWar.app
            print_success "游戏已启动！"
            print_info "如果游戏窗口没有显示，请点击 Dock 栏中的 PlaneWar 图标"
        else
            print_error "找不到游戏应用程序"
            exit 1
        fi
    else
        # Linux 上直接运行可执行文件
        if [ -f "bin/PlaneWar" ]; then
            ./bin/PlaneWar &
            print_success "游戏已启动！"
        else
            print_error "找不到游戏可执行文件"
            exit 1
        fi
    fi
}

# 显示游戏说明
show_game_info() {
    echo ""
    echo "========================================="
    echo "           🎮 游戏操作说明"
    echo "========================================="
    echo "  W/A/S/D 或 方向键  - 移动飞机"
    echo "  空格键              - 射击"
    echo "  B 键                - 使用炸弹"
    echo "  ESC 或 P 键         - 暂停游戏"
    echo "  Enter 键            - 确认/重新开始"
    echo "========================================="
    echo ""
}

# 主函数
main() {
    echo ""
    echo "========================================="
    echo "    ✈️  飞机大战 - 一键启动脚本"
    echo "========================================="
    echo ""
    
    detect_os
    check_brew
    install_dependencies
    
    # 检查是否已经构建过
    if [ -d "game/build/bin/PlaneWar.app" ]; then
        print_success "检测到已编译的游戏，直接启动..."
        show_game_info
        cd game/build
        run_game
    else
        build_project
        show_game_info
        run_game
    fi
    
    echo ""
    print_success "🎉 游戏启动成功！享受游戏吧！"
    echo ""
}

# 执行主函数
main
