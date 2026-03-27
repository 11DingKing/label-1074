# ✈️ 飞机大战 (Plane War)

<p align="center">
  <b>经典街机飞行射击游戏 · 现代 C++/Qt6 重制版</b>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/平台-macOS%20%7C%20Linux%20%7C%20Windows-blue" alt="平台">
  <img src="https://img.shields.io/badge/语言-C++17-green" alt="C++17">
  <img src="https://img.shields.io/badge/框架-Qt%206-41CD52" alt="Qt6">
  <img src="https://img.shields.io/badge/构建-CMake-064F8C" alt="CMake">
  <img src="https://img.shields.io/badge/许可证-MIT-yellow" alt="MIT">
</p>

---

## 📖 项目简介

**飞机大战** 是一款基于 Qt 6 框架开发的经典飞行射击游戏，致敬上世纪 80-90 年代的街机经典。

玩家驾驶战斗机，在无尽的星空中与敌机编队交战，躲避弹幕，收集道具，挑战更高分数！

### ✨ 核心特点

- 🎮 **经典玩法** - 还原街机飞机大战的核心乐趣
- 🖼️ **精美画面** - 动态星空背景、粒子特效、流畅动画
- 🎯 **丰富内容** - 多种敌机类型、道具系统、Boss 战
- 📈 **成长系统** - 关卡递进、火力升级、段位评价
- 💻 **跨平台** - 支持 macOS、Linux、Windows 三大系统
- 🚀 **傻瓜安装** - 一键启动脚本，自动处理所有依赖

---

## 🚀 快速开始

### 一键启动（推荐）

**无需任何配置！脚本会自动检测环境、安装依赖、编译并运行游戏。**

#### macOS / Linux

```bash
# 克隆项目
git clone <repo-url>
cd plane-war

# 一键启动
./start.sh
```

#### Windows

```powershell
# 克隆项目
git clone <repo-url>
cd plane-war

# 方式一：双击 start.bat

# 方式二：PowerShell (推荐)
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
.\start.ps1
```

### 启动脚本工作流程

```
┌─────────────────────────────────────────────────────────────────┐
│                      一键启动脚本                                │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ① 检测系统环境                                                  │
│     └─ 识别操作系统类型 (macOS/Ubuntu/CentOS/Arch/Windows)       │
│                                                                 │
│  ② 检查开发依赖                                                  │
│     ├─ CMake (构建工具)                                          │
│     ├─ Qt6 (图形框架)                                            │
│     └─ C++ 编译器 (Clang/GCC/MSVC)                               │
│                                                                 │
│  ③ 自动安装缺失依赖                                              │
│     ├─ macOS: brew install                                      │
│     ├─ Ubuntu: apt install                                      │
│     ├─ CentOS: dnf install                                      │
│     ├─ Arch: pacman -S                                          │
│     └─ Windows: winget install                                  │
│                                                                 │
│  ④ 编译游戏                                                      │
│     └─ CMake 配置 → 多核并行编译 → 生成可执行文件                 │
│                                                                 │
│  ⑤ 启动游戏                                                      │
│     └─ 自动运行游戏，显示操作说明                                 │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## 🎮 游戏玩法

### 操作说明

| 按键 | 功能 | 说明 |
|:----:|:----:|:-----|
| `W` `A` `S` `D` | 移动 | 控制飞机上下左右移动 |
| `↑` `↓` `←` `→` | 移动 | 方向键，与 WASD 功能相同 |
| `空格` | 射击 | 发射子弹攻击敌机 |
| `B` | 炸弹 | 释放炸弹，清除屏幕上所有敌机和子弹 |
| `ESC` / `P` | 暂停 | 暂停/恢复游戏 |
| `Enter` | 确认 | 菜单确认、重新开始 |

### 游戏目标

1. **生存** - 躲避敌机和子弹，保护自己
2. **击杀** - 消灭尽可能多的敌机获取分数
3. **升级** - 收集道具增强火力
4. **挑战** - 突破关卡，挑战 Boss，刷新最高分

### 敌机图鉴

| 类型 | 外观 | 血量 | 分数 | 特点 |
|:----:|:----:|:----:|:----:|:-----|
| **小型机** | 🔵 | 1 | 100 | 速度最快，数量多，直线俯冲 |
| **中型机** | 🟡 | 3 | 300 | 中等速度，S型移动轨迹，较难命中 |
| **大型机** | 🔴 | 10 | 1000 | 速度慢但血厚，会发射子弹反击 |
| **Boss** | 👾 | 50+ | 5000 | 每5关出现，多种攻击模式，需要策略应对 |

### 道具系统

击毁敌机有概率掉落道具，及时拾取可获得强力增益：

| 道具 | 图标 | 效果 | 持续时间 |
|:----:|:----:|:-----|:--------:|
| **生命** | ❤️ | 恢复一条生命（上限5条） | 永久 |
| **火力** | ⚡ | 火力升级，子弹更密集（最高3级） | 永久 |
| **护盾** | 🛡️ | 无敌状态，免疫所有伤害 | 5秒 |
| **炸弹** | 💣 | 获得一枚炸弹（上限5枚） | 永久 |

### 火力等级

| 等级 | 子弹形态 | 说明 |
|:----:|:---------|:-----|
| Lv.1 | 单发 | 初始火力，单发子弹 |
| Lv.2 | 双发 | 平行双发子弹 |
| Lv.3 | 三发散射 | 三发扇形散射，覆盖面最广 |

### 关卡系统

- 每消灭一定数量敌机进入下一关
- 关卡越高，敌机出现频率越高、移动越快
- **每 5 关出现 Boss 战**，击败 Boss 可获得大量分数和道具

### 段位评价

游戏结束后根据最终分数评定段位：

| 分数区间 | 段位 | 称号 |
|:--------:|:----:|:-----|
| 50,000+ | 🥇 | **传奇王牌** - 顶尖飞行员！ |
| 30,000+ | 🥈 | **超级精英** - 空战大师级别 |
| 20,000+ | 🥉 | **空战大师** - 技术精湛 |
| 10,000+ | ⭐⭐⭐ | **熟练飞行员** - 值得称赞 |
| 5,000+ | ⭐⭐ | **初级飞行员** - 崭露头角 |
| 2,000+ | ⭐ | **新手学员** - 继续努力 |
| <2,000 | 🐣 | **菜鸟入门** - 加油！ |

---

## 💻 系统支持

### 支持的操作系统

| 操作系统 | 版本要求 | 包管理器 | 启动命令 |
|:---------|:---------|:---------|:---------|
| **macOS** | 10.15+ (Catalina) | Homebrew | `./start.sh` |
| **Ubuntu** | 20.04+ | apt | `./start.sh` |
| **Debian** | 11+ | apt | `./start.sh` |
| **Linux Mint** | 20+ | apt | `./start.sh` |
| **CentOS** | 8+ | dnf | `./start.sh` |
| **Fedora** | 35+ | dnf | `./start.sh` |
| **Arch Linux** | Rolling | pacman | `./start.sh` |
| **Windows** | 10/11 | winget | `start.bat` |

### 自动安装的依赖

启动脚本会自动检测并安装以下依赖：

| 依赖 | 用途 | macOS | Linux | Windows |
|:-----|:-----|:------|:------|:--------|
| **CMake** | 构建系统 | `brew install cmake` | `apt install cmake` | `winget install Kitware.CMake` |
| **Qt6** | GUI 框架 | `brew install qt@6` | `apt install qt6-base-dev` | 手动安装 |
| **C++ 编译器** | 编译代码 | Xcode CLT (clang) | `apt install g++` | VS Build Tools |

---

## 📦 手动编译

如果你想手动编译项目，请参考以下步骤：

<details>
<summary><b>📱 macOS</b></summary>

```bash
# 1. 安装依赖
brew install qt@6 cmake

# 2. 进入项目目录
cd game
mkdir build && cd build

# 3. 配置 CMake
cmake .. -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6)

# 4. 编译
cmake --build . --parallel

# 5. 运行
open ./bin/PlaneWar.app
```
</details>

<details>
<summary><b>🐧 Ubuntu / Debian</b></summary>

```bash
# 1. 安装依赖
sudo apt update
sudo apt install -y build-essential cmake qt6-base-dev \
    libgl1-mesa-dev libxkbcommon-dev libxkbcommon-x11-0

# 2. 进入项目目录
cd game
mkdir build && cd build

# 3. 配置并编译
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel

# 4. 运行
./bin/PlaneWar
```
</details>

<details>
<summary><b>🎩 Arch Linux</b></summary>

```bash
# 1. 安装依赖
sudo pacman -S base-devel cmake qt6-base

# 2. 进入项目目录
cd game
mkdir build && cd build

# 3. 配置并编译
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel

# 4. 运行
./bin/PlaneWar
```
</details>

<details>
<summary><b>🪟 Windows</b></summary>

**前置要求：**
- Visual Studio 2022 (安装 "C++ 桌面开发" 工作负载)
- Qt 6.5+ (从 [qt.io](https://www.qt.io/download-qt-installer) 下载安装器)

```powershell
# 1. 打开 "x64 Native Tools Command Prompt for VS 2022"

# 2. 进入项目目录
cd game
mkdir build && cd build

# 3. 配置 CMake (根据你的 Qt 安装路径调整)
cmake .. -G "Visual Studio 17 2022" -A x64 ^
    -DCMAKE_PREFIX_PATH="C:\Qt\6.5.0\msvc2019_64"

# 4. 编译
cmake --build . --config Release --parallel

# 5. 部署 Qt DLL
C:\Qt\6.5.0\msvc2019_64\bin\windeployqt.exe .\bin\Release\PlaneWar.exe

# 6. 运行
.\bin\Release\PlaneWar.exe
```
</details>

---

## 📁 项目结构

```
plane-war/
│
├── 📄 start.sh                 # macOS/Linux 一键启动脚本
├── 📄 start.bat                # Windows CMD 一键启动脚本
├── 📄 start.ps1                # Windows PowerShell 一键启动脚本
├── 📄 README.md                # 项目说明文档
│
├── 📁 game/                    # 游戏主目录
│   ├── 📄 CMakeLists.txt       # CMake 构建配置
│   ├── 📄 main.cpp             # 程序入口
│   ├── 📄 Info.plist.in        # macOS 应用配置模板
│   │
│   ├── 📁 src/                 # 源代码目录
│   │   ├── 📁 config/          # 游戏配置
│   │   │   ├── GameConfig.h    # 游戏常量定义
│   │   │   └── Logger.h/cpp    # 日志系统
│   │   │
│   │   ├── 📁 core/            # 核心逻辑
│   │   │   ├── GameScene.h/cpp     # 游戏场景（主循环）
│   │   │   ├── GameWindow.h/cpp    # 主窗口管理
│   │   │   └── CollisionManager.h  # 碰撞检测
│   │   │
│   │   ├── 📁 entities/        # 游戏实体
│   │   │   ├── Player.h/cpp    # 玩家飞机
│   │   │   ├── Enemy.h/cpp     # 敌机（含 Boss）
│   │   │   ├── Bullet.h/cpp    # 子弹
│   │   │   ├── PowerUp.h/cpp   # 道具
│   │   │   └── Explosion.h/cpp # 爆炸效果
│   │   │
│   │   └── 📁 ui/              # 界面组件
│   │       ├── GameHUD.h/cpp       # 游戏内 HUD
│   │       ├── MainMenu.h/cpp      # 主菜单
│   │       ├── PauseMenu.h/cpp     # 暂停菜单
│   │       └── GameOverScreen.h/cpp # 游戏结束界面
│   │
│   └── 📁 tests/               # 单元测试
│       └── test_main.cpp
│
├── 📁 scripts/                 # 辅助脚本
│   ├── build.sh                # 构建脚本
│   ├── run.sh                  # 运行脚本
│   └── test.sh                 # 测试脚本
│
└── 📁 docs/                    # 文档
    └── project_design.md       # 设计文档
```

---

## 🛠️ 技术栈

### 核心技术

| 类别 | 技术 | 说明 |
|:-----|:-----|:-----|
| **编程语言** | C++ 17 | 现代 C++ 特性 |
| **GUI 框架** | Qt 6 | 跨平台图形界面 |
| **构建系统** | CMake | 跨平台构建配置 |
| **测试框架** | Qt Test | 单元测试 |

### 架构设计

```
┌─────────────────────────────────────────────────────────────────┐
│                         GameWindow                              │
│                      (主窗口 / 屏幕管理)                          │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────┐  │
│  │  MainMenu   │  │ PauseMenu   │  │     GameOverScreen      │  │
│  │   主菜单    │  │  暂停菜单   │  │       结算界面          │  │
│  └─────────────┘  └─────────────┘  └─────────────────────────┘  │
│                                                                 │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │                       GameScene                           │  │
│  │                    (游戏主场景)                            │  │
│  │  ┌─────────────────────────────────────────────────────┐  │  │
│  │  │                    GameHUD                          │  │  │
│  │  │              (分数/生命/炸弹显示)                    │  │  │
│  │  └─────────────────────────────────────────────────────┘  │  │
│  │                                                           │  │
│  │  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────────────┐  │  │
│  │  │ Player  │ │ Enemies │ │ Bullets │ │ PowerUps        │  │  │
│  │  │ 玩家    │ │ 敌机群  │ │ 子弹池  │ │ 道具            │  │  │
│  │  └─────────┘ └─────────┘ └─────────┘ └─────────────────┘  │  │
│  │                                                           │  │
│  │  ┌─────────────────────────────────────────────────────┐  │  │
│  │  │              CollisionManager                       │  │  │
│  │  │                (碰撞检测系统)                        │  │  │
│  │  └─────────────────────────────────────────────────────┘  │  │
│  └───────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
```

### 设计亮点

| 特性 | 说明 |
|:-----|:-----|
| 🎨 **模块化架构** | 实体、UI、核心逻辑清晰分离，易于维护和扩展 |
| ⚡ **60FPS 流畅** | 使用 QTimer 实现稳定的游戏循环，确保流畅体验 |
| 🧠 **智能指针** | 使用 `std::unique_ptr` 管理游戏对象生命周期，无内存泄漏 |
| 📝 **日志系统** | 文件+控制台双输出，便于开发调试和问题追踪 |
| ✅ **单元测试** | Qt Test 框架覆盖核心逻辑 |
| 🌍 **跨平台** | 一套代码，支持 macOS / Linux / Windows |
| 🎭 **状态管理** | 清晰的游戏状态机（菜单→游戏→暂停→结束） |
| 🎨 **视觉特效** | 粒子爆炸、屏幕震动、分数弹出、段位评价动画 |

---

## ❓ 常见问题

<details>
<summary><b>Q: macOS 上运行后看不到窗口？</b></summary>

这可能是因为应用没有获得前台焦点。请尝试：
1. 点击 Dock 栏中的 PlaneWar 图标
2. 或按 `Cmd + Tab` 切换到游戏窗口

脚本已经针对这个问题做了优化（创建 .app 包），如果仍有问题请提交 Issue。
</details>

<details>
<summary><b>Q: Windows 上 winget 安装失败？</b></summary>

确保你使用的是 Windows 10 1709 或更高版本。你也可以手动安装：
1. 从 [cmake.org](https://cmake.org/download/) 下载 CMake
2. 从 [Visual Studio](https://visualstudio.microsoft.com/) 下载安装器
3. 从 [qt.io](https://www.qt.io/download-qt-installer) 下载 Qt
</details>

<details>
<summary><b>Q: Linux 上提示 Qt 版本太低？</b></summary>

某些 Linux 发行版仓库中的 Qt 版本可能较旧。你可以：
1. 添加 Qt 官方 PPA：`sudo add-apt-repository ppa:beineri/opt-qt-6.2.4-focal`
2. 或从 [qt.io](https://www.qt.io/download-qt-installer) 下载在线安装器手动安装
</details>

<details>
<summary><b>Q: 游戏运行卡顿？</b></summary>

1. 确保没有其他大量占用 CPU/GPU 的程序在运行
2. 尝试在 Release 模式下编译：`cmake .. -DCMAKE_BUILD_TYPE=Release`
3. 检查显卡驱动是否是最新版本
</details>

<details>
<summary><b>Q: 如何重置最高分？</b></summary>

最高分保存在系统的 QSettings 位置：
- **macOS**: `~/Library/Preferences/com.planewar.game.plist`
- **Linux**: `~/.config/PlaneWar/PlaneWar.conf`
- **Windows**: 注册表 `HKEY_CURRENT_USER\Software\PlaneWar`

删除对应文件/注册表项即可重置。
</details>

---

## 🤝 贡献指南

欢迎提交 Pull Request 或 Issue！

### 开发流程

```bash
# 1. Fork 并克隆项目
git clone https://github.com/your-username/plane-war.git
cd plane-war

# 2. 创建特性分支
git checkout -b feature/your-feature

# 3. 开发并测试
./start.sh

# 4. 提交更改
git add .
git commit -m "feat: 添加新功能"

# 5. 推送并创建 PR
git push origin feature/your-feature
```

### 提交规范

| 类型 | 说明 |
|:-----|:-----|
| `feat` | 新功能 |
| `fix` | Bug 修复 |
| `docs` | 文档更新 |
| `style` | 代码格式（不影响逻辑） |
| `refactor` | 代码重构 |
| `test` | 测试相关 |
| `chore` | 构建/工具相关 |

---

## 📄 许可证

本项目采用 [MIT License](LICENSE) 开源许可证。

```
MIT License

Copyright (c) 2024

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
```

---

## 🙏 致谢

- [Qt Project](https://www.qt.io/) - 优秀的跨平台 GUI 框架
- [CMake](https://cmake.org/) - 强大的构建系统
- 经典街机游戏 - 带给我们无数美好的童年回忆

---

<p align="center">
  <b>🎮 现在就启动游戏，开始你的空战之旅！</b>
</p>

```bash
./start.sh   # macOS / Linux
start.bat    # Windows
```

<p align="center">
  Made with ❤️ and C++
</p>
