#include "Explosion.h"
#include <QPainter>
#include <cmath>
#include <random>

Explosion::Explosion(const QPointF& pos, ExplosionSize size)
    : GameObject(pos, QSizeF(40, 40))
    , m_explosionSize(size)
    , m_currentFrame(0)
    , m_totalFrames(GameConfig::Explosion::FRAMES)
    , m_frameTimer(0)
    , m_frameTime(GameConfig::Explosion::FRAME_TIME)
{
    // 根据大小设置属性
    switch (size) {
        case ExplosionSize::Small:
            m_maxRadius = GameConfig::Explosion::SMALL_SIZE / 2.0f;
            m_size = QSizeF(GameConfig::Explosion::SMALL_SIZE, GameConfig::Explosion::SMALL_SIZE);
            break;
        case ExplosionSize::Medium:
            m_maxRadius = GameConfig::Explosion::MEDIUM_SIZE / 2.0f;
            m_size = QSizeF(GameConfig::Explosion::MEDIUM_SIZE, GameConfig::Explosion::MEDIUM_SIZE);
            break;
        case ExplosionSize::Large:
            m_maxRadius = GameConfig::Explosion::LARGE_SIZE / 2.0f;
            m_size = QSizeF(GameConfig::Explosion::LARGE_SIZE, GameConfig::Explosion::LARGE_SIZE);
            break;
    }
    
    // 生成粒子
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<float> angleDist(0, 2 * M_PI);
    std::uniform_real_distribution<float> speedDist(2.0f, 8.0f);
    std::uniform_real_distribution<float> sizeDist(2.0f, 6.0f);
    
    int particleCount = (size == ExplosionSize::Large) ? 20 : 
                        (size == ExplosionSize::Medium) ? 15 : 10;
    
    for (int i = 0; i < particleCount; ++i) {
        Particle p;
        float angle = angleDist(rng);
        float speed = speedDist(rng);
        p.x = 0;
        p.y = 0;
        p.vx = cos(angle) * speed;
        p.vy = sin(angle) * speed;
        p.size = sizeDist(rng);
        p.alpha = 255;
        m_particles.append(p);
    }
}

void Explosion::update(float deltaTime)
{
    m_frameTimer += deltaTime;
    
    if (m_frameTimer >= m_frameTime) {
        m_frameTimer = 0;
        m_currentFrame++;
        
        if (m_currentFrame >= m_totalFrames) {
            m_active = false;
        }
    }
    
    // 更新粒子
    for (auto& p : m_particles) {
        p.x += p.vx;
        p.y += p.vy;
        p.vx *= 0.95f;  // 减速
        p.vy *= 0.95f;
        p.alpha = std::max(0, p.alpha - 20);
    }
}

void Explosion::render(QPainter& painter)
{
    painter.save();
    painter.translate(m_position);
    
    float progress = float(m_currentFrame) / float(m_totalFrames);
    float radius = m_maxRadius * (0.3f + 0.7f * progress);
    int alpha = int(255 * (1.0f - progress));
    
    // 绘制主爆炸
    // 外圈
    QRadialGradient outerGlow(0, 0, radius);
    outerGlow.setColorAt(0, QColor(255, 255, 200, alpha));
    outerGlow.setColorAt(0.3, QColor(255, 200, 100, alpha * 0.8));
    outerGlow.setColorAt(0.6, QColor(255, 100, 50, alpha * 0.5));
    outerGlow.setColorAt(1, QColor(255, 50, 0, 0));
    
    painter.setPen(Qt::NoPen);
    painter.setBrush(outerGlow);
    painter.drawEllipse(QPointF(0, 0), radius, radius);
    
    // 内核
    float coreRadius = radius * 0.4f * (1.0f - progress);
    QRadialGradient coreGlow(0, 0, coreRadius);
    coreGlow.setColorAt(0, QColor(255, 255, 255, alpha));
    coreGlow.setColorAt(1, QColor(255, 255, 200, 0));
    painter.setBrush(coreGlow);
    painter.drawEllipse(QPointF(0, 0), coreRadius, coreRadius);
    
    // 绘制粒子
    for (const auto& p : m_particles) {
        if (p.alpha > 0) {
            QRadialGradient particleGlow(p.x, p.y, p.size);
            particleGlow.setColorAt(0, QColor(255, 200, 100, p.alpha));
            particleGlow.setColorAt(1, QColor(255, 100, 50, 0));
            painter.setBrush(particleGlow);
            painter.drawEllipse(QPointF(p.x, p.y), p.size, p.size);
        }
    }
    
    // 冲击波效果（仅大爆炸）
    if (m_explosionSize == ExplosionSize::Large && progress < 0.5f) {
        float waveRadius = m_maxRadius * 1.5f * progress * 2;
        int waveAlpha = int(150 * (1.0f - progress * 2));
        
        painter.setPen(QPen(QColor(255, 200, 150, waveAlpha), 3));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(QPointF(0, 0), waveRadius, waveRadius);
    }
    
    painter.restore();
}
