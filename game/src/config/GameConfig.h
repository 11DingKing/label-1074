#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <QColor>
#include <QString>

/**
 * 游戏配置常量类
 * 集中管理所有游戏参数，便于调整和维护
 */
namespace GameConfig {

// ==================== 窗口设置 ====================
constexpr int WINDOW_WIDTH = 480;
constexpr int WINDOW_HEIGHT = 700;
constexpr int FPS = 60;
constexpr int FRAME_TIME = 1000 / FPS;  // 毫秒

// ==================== 颜色配置 ====================
namespace Colors {
    const QColor BACKGROUND(26, 26, 46);           // #1A1A2E - 深蓝色背景
    const QColor ACCENT(233, 69, 96);              // #E94560 - 红色强调
    const QColor PRIMARY(15, 52, 96);              // #0F3460 - 蓝色主色
    const QColor HIGHLIGHT(255, 217, 61);          // #FFD93D - 金色高亮
    const QColor TEXT(234, 234, 234);              // #EAEAEA - 文字色
    const QColor TEXT_SECONDARY(150, 150, 150);    // 次要文字
    const QColor PLAYER(100, 200, 255);            // 玩家颜色
    const QColor ENEMY_SMALL(255, 100, 100);       // 小型敌机
    const QColor ENEMY_MEDIUM(255, 150, 50);       // 中型敌机
    const QColor ENEMY_LARGE(255, 50, 150);        // 大型敌机
    const QColor BOSS(200, 50, 255);               // Boss
    const QColor BULLET_PLAYER(100, 255, 200);     // 玩家子弹
    const QColor BULLET_ENEMY(255, 100, 100);      // 敌机子弹
    const QColor POWERUP_LIFE(255, 100, 150);      // 生命道具
    const QColor POWERUP_POWER(100, 200, 255);     // 火力道具
    const QColor POWERUP_SHIELD(100, 255, 200);    // 护盾道具
    const QColor POWERUP_BOMB(255, 200, 100);      // 炸弹道具
    const QColor EXPLOSION(255, 200, 100);         // 爆炸颜色
    const QColor HUD_BG(0, 0, 0, 150);             // HUD背景
    const QColor BUTTON_NORMAL(50, 80, 120);       // 按钮正常
    const QColor BUTTON_HOVER(70, 110, 160);       // 按钮悬停
    const QColor BUTTON_PRESSED(40, 60, 90);       // 按钮按下
}

// ==================== 玩家设置 ====================
namespace Player {
    constexpr int WIDTH = 50;
    constexpr int HEIGHT = 60;
    constexpr float SPEED = 8.0f;
    constexpr int INITIAL_LIVES = 3;
    constexpr int MAX_LIVES = 5;
    constexpr float FIRE_RATE = 0.15f;             // 射击间隔（秒）
    constexpr float INVINCIBLE_TIME = 2.0f;        // 无敌时间（秒）
    constexpr float BLINK_INTERVAL = 0.1f;         // 闪烁间隔
    constexpr int INITIAL_BOMBS = 3;
    constexpr int MAX_BOMBS = 5;
    constexpr int MARGIN = 10;                     // 距边界最小距离
}

// ==================== 敌机设置 ====================
namespace Enemy {
    // 小型敌机
    namespace Small {
        constexpr int WIDTH = 35;
        constexpr int HEIGHT = 30;
        constexpr int HEALTH = 1;
        constexpr int SCORE = 100;
        constexpr float SPEED = 3.0f;
    }
    
    // 中型敌机
    namespace Medium {
        constexpr int WIDTH = 50;
        constexpr int HEIGHT = 45;
        constexpr int HEALTH = 3;
        constexpr int SCORE = 300;
        constexpr float SPEED = 2.0f;
        constexpr float AMPLITUDE = 50.0f;         // 摇摆幅度
        constexpr float FREQUENCY = 2.0f;          // 摇摆频率
    }
    
    // 大型敌机
    namespace Large {
        constexpr int WIDTH = 70;
        constexpr int HEIGHT = 65;
        constexpr int HEALTH = 10;
        constexpr int SCORE = 1000;
        constexpr float SPEED = 1.5f;
        constexpr float FIRE_RATE = 1.5f;          // 射击间隔
    }
    
    // Boss
    namespace Boss {
        constexpr int WIDTH = 150;
        constexpr int HEIGHT = 120;
        constexpr int HEALTH = 50;
        constexpr int SCORE = 5000;
        constexpr float SPEED = 1.0f;
        constexpr float FIRE_RATE = 0.8f;
    }
}

// ==================== 子弹设置 ====================
namespace Bullet {
    // 玩家子弹
    namespace Player {
        constexpr int WIDTH = 6;
        constexpr int HEIGHT = 15;
        constexpr float SPEED = 12.0f;
        constexpr int DAMAGE = 1;
    }
    
    // 敌机子弹
    namespace Enemy {
        constexpr int WIDTH = 8;
        constexpr int HEIGHT = 8;
        constexpr float SPEED = 5.0f;
        constexpr int DAMAGE = 1;
    }
}

// ==================== 道具设置 ====================
namespace PowerUp {
    constexpr int WIDTH = 30;
    constexpr int HEIGHT = 30;
    constexpr float SPEED = 2.0f;
    constexpr float DROP_CHANCE = 0.15f;           // 掉落概率
    constexpr float POWER_DURATION = 10.0f;        // 火力增强持续时间
    constexpr float SHIELD_DURATION = 5.0f;        // 护盾持续时间
}

// ==================== 爆炸设置 ====================
namespace Explosion {
    constexpr int SMALL_SIZE = 40;
    constexpr int MEDIUM_SIZE = 60;
    constexpr int LARGE_SIZE = 100;
    constexpr int FRAMES = 8;
    constexpr float FRAME_TIME = 0.05f;            // 每帧时间
}

// ==================== 游戏进程 ====================
namespace GameProgress {
    constexpr int ENEMIES_PER_LEVEL = 100;         // 每关击杀数
    constexpr int BOSS_INTERVAL = 5;               // Boss出现间隔（关卡数）
    constexpr float DIFFICULTY_SCALE = 1.1f;       // 每关难度增加
    constexpr float SPAWN_INTERVAL_MIN = 0.5f;     // 最小生成间隔
    constexpr float SPAWN_INTERVAL_MAX = 2.0f;     // 最大生成间隔
}

// ==================== UI设置 ====================
namespace UI {
    constexpr int PADDING_SMALL = 8;
    constexpr int PADDING_MEDIUM = 16;
    constexpr int PADDING_LARGE = 24;
    constexpr int BORDER_RADIUS = 8;
    constexpr int BUTTON_WIDTH = 200;
    constexpr int BUTTON_HEIGHT = 50;
    constexpr int FONT_SIZE_TITLE = 36;
    constexpr int FONT_SIZE_LARGE = 24;
    constexpr int FONT_SIZE_MEDIUM = 18;
    constexpr int FONT_SIZE_SMALL = 14;
    constexpr float TRANSITION_TIME = 0.3f;        // 过渡动画时间
}

// ==================== 星空背景 ====================
namespace Stars {
    constexpr int COUNT = 100;                     // 星星数量
    constexpr float MIN_SPEED = 1.0f;
    constexpr float MAX_SPEED = 4.0f;
    constexpr int MIN_SIZE = 1;
    constexpr int MAX_SIZE = 3;
}

}  // namespace GameConfig

#endif // GAMECONFIG_H
