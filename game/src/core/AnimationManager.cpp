#include "AnimationManager.h"
#include "GameConfig.h"
#include <QPainterPath>
#include <cmath>

AnimationManager& AnimationManager::instance()
{
    static AnimationManager instance;
    return instance;
}

AnimationManager::AnimationManager()
    : m_shakeActive(false)
    , m_shakeIntensity(0)
    , m_shakeDuration(0)
    , m_shakeTimer(0)
    , m_flashActive(false)
    , m_flashDuration(0)
    , m_flashTimer(0)
    , m_slowMotionActive(false)
    , m_slowMotionFactor(1.0f)
    , m_slowMotionDuration(0)
    , m_slowMotionTimer(0)
    , m_rng(std::random_device{}())
{
}

void AnimationManager::update(float deltaTime)
{
    updateShake(deltaTime);
    updateFlash(deltaTime);
    updateSlowMotion(deltaTime);
    updateParticles(deltaTime);
    updateTrails(deltaTime);
    updatePopups(deltaTime);
}

void AnimationManager::render(QPainter& painter, int width, int height)
{
    renderTrails(painter);
    renderParticles(painter);
    renderPopups(painter);
    renderFlash(painter, width, height);
}

void AnimationManager::triggerScreenShake(float intensity, float duration)
{
    m_shakeActive = true;
    m_shakeIntensity = intensity;
    m_shakeDuration = duration;
    m_shakeTimer = 0;
}

void AnimationManager::triggerFlash(const QColor& color, float duration)
{
    m_flashActive = true;
    m_flashColor = color;
    m_flashDuration = duration;
    m_flashTimer = 0;
}

void AnimationManager::triggerSlowMotion(float factor, float duration)
{
    m_slowMotionActive = true;
    m_slowMotionFactor = factor;
    m_slowMotionDuration = duration;
    m_slowMotionTimer = 0;
}

void AnimationManager::spawnParticles(const QPointF& pos, int count, const QColor& color, float speed)
{
    std::uniform_real_distribution<float> angleDist(0, 2 * M_PI);
    std::uniform_real_distribution<float> speedDist(speed * 0.5f, speed * 1.5f);
    std::uniform_real_distribution<float> sizeDist(2.0f, 8.0f);
    std::uniform_real_distribution<float> lifeDist(0.3f, 0.8f);
    std::uniform_int_distribution<int> typeDist(0, 2);
    std::uniform_real_distribution<float> rotDist(-360.0f, 360.0f);
    
    for (int i = 0; i < count; ++i) {
        Particle p;
        float angle = angleDist(m_rng);
        float spd = speedDist(m_rng);
        
        p.pos = pos;
        p.vel = QPointF(cos(angle) * spd, sin(angle) * spd);
        p.color = color;
        p.size = sizeDist(m_rng);
        p.maxLife = lifeDist(m_rng);
        p.life = p.maxLife;
        p.type = typeDist(m_rng);
        p.rotation = angleDist(m_rng) * 180 / M_PI;
        p.rotationSpeed = rotDist(m_rng);
        
        m_particles.append(p);
    }
}

void AnimationManager::spawnTrail(const QPointF& pos, const QColor& color, float size)
{
    Trail t;
    t.pos = pos;
    t.color = color;
    t.size = size;
    t.life = 0.2f;
    m_trails.append(t);
}

void AnimationManager::spawnScorePopup(const QPointF& pos, int score)
{
    QString text = QString("+%1").arg(score);
    QColor color = GameConfig::Colors::HIGHLIGHT;
    spawnTextPopup(pos, text, color);
}

void AnimationManager::spawnTextPopup(const QPointF& pos, const QString& text, const QColor& color)
{
    TextPopup popup;
    popup.pos = pos;
    popup.text = text;
    popup.color = color;
    popup.maxLife = 1.0f;
    popup.life = popup.maxLife;
    popup.scale = 0.5f;
    m_popups.append(popup);
}

void AnimationManager::reset()
{
    m_shakeActive = false;
    m_flashActive = false;
    m_slowMotionActive = false;
    m_particles.clear();
    m_trails.clear();
    m_popups.clear();
    m_shakeOffset = QPointF(0, 0);
}

void AnimationManager::updateShake(float deltaTime)
{
    if (!m_shakeActive) return;
    
    m_shakeTimer += deltaTime;
    
    if (m_shakeTimer >= m_shakeDuration) {
        m_shakeActive = false;
        m_shakeOffset = QPointF(0, 0);
        return;
    }
    
    // 震动衰减
    float progress = m_shakeTimer / m_shakeDuration;
    float currentIntensity = m_shakeIntensity * (1.0f - progress);
    
    std::uniform_real_distribution<float> dist(-currentIntensity, currentIntensity);
    m_shakeOffset = QPointF(dist(m_rng), dist(m_rng));
}

void AnimationManager::updateFlash(float deltaTime)
{
    if (!m_flashActive) return;
    
    m_flashTimer += deltaTime;
    
    if (m_flashTimer >= m_flashDuration) {
        m_flashActive = false;
    }
}

void AnimationManager::updateSlowMotion(float deltaTime)
{
    if (!m_slowMotionActive) return;
    
    m_slowMotionTimer += deltaTime;
    
    if (m_slowMotionTimer >= m_slowMotionDuration) {
        m_slowMotionActive = false;
        m_slowMotionFactor = 1.0f;
    }
}

void AnimationManager::updateParticles(float deltaTime)
{
    for (int i = m_particles.size() - 1; i >= 0; --i) {
        Particle& p = m_particles[i];
        
        p.life -= deltaTime;
        if (p.life <= 0) {
            m_particles.removeAt(i);
            continue;
        }
        
        // 物理更新
        p.pos += p.vel;
        p.vel *= 0.95f;  // 阻力
        p.vel.setY(p.vel.y() + 0.2f);  // 重力
        p.rotation += p.rotationSpeed * deltaTime;
    }
}

void AnimationManager::updateTrails(float deltaTime)
{
    for (int i = m_trails.size() - 1; i >= 0; --i) {
        Trail& t = m_trails[i];
        t.life -= deltaTime;
        
        if (t.life <= 0) {
            m_trails.removeAt(i);
        }
    }
}

void AnimationManager::updatePopups(float deltaTime)
{
    for (int i = m_popups.size() - 1; i >= 0; --i) {
        TextPopup& p = m_popups[i];
        p.life -= deltaTime;
        
        if (p.life <= 0) {
            m_popups.removeAt(i);
            continue;
        }
        
        // 向上飘动
        p.pos.setY(p.pos.y() - 30 * deltaTime);
        
        // 缩放动画
        float progress = 1.0f - (p.life / p.maxLife);
        if (progress < 0.2f) {
            p.scale = 0.5f + progress * 2.5f;  // 弹出放大
        } else {
            p.scale = 1.0f;
        }
    }
}

void AnimationManager::renderFlash(QPainter& painter, int width, int height)
{
    if (!m_flashActive) return;
    
    float progress = m_flashTimer / m_flashDuration;
    int alpha = int(255 * (1.0f - progress) * 0.5f);
    
    QColor flashColor = m_flashColor;
    flashColor.setAlpha(alpha);
    
    painter.fillRect(0, 0, width, height, flashColor);
}

void AnimationManager::renderParticles(QPainter& painter)
{
    painter.save();
    
    for (const auto& p : m_particles) {
        float lifeRatio = p.life / p.maxLife;
        QColor color = p.color;
        color.setAlpha(int(255 * lifeRatio));
        
        painter.save();
        painter.translate(p.pos);
        painter.rotate(p.rotation);
        
        float size = p.size * lifeRatio;
        
        switch (p.type) {
            case 0:  // 圆形
            {
                QRadialGradient gradient(0, 0, size);
                gradient.setColorAt(0, color);
                gradient.setColorAt(1, QColor(color.red(), color.green(), color.blue(), 0));
                painter.setPen(Qt::NoPen);
                painter.setBrush(gradient);
                painter.drawEllipse(QPointF(0, 0), size, size);
                break;
            }
            case 1:  // 方形
            {
                painter.setPen(Qt::NoPen);
                painter.setBrush(color);
                painter.drawRect(QRectF(-size/2, -size/2, size, size));
                break;
            }
            case 2:  // 星形
            {
                QPainterPath starPath;
                for (int i = 0; i < 5; ++i) {
                    float angle = i * 72 * M_PI / 180 - M_PI / 2;
                    float x = cos(angle) * size;
                    float y = sin(angle) * size;
                    if (i == 0) starPath.moveTo(x, y);
                    else starPath.lineTo(x, y);
                    
                    angle += 36 * M_PI / 180;
                    x = cos(angle) * size * 0.4f;
                    y = sin(angle) * size * 0.4f;
                    starPath.lineTo(x, y);
                }
                starPath.closeSubpath();
                painter.setPen(Qt::NoPen);
                painter.setBrush(color);
                painter.drawPath(starPath);
                break;
            }
        }
        
        painter.restore();
    }
    
    painter.restore();
}

void AnimationManager::renderTrails(QPainter& painter)
{
    painter.setPen(Qt::NoPen);
    
    for (const auto& t : m_trails) {
        float lifeRatio = t.life / 0.2f;
        QColor color = t.color;
        color.setAlpha(int(100 * lifeRatio));
        
        float size = t.size * lifeRatio;
        
        QRadialGradient gradient(t.pos, size);
        gradient.setColorAt(0, color);
        gradient.setColorAt(1, QColor(color.red(), color.green(), color.blue(), 0));
        
        painter.setBrush(gradient);
        painter.drawEllipse(t.pos, size, size);
    }
}

void AnimationManager::renderPopups(QPainter& painter)
{
    QFont font;
    font.setPixelSize(18);
    font.setBold(true);
    painter.setFont(font);
    
    for (const auto& p : m_popups) {
        float lifeRatio = p.life / p.maxLife;
        QColor color = p.color;
        color.setAlpha(int(255 * lifeRatio));
        
        painter.save();
        painter.translate(p.pos);
        painter.scale(p.scale, p.scale);
        
        // 文字阴影
        painter.setPen(QColor(0, 0, 0, int(150 * lifeRatio)));
        painter.drawText(QRectF(-50, -12, 100, 24), Qt::AlignCenter, p.text);
        
        // 文字主体
        painter.setPen(color);
        painter.drawText(QRectF(-51, -13, 100, 24), Qt::AlignCenter, p.text);
        
        painter.restore();
    }
}
