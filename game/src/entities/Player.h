#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include "GameConfig.h"

/**
 * 玩家飞机类
 * 处理玩家控制、射击、状态管理
 */
class Player : public GameObject {
public:
    explicit Player(const QPointF& pos);
    ~Player() override = default;
    
    // GameObject接口实现
    void update(float deltaTime) override;
    void render(QPainter& painter) override;
    
    // 移动控制
    void setMoveDirection(float dx, float dy);
    
    // 射击系统
    bool canFire() const;
    void resetFireTimer();
    
    // 生命系统
    int lives() const { return m_lives; }
    void addLife();
    void takeDamage();
    
    // 无敌状态
    bool isInvincible() const { return m_invincible || m_shieldActive; }
    void activateShield(float duration);
    
    // 火力系统
    int powerLevel() const { return m_powerLevel; }
    void powerUp();
    void resetPower();
    
    // 炸弹系统
    int bombs() const { return m_bombs; }
    void addBomb();
    void useBomb();

private:
    void updateTimers(float deltaTime);
    void renderShip(QPainter& painter);
    void renderShield(QPainter& painter);
    void renderFlame(QPainter& painter);

    // 移动
    float m_moveX;
    float m_moveY;
    
    // 生命
    int m_lives;
    
    // 射击
    float m_fireTimer;
    float m_fireRate;
    
    // 无敌
    bool m_invincible;
    float m_invincibleTimer;
    float m_blinkTimer;
    bool m_visible;
    
    // 护盾
    bool m_shieldActive;
    float m_shieldTimer;
    
    // 火力等级
    int m_powerLevel;
    float m_powerTimer;
    
    // 炸弹
    int m_bombs;
    
    // 动画
    float m_flameOffset;
};

#endif // PLAYER_H
