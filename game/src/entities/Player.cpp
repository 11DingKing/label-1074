#include "Player.h"
#include <QPainter>
#include <QPainterPath>
#include <cmath>

Player::Player(const QPointF& pos)
    : GameObject(pos, QSizeF(GameConfig::Player::WIDTH, GameConfig::Player::HEIGHT))
    , m_moveX(0)
    , m_moveY(0)
    , m_lives(GameConfig::Player::INITIAL_LIVES)
    , m_fireTimer(0)
    , m_fireRate(GameConfig::Player::FIRE_RATE)
    , m_invincible(false)
    , m_invincibleTimer(0)
    , m_blinkTimer(0)
    , m_visible(true)
    , m_shieldActive(false)
    , m_shieldTimer(0)
    , m_powerLevel(1)
    , m_powerTimer(0)
    , m_bombs(GameConfig::Player::INITIAL_BOMBS)
    , m_flameOffset(0)
{
    m_health = 1;
}

void Player::update(float deltaTime)
{
    // 更新位置
    float speed = GameConfig::Player::SPEED;
    m_position.setX(m_position.x() + m_moveX * speed);
    m_position.setY(m_position.y() + m_moveY * speed);
    
    // 更新计时器
    updateTimers(deltaTime);
    
    // 更新火焰动画
    m_flameOffset += deltaTime * 20;
    if (m_flameOffset > 2 * M_PI) {
        m_flameOffset -= 2 * M_PI;
    }
}

void Player::updateTimers(float deltaTime)
{
    // 射击计时器
    m_fireTimer += deltaTime;
    
    // 无敌计时器
    if (m_invincible) {
        m_invincibleTimer -= deltaTime;
        m_blinkTimer += deltaTime;
        
        if (m_blinkTimer >= GameConfig::Player::BLINK_INTERVAL) {
            m_blinkTimer = 0;
            m_visible = !m_visible;
        }
        
        if (m_invincibleTimer <= 0) {
            m_invincible = false;
            m_visible = true;
        }
    }
    
    // 护盾计时器
    if (m_shieldActive) {
        m_shieldTimer -= deltaTime;
        if (m_shieldTimer <= 0) {
            m_shieldActive = false;
        }
    }
    
    // 火力增强计时器
    if (m_powerLevel > 1) {
        m_powerTimer -= deltaTime;
        if (m_powerTimer <= 0) {
            m_powerLevel = 1;
        }
    }
}

void Player::render(QPainter& painter)
{
    if (!m_visible && m_invincible) return;
    
    painter.save();
    painter.translate(m_position);
    
    // 绘制火焰尾迹
    renderFlame(painter);
    
    // 绘制飞船主体
    renderShip(painter);
    
    // 绘制护盾效果
    if (m_shieldActive) {
        renderShield(painter);
    }
    
    painter.restore();
}

void Player::renderShip(QPainter& painter)
{
    // 飞船主体 - 科幻风格
    QPainterPath shipPath;
    
    // 机身主体
    shipPath.moveTo(0, -m_size.height() / 2);  // 机头
    shipPath.lineTo(-m_size.width() / 4, -m_size.height() / 4);
    shipPath.lineTo(-m_size.width() / 2, m_size.height() / 3);  // 左翼尖
    shipPath.lineTo(-m_size.width() / 4, m_size.height() / 4);
    shipPath.lineTo(-m_size.width() / 6, m_size.height() / 2);  // 左尾
    shipPath.lineTo(0, m_size.height() / 3);
    shipPath.lineTo(m_size.width() / 6, m_size.height() / 2);   // 右尾
    shipPath.lineTo(m_size.width() / 4, m_size.height() / 4);
    shipPath.lineTo(m_size.width() / 2, m_size.height() / 3);   // 右翼尖
    shipPath.lineTo(m_size.width() / 4, -m_size.height() / 4);
    shipPath.closeSubpath();
    
    // 填充渐变
    QLinearGradient bodyGradient(0, -m_size.height() / 2, 0, m_size.height() / 2);
    bodyGradient.setColorAt(0, QColor(150, 220, 255));
    bodyGradient.setColorAt(0.5, GameConfig::Colors::PLAYER);
    bodyGradient.setColorAt(1, QColor(50, 100, 150));
    
    painter.setPen(QPen(QColor(200, 240, 255), 2));
    painter.setBrush(bodyGradient);
    painter.drawPath(shipPath);
    
    // 驾驶舱
    painter.setPen(Qt::NoPen);
    QRadialGradient cockpitGradient(0, -m_size.height() / 6, m_size.width() / 4);
    cockpitGradient.setColorAt(0, QColor(200, 255, 255, 200));
    cockpitGradient.setColorAt(1, QColor(100, 200, 255, 100));
    painter.setBrush(cockpitGradient);
    painter.drawEllipse(QPointF(0, -m_size.height() / 6), 
                        m_size.width() / 5, m_size.height() / 5);
    
    // 引擎发光效果
    QRadialGradient engineGlow(-m_size.width() / 4, m_size.height() / 4, 10);
    engineGlow.setColorAt(0, QColor(100, 200, 255, 200));
    engineGlow.setColorAt(1, QColor(100, 200, 255, 0));
    painter.setBrush(engineGlow);
    painter.drawEllipse(QPointF(-m_size.width() / 4, m_size.height() / 4), 8, 8);
    painter.drawEllipse(QPointF(m_size.width() / 4, m_size.height() / 4), 8, 8);
}

void Player::renderFlame(QPainter& painter)
{
    // 动态火焰效果
    float flameLength = 15 + 5 * sin(m_flameOffset);
    
    // 左引擎火焰
    QPainterPath leftFlame;
    leftFlame.moveTo(-m_size.width() / 4 - 4, m_size.height() / 2);
    leftFlame.lineTo(-m_size.width() / 4, m_size.height() / 2 + flameLength);
    leftFlame.lineTo(-m_size.width() / 4 + 4, m_size.height() / 2);
    leftFlame.closeSubpath();
    
    // 右引擎火焰
    QPainterPath rightFlame;
    rightFlame.moveTo(m_size.width() / 4 - 4, m_size.height() / 2);
    rightFlame.lineTo(m_size.width() / 4, m_size.height() / 2 + flameLength);
    rightFlame.lineTo(m_size.width() / 4 + 4, m_size.height() / 2);
    rightFlame.closeSubpath();
    
    // 火焰渐变
    QLinearGradient flameGradient(0, m_size.height() / 2, 0, m_size.height() / 2 + flameLength);
    flameGradient.setColorAt(0, QColor(255, 255, 200));
    flameGradient.setColorAt(0.3, QColor(255, 200, 100));
    flameGradient.setColorAt(1, QColor(255, 100, 50, 0));
    
    painter.setPen(Qt::NoPen);
    painter.setBrush(flameGradient);
    painter.drawPath(leftFlame);
    painter.drawPath(rightFlame);
}

void Player::renderShield(QPainter& painter)
{
    // 护盾效果 - 半透明圆形
    float shieldRadius = std::max(m_size.width(), m_size.height()) * 0.7f;
    
    // 闪烁效果
    int alpha = 100 + 50 * sin(m_shieldTimer * 10);
    
    QRadialGradient shieldGradient(0, 0, shieldRadius);
    shieldGradient.setColorAt(0, QColor(100, 255, 200, 0));
    shieldGradient.setColorAt(0.7, QColor(100, 255, 200, alpha / 2));
    shieldGradient.setColorAt(1, QColor(100, 255, 200, alpha));
    
    painter.setPen(QPen(QColor(150, 255, 220, alpha), 2));
    painter.setBrush(shieldGradient);
    painter.drawEllipse(QPointF(0, 0), shieldRadius, shieldRadius);
}

void Player::setMoveDirection(float dx, float dy)
{
    m_moveX = dx;
    m_moveY = dy;
}

bool Player::canFire() const
{
    return m_fireTimer >= m_fireRate;
}

void Player::resetFireTimer()
{
    m_fireTimer = 0;
}

void Player::addLife()
{
    if (m_lives < GameConfig::Player::MAX_LIVES) {
        m_lives++;
    }
}

void Player::takeDamage()
{
    if (m_invincible || m_shieldActive) return;
    
    m_lives--;
    
    if (m_lives > 0) {
        // 进入无敌状态
        m_invincible = true;
        m_invincibleTimer = GameConfig::Player::INVINCIBLE_TIME;
        m_blinkTimer = 0;
        m_visible = true;
        
        // 重置火力
        m_powerLevel = 1;
    }
}

void Player::activateShield(float duration)
{
    m_shieldActive = true;
    m_shieldTimer = duration;
}

void Player::powerUp()
{
    if (m_powerLevel < 3) {
        m_powerLevel++;
    }
    m_powerTimer = GameConfig::PowerUp::POWER_DURATION;
}

void Player::resetPower()
{
    m_powerLevel = 1;
    m_powerTimer = 0;
}

void Player::addBomb()
{
    if (m_bombs < GameConfig::Player::MAX_BOMBS) {
        m_bombs++;
    }
}

void Player::useBomb()
{
    if (m_bombs > 0) {
        m_bombs--;
    }
}
