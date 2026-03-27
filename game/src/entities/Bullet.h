#ifndef BULLET_H
#define BULLET_H

#include "GameObject.h"
#include "GameConfig.h"

/**
 * 子弹类
 * 处理玩家和敌机的子弹行为
 */
class Bullet : public GameObject {
public:
    /**
     * 构造函数
     * @param pos 初始位置
     * @param isPlayerBullet 是否为玩家子弹
     * @param angleOffset 发射角度偏移（用于散射）
     */
    Bullet(const QPointF& pos, bool isPlayerBullet, float angleOffset = 0.0f);
    ~Bullet() override = default;
    
    // GameObject接口实现
    void update(float deltaTime) override;
    void render(QPainter& painter) override;
    
    // 属性访问
    bool isPlayerBullet() const { return m_isPlayerBullet; }
    int damage() const { return m_damage; }

private:
    bool m_isPlayerBullet;
    float m_speed;
    int m_damage;
    float m_angleOffset;
    float m_trailTimer;
};

#endif // BULLET_H
