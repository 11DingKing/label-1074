#include "Bullet.h"
#include <QPainter>
#include <QPainterPath>
#include <cmath>

Bullet::Bullet(const QPointF& pos, bool isPlayerBullet, float angleOffset)
    : GameObject(pos, QSizeF(6, 15))
    , m_isPlayerBullet(isPlayerBullet)
    , m_angleOffset(angleOffset)
    , m_trailTimer(0)
{
    if (isPlayerBullet) {
        m_size = QSizeF(GameConfig::Bullet::Player::WIDTH, GameConfig::Bullet::Player::HEIGHT);
        m_speed = GameConfig::Bullet::Player::SPEED;
        m_damage = GameConfig::Bullet::Player::DAMAGE;
    } else {
        m_size = QSizeF(GameConfig::Bullet::Enemy::WIDTH, GameConfig::Bullet::Enemy::HEIGHT);
        m_speed = GameConfig::Bullet::Enemy::SPEED;
        m_damage = GameConfig::Bullet::Enemy::DAMAGE;
    }
}

void Bullet::update(float deltaTime)
{
    m_trailTimer += deltaTime;
    
    float direction = m_isPlayerBullet ? -1.0f : 1.0f;
    
    // 计算速度分量
    float vx = sin(m_angleOffset) * m_speed;
    float vy = direction * cos(m_angleOffset) * m_speed;
    
    m_position.setX(m_position.x() + vx);
    m_position.setY(m_position.y() + vy);
}

void Bullet::render(QPainter& painter)
{
    painter.save();
    painter.translate(m_position);
    
    // 旋转以匹配飞行方向
    if (m_angleOffset != 0) {
        float direction = m_isPlayerBullet ? -1.0f : 1.0f;
        painter.rotate(m_angleOffset * 180 / M_PI * direction);
    }
    
    if (m_isPlayerBullet) {
        // 玩家子弹 - 能量弹
        QLinearGradient gradient(0, -m_size.height() / 2, 0, m_size.height() / 2);
        gradient.setColorAt(0, QColor(200, 255, 255));
        gradient.setColorAt(0.5, GameConfig::Colors::BULLET_PLAYER);
        gradient.setColorAt(1, QColor(50, 200, 150, 100));
        
        painter.setPen(Qt::NoPen);
        painter.setBrush(gradient);
        
        // 子弹主体
        QPainterPath bulletPath;
        bulletPath.moveTo(0, -m_size.height() / 2);
        bulletPath.quadTo(-m_size.width() / 2, 0, 0, m_size.height() / 2);
        bulletPath.quadTo(m_size.width() / 2, 0, 0, -m_size.height() / 2);
        painter.drawPath(bulletPath);
        
        // 发光效果
        QRadialGradient glow(0, 0, m_size.width());
        glow.setColorAt(0, QColor(100, 255, 200, 150));
        glow.setColorAt(1, QColor(100, 255, 200, 0));
        painter.setBrush(glow);
        painter.drawEllipse(QPointF(0, 0), m_size.width(), m_size.width());
        
    } else {
        // 敌机子弹 - 红色能量球
        QRadialGradient gradient(0, 0, m_size.width());
        gradient.setColorAt(0, QColor(255, 255, 200));
        gradient.setColorAt(0.3, GameConfig::Colors::BULLET_ENEMY);
        gradient.setColorAt(1, QColor(200, 50, 50, 100));
        
        painter.setPen(Qt::NoPen);
        painter.setBrush(gradient);
        painter.drawEllipse(QPointF(0, 0), m_size.width() / 2, m_size.width() / 2);
        
        // 外圈光晕
        QRadialGradient glow(0, 0, m_size.width());
        glow.setColorAt(0, QColor(255, 100, 100, 100));
        glow.setColorAt(1, QColor(255, 100, 100, 0));
        painter.setBrush(glow);
        painter.drawEllipse(QPointF(0, 0), m_size.width(), m_size.width());
    }
    
    painter.restore();
}
