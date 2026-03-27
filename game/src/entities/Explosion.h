#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "GameObject.h"
#include "GameConfig.h"

/**
 * 爆炸大小枚举
 */
enum class ExplosionSize {
    Small,
    Medium,
    Large
};

/**
 * 爆炸效果类
 * 处理爆炸动画的播放
 */
class Explosion : public GameObject {
public:
    Explosion(const QPointF& pos, ExplosionSize size);
    ~Explosion() override = default;
    
    // GameObject接口实现
    void update(float deltaTime) override;
    void render(QPainter& painter) override;
    
    // 状态查询
    bool isFinished() const { return m_currentFrame >= m_totalFrames; }

private:
    ExplosionSize m_explosionSize;
    int m_currentFrame;
    int m_totalFrames;
    float m_frameTimer;
    float m_frameTime;
    float m_maxRadius;
    
    // 粒子效果
    struct Particle {
        float x, y;
        float vx, vy;
        float size;
        int alpha;
    };
    QList<Particle> m_particles;
};

#endif // EXPLOSION_H
