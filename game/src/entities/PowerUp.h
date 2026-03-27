#ifndef POWERUP_H
#define POWERUP_H

#include "GameObject.h"
#include "GameConfig.h"

/**
 * 道具类型枚举
 */
enum class PowerUpType {
    Life,       // 生命+1
    Power,      // 火力增强
    Shield,     // 护盾
    Bomb        // 炸弹
};

/**
 * 道具类
 * 处理道具的移动和渲染
 */
class PowerUp : public GameObject {
public:
    PowerUp(const QPointF& pos, PowerUpType type);
    ~PowerUp() override = default;
    
    // GameObject接口实现
    void update(float deltaTime) override;
    void render(QPainter& painter) override;
    
    // 属性访问
    PowerUpType type() const { return m_type; }

private:
    PowerUpType m_type;
    float m_speed;
    float m_time;           // 动画计时器
    float m_rotationAngle;  // 旋转角度
};

#endif // POWERUP_H
