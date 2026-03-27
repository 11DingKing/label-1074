#include "Enemy.h"
#include <QPainter>
#include <QPainterPath>
#include <cmath>

Enemy::Enemy(const QPointF& pos, EnemyType type)
    : GameObject(pos, QSizeF(30, 30))
    , m_type(type)
    , m_pattern(MovementPattern::Straight)
    , m_scoreValue(100)
    , m_maxHealth(1)
    , m_speed(3.0f)
    , m_speedMultiplier(1.0f)
    , m_time(0)
    , m_startX(pos.x())
    , m_fireTimer(0)
    , m_fireRate(0)
    , m_canFire(false)
    , m_bossPhase(0)
    , m_bossPatternTimer(0)
{
    initByType();
}

void Enemy::initByType()
{
    switch (m_type) {
        case EnemyType::Small:
            m_size = QSizeF(GameConfig::Enemy::Small::WIDTH, GameConfig::Enemy::Small::HEIGHT);
            m_health = GameConfig::Enemy::Small::HEALTH;
            m_maxHealth = m_health;
            m_scoreValue = GameConfig::Enemy::Small::SCORE;
            m_speed = GameConfig::Enemy::Small::SPEED;
            m_pattern = MovementPattern::Straight;
            m_canFire = false;
            break;
            
        case EnemyType::Medium:
            m_size = QSizeF(GameConfig::Enemy::Medium::WIDTH, GameConfig::Enemy::Medium::HEIGHT);
            m_health = GameConfig::Enemy::Medium::HEALTH;
            m_maxHealth = m_health;
            m_scoreValue = GameConfig::Enemy::Medium::SCORE;
            m_speed = GameConfig::Enemy::Medium::SPEED;
            m_pattern = MovementPattern::Zigzag;
            m_canFire = false;
            break;
            
        case EnemyType::Large:
            m_size = QSizeF(GameConfig::Enemy::Large::WIDTH, GameConfig::Enemy::Large::HEIGHT);
            m_health = GameConfig::Enemy::Large::HEALTH;
            m_maxHealth = m_health;
            m_scoreValue = GameConfig::Enemy::Large::SCORE;
            m_speed = GameConfig::Enemy::Large::SPEED;
            m_pattern = MovementPattern::Sine;
            m_canFire = true;
            m_fireRate = GameConfig::Enemy::Large::FIRE_RATE;
            break;
            
        case EnemyType::Boss:
            m_size = QSizeF(GameConfig::Enemy::Boss::WIDTH, GameConfig::Enemy::Boss::HEIGHT);
            m_health = GameConfig::Enemy::Boss::HEALTH;
            m_maxHealth = m_health;
            m_scoreValue = GameConfig::Enemy::Boss::SCORE;
            m_speed = GameConfig::Enemy::Boss::SPEED;
            m_pattern = MovementPattern::Sine;
            m_canFire = true;
            m_fireRate = GameConfig::Enemy::Boss::FIRE_RATE;
            break;
    }
}

void Enemy::update(float deltaTime)
{
    m_time += deltaTime;
    m_fireTimer += deltaTime;
    
    updateMovement(deltaTime);
    
    // Boss阶段切换
    if (m_type == EnemyType::Boss) {
        m_bossPatternTimer += deltaTime;
        if (m_bossPatternTimer > 5.0f) {
            m_bossPatternTimer = 0;
            m_bossPhase = (m_bossPhase + 1) % 3;
        }
    }
}

void Enemy::updateMovement(float deltaTime)
{
    float actualSpeed = m_speed * m_speedMultiplier;
    
    switch (m_pattern) {
        case MovementPattern::Straight:
            m_position.setY(m_position.y() + actualSpeed);
            break;
            
        case MovementPattern::Zigzag:
            m_position.setY(m_position.y() + actualSpeed);
            m_position.setX(m_startX + GameConfig::Enemy::Medium::AMPLITUDE * 
                           sin(m_time * GameConfig::Enemy::Medium::FREQUENCY));
            break;
            
        case MovementPattern::Sine:
            if (m_type == EnemyType::Boss) {
                // Boss移动到屏幕上方后左右移动
                if (m_position.y() < 100) {
                    m_position.setY(m_position.y() + actualSpeed);
                } else {
                    m_position.setX(240 + 150 * sin(m_time * 0.8f));
                }
            } else {
                m_position.setY(m_position.y() + actualSpeed);
                m_position.setX(m_startX + 30 * sin(m_time * 2));
            }
            break;
            
        case MovementPattern::Chase:
            // 追踪模式暂不实现
            m_position.setY(m_position.y() + actualSpeed);
            break;
    }
}

void Enemy::render(QPainter& painter)
{
    painter.save();
    painter.translate(m_position);
    
    switch (m_type) {
        case EnemyType::Small:
            renderSmall(painter);
            break;
        case EnemyType::Medium:
            renderMedium(painter);
            break;
        case EnemyType::Large:
            renderLarge(painter);
            break;
        case EnemyType::Boss:
            renderBoss(painter);
            break;
    }
    
    // 显示血条（中型及以上）
    if (m_type != EnemyType::Small) {
        renderHealthBar(painter);
    }
    
    painter.restore();
}

void Enemy::renderSmall(QPainter& painter)
{
    // 小型敌机 - 三角形设计
    QPainterPath path;
    path.moveTo(0, m_size.height() / 2);
    path.lineTo(-m_size.width() / 2, -m_size.height() / 2);
    path.lineTo(m_size.width() / 2, -m_size.height() / 2);
    path.closeSubpath();
    
    QLinearGradient gradient(0, -m_size.height() / 2, 0, m_size.height() / 2);
    gradient.setColorAt(0, QColor(255, 150, 150));
    gradient.setColorAt(1, GameConfig::Colors::ENEMY_SMALL);
    
    painter.setPen(QPen(QColor(255, 200, 200), 1));
    painter.setBrush(gradient);
    painter.drawPath(path);
    
    // 机舱
    painter.setBrush(QColor(100, 50, 50));
    painter.drawEllipse(QPointF(0, 0), 6, 6);
}

void Enemy::renderMedium(QPainter& painter)
{
    // 中型敌机 - 菱形设计
    QPainterPath path;
    path.moveTo(0, m_size.height() / 2);
    path.lineTo(-m_size.width() / 2, 0);
    path.lineTo(-m_size.width() / 3, -m_size.height() / 2);
    path.lineTo(m_size.width() / 3, -m_size.height() / 2);
    path.lineTo(m_size.width() / 2, 0);
    path.closeSubpath();
    
    QLinearGradient gradient(0, -m_size.height() / 2, 0, m_size.height() / 2);
    gradient.setColorAt(0, QColor(255, 200, 100));
    gradient.setColorAt(1, GameConfig::Colors::ENEMY_MEDIUM);
    
    painter.setPen(QPen(QColor(255, 220, 150), 1));
    painter.setBrush(gradient);
    painter.drawPath(path);
    
    // 机翼装饰
    painter.setBrush(QColor(200, 100, 50));
    painter.drawRect(QRectF(-m_size.width() / 2 - 5, -5, 10, 10));
    painter.drawRect(QRectF(m_size.width() / 2 - 5, -5, 10, 10));
    
    // 机舱
    painter.setBrush(QColor(100, 80, 50));
    painter.drawEllipse(QPointF(0, -5), 8, 8);
}

void Enemy::renderLarge(QPainter& painter)
{
    // 大型敌机 - 重型战机设计
    QPainterPath path;
    
    // 主体
    path.moveTo(0, m_size.height() / 2);
    path.lineTo(-m_size.width() / 3, m_size.height() / 3);
    path.lineTo(-m_size.width() / 2, 0);
    path.lineTo(-m_size.width() / 2, -m_size.height() / 3);
    path.lineTo(-m_size.width() / 4, -m_size.height() / 2);
    path.lineTo(m_size.width() / 4, -m_size.height() / 2);
    path.lineTo(m_size.width() / 2, -m_size.height() / 3);
    path.lineTo(m_size.width() / 2, 0);
    path.lineTo(m_size.width() / 3, m_size.height() / 3);
    path.closeSubpath();
    
    QLinearGradient gradient(0, -m_size.height() / 2, 0, m_size.height() / 2);
    gradient.setColorAt(0, QColor(255, 150, 200));
    gradient.setColorAt(1, GameConfig::Colors::ENEMY_LARGE);
    
    painter.setPen(QPen(QColor(255, 180, 200), 2));
    painter.setBrush(gradient);
    painter.drawPath(path);
    
    // 引擎
    painter.setBrush(QColor(100, 50, 80));
    painter.drawEllipse(QPointF(-m_size.width() / 4, m_size.height() / 4), 8, 8);
    painter.drawEllipse(QPointF(m_size.width() / 4, m_size.height() / 4), 8, 8);
    
    // 机舱
    painter.setBrush(QColor(150, 100, 100));
    painter.drawEllipse(QPointF(0, -m_size.height() / 4), 12, 12);
}

void Enemy::renderBoss(QPainter& painter)
{
    // Boss - 大型战舰设计
    QPainterPath path;
    
    // 主体
    path.moveTo(0, m_size.height() / 2);
    path.lineTo(-m_size.width() / 4, m_size.height() / 3);
    path.lineTo(-m_size.width() / 2, m_size.height() / 4);
    path.lineTo(-m_size.width() / 2, -m_size.height() / 4);
    path.lineTo(-m_size.width() / 3, -m_size.height() / 2);
    path.lineTo(m_size.width() / 3, -m_size.height() / 2);
    path.lineTo(m_size.width() / 2, -m_size.height() / 4);
    path.lineTo(m_size.width() / 2, m_size.height() / 4);
    path.lineTo(m_size.width() / 4, m_size.height() / 3);
    path.closeSubpath();
    
    // 阶段颜色变化
    QColor baseColor = GameConfig::Colors::BOSS;
    if (m_health < m_maxHealth / 3) {
        baseColor = QColor(255, 100, 100);  // 低血量红色
    } else if (m_health < m_maxHealth * 2 / 3) {
        baseColor = QColor(255, 180, 100);  // 中等血量橙色
    }
    
    QLinearGradient gradient(0, -m_size.height() / 2, 0, m_size.height() / 2);
    gradient.setColorAt(0, baseColor.lighter(150));
    gradient.setColorAt(0.5, baseColor);
    gradient.setColorAt(1, baseColor.darker(150));
    
    painter.setPen(QPen(baseColor.lighter(170), 3));
    painter.setBrush(gradient);
    painter.drawPath(path);
    
    // 侧翼
    painter.setBrush(baseColor.darker(120));
    QRectF leftWing(-m_size.width() / 2 - 15, -m_size.height() / 6, 20, m_size.height() / 3);
    QRectF rightWing(m_size.width() / 2 - 5, -m_size.height() / 6, 20, m_size.height() / 3);
    painter.drawRoundedRect(leftWing, 5, 5);
    painter.drawRoundedRect(rightWing, 5, 5);
    
    // 炮台
    painter.setBrush(QColor(100, 80, 120));
    painter.drawEllipse(QPointF(-m_size.width() / 3, 0), 10, 10);
    painter.drawEllipse(QPointF(m_size.width() / 3, 0), 10, 10);
    painter.drawEllipse(QPointF(0, m_size.height() / 4), 12, 12);
    
    // 核心
    QRadialGradient coreGradient(0, -m_size.height() / 4, 20);
    coreGradient.setColorAt(0, QColor(255, 255, 255, 200));
    coreGradient.setColorAt(0.5, baseColor.lighter(120));
    coreGradient.setColorAt(1, baseColor);
    painter.setBrush(coreGradient);
    painter.drawEllipse(QPointF(0, -m_size.height() / 4), 18, 18);
    
    // 呼吸灯效果
    float pulse = 0.5f + 0.5f * sin(m_time * 3);
    painter.setBrush(QColor(255, 255, 255, int(pulse * 100)));
    painter.drawEllipse(QPointF(0, -m_size.height() / 4), 10, 10);
}

void Enemy::renderHealthBar(QPainter& painter)
{
    float barWidth = m_size.width() * 0.8f;
    float barHeight = 4;
    float x = -barWidth / 2;
    float y = -m_size.height() / 2 - 10;
    
    // 背景
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(50, 50, 50, 200));
    painter.drawRoundedRect(QRectF(x, y, barWidth, barHeight), 2, 2);
    
    // 血量
    float healthPercent = float(m_health) / float(m_maxHealth);
    QColor healthColor;
    if (healthPercent > 0.6f) {
        healthColor = QColor(100, 255, 100);
    } else if (healthPercent > 0.3f) {
        healthColor = QColor(255, 200, 100);
    } else {
        healthColor = QColor(255, 100, 100);
    }
    
    painter.setBrush(healthColor);
    painter.drawRoundedRect(QRectF(x, y, barWidth * healthPercent, barHeight), 2, 2);
}

bool Enemy::canFire() const
{
    return m_canFire && m_fireTimer >= m_fireRate;
}

void Enemy::resetFireTimer()
{
    m_fireTimer = 0;
}

void Enemy::setSpeedMultiplier(float multiplier)
{
    m_speedMultiplier = multiplier;
}
