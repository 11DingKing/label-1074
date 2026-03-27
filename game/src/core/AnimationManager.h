#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include <QObject>
#include <QList>
#include <QPointF>
#include <QColor>
#include <QPainter>
#include <functional>
#include <random>

/**
 * 动画管理器
 * 管理全局动画效果：屏幕震动、闪光、粒子等
 */
class AnimationManager : public QObject {
    Q_OBJECT

public:
    static AnimationManager& instance();
    
    // 禁止拷贝
    AnimationManager(const AnimationManager&) = delete;
    AnimationManager& operator=(const AnimationManager&) = delete;
    
    // 更新所有动画
    void update(float deltaTime);
    
    // 渲染所有效果
    void render(QPainter& painter, int width, int height);
    
    // 触发效果
    void triggerScreenShake(float intensity, float duration);
    void triggerFlash(const QColor& color, float duration);
    void triggerSlowMotion(float factor, float duration);
    void spawnParticles(const QPointF& pos, int count, const QColor& color, float speed = 5.0f);
    void spawnTrail(const QPointF& pos, const QColor& color, float size);
    void spawnScorePopup(const QPointF& pos, int score);
    void spawnTextPopup(const QPointF& pos, const QString& text, const QColor& color);
    
    // 获取当前震动偏移
    QPointF getShakeOffset() const { return m_shakeOffset; }
    
    // 获取慢动作因子
    float getSlowMotionFactor() const { return m_slowMotionActive ? m_slowMotionFactor : 1.0f; }
    
    // 重置所有效果
    void reset();

private:
    AnimationManager();
    ~AnimationManager() = default;
    
    // 粒子结构
    struct Particle {
        QPointF pos;
        QPointF vel;
        QColor color;
        float size;
        float life;
        float maxLife;
        float rotation;
        float rotationSpeed;
        int type;  // 0: 圆形, 1: 方形, 2: 星形
    };
    
    // 拖尾结构
    struct Trail {
        QPointF pos;
        QColor color;
        float size;
        float life;
    };
    
    // 文字弹出结构
    struct TextPopup {
        QPointF pos;
        QString text;
        QColor color;
        float life;
        float maxLife;
        float scale;
    };
    
    // 更新各类效果
    void updateShake(float deltaTime);
    void updateFlash(float deltaTime);
    void updateSlowMotion(float deltaTime);
    void updateParticles(float deltaTime);
    void updateTrails(float deltaTime);
    void updatePopups(float deltaTime);
    
    // 渲染各类效果
    void renderFlash(QPainter& painter, int width, int height);
    void renderParticles(QPainter& painter);
    void renderTrails(QPainter& painter);
    void renderPopups(QPainter& painter);
    
    // 震动效果
    bool m_shakeActive;
    float m_shakeIntensity;
    float m_shakeDuration;
    float m_shakeTimer;
    QPointF m_shakeOffset;
    
    // 闪光效果
    bool m_flashActive;
    QColor m_flashColor;
    float m_flashDuration;
    float m_flashTimer;
    
    // 慢动作效果
    bool m_slowMotionActive;
    float m_slowMotionFactor;
    float m_slowMotionDuration;
    float m_slowMotionTimer;
    
    // 粒子系统
    QList<Particle> m_particles;
    
    // 拖尾系统
    QList<Trail> m_trails;
    
    // 文字弹出
    QList<TextPopup> m_popups;
    
    // 随机数生成器
    std::mt19937 m_rng;
};

#endif // ANIMATIONMANAGER_H
