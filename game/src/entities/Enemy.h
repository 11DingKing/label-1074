#ifndef ENEMY_H
#define ENEMY_H

#include "GameObject.h"
#include "GameConfig.h"

/**
 * 敌机类型枚举
 */
enum class EnemyType {
    Small,      // 小型敌机
    Medium,     // 中型敌机
    Large,      // 大型敌机
    Boss        // Boss
};

/**
 * 移动模式枚举
 */
enum class MovementPattern {
    Straight,   // 直线下落
    Zigzag,     // 左右摇摆
    Sine,       // 正弦波动
    Chase       // 追踪玩家
};

/**
 * 敌机类
 * 处理不同类型敌机的行为和渲染
 */
class Enemy : public GameObject {
public:
    Enemy(const QPointF& pos, EnemyType type);
    ~Enemy() override = default;
    
    // GameObject接口实现
    void update(float deltaTime) override;
    void render(QPainter& painter) override;
    
    // 属性访问
    EnemyType type() const { return m_type; }
    int scoreValue() const { return m_scoreValue; }
    
    // 射击系统
    bool canFire() const;
    void resetFireTimer();
    
    // 速度调整
    void setSpeedMultiplier(float multiplier);

private:
    void initByType();
    void updateMovement(float deltaTime);
    
    void renderSmall(QPainter& painter);
    void renderMedium(QPainter& painter);
    void renderLarge(QPainter& painter);
    void renderBoss(QPainter& painter);
    void renderHealthBar(QPainter& painter);

    EnemyType m_type;
    MovementPattern m_pattern;
    int m_scoreValue;
    int m_maxHealth;
    
    float m_speed;
    float m_speedMultiplier;
    float m_time;           // 用于动画和移动模式
    float m_startX;         // 起始X位置（用于摇摆）
    
    // 射击
    float m_fireTimer;
    float m_fireRate;
    bool m_canFire;
    
    // Boss专用
    int m_bossPhase;
    float m_bossPatternTimer;
};

#endif // ENEMY_H
