#include "PowerUp.h"
#include <QPainter>
#include <QPainterPath>
#include <cmath>

PowerUp::PowerUp(const QPointF& pos, PowerUpType type)
    : GameObject(pos, QSizeF(GameConfig::PowerUp::WIDTH, GameConfig::PowerUp::HEIGHT))
    , m_type(type)
    , m_speed(GameConfig::PowerUp::SPEED)
    , m_time(0)
    , m_rotationAngle(0)
{
}

void PowerUp::update(float deltaTime)
{
    m_time += deltaTime;
    m_rotationAngle += deltaTime * 180;  // 每秒旋转180度
    
    // 下落移动
    m_position.setY(m_position.y() + m_speed);
    
    // 左右摇摆
    m_position.setX(m_position.x() + sin(m_time * 3) * 0.5f);
}

void PowerUp::render(QPainter& painter)
{
    painter.save();
    painter.translate(m_position);
    
    // 呼吸效果
    float pulse = 1.0f + 0.1f * sin(m_time * 5);
    float size = m_size.width() / 2 * pulse;
    
    // 获取颜色
    QColor mainColor;
    QColor glowColor;
    QString symbol;
    
    switch (m_type) {
        case PowerUpType::Life:
            mainColor = GameConfig::Colors::POWERUP_LIFE;
            glowColor = QColor(255, 150, 180);
            symbol = "+";
            break;
        case PowerUpType::Power:
            mainColor = GameConfig::Colors::POWERUP_POWER;
            glowColor = QColor(150, 220, 255);
            symbol = "P";
            break;
        case PowerUpType::Shield:
            mainColor = GameConfig::Colors::POWERUP_SHIELD;
            glowColor = QColor(150, 255, 220);
            symbol = "S";
            break;
        case PowerUpType::Bomb:
            mainColor = GameConfig::Colors::POWERUP_BOMB;
            glowColor = QColor(255, 220, 150);
            symbol = "B";
            break;
    }
    
    // 外圈光晕
    QRadialGradient outerGlow(0, 0, size * 1.5f);
    outerGlow.setColorAt(0, QColor(glowColor.red(), glowColor.green(), glowColor.blue(), 100));
    outerGlow.setColorAt(1, QColor(glowColor.red(), glowColor.green(), glowColor.blue(), 0));
    painter.setPen(Qt::NoPen);
    painter.setBrush(outerGlow);
    painter.drawEllipse(QPointF(0, 0), size * 1.5f, size * 1.5f);
    
    // 主体背景
    QRadialGradient mainGradient(0, 0, size);
    mainGradient.setColorAt(0, mainColor.lighter(150));
    mainGradient.setColorAt(0.5, mainColor);
    mainGradient.setColorAt(1, mainColor.darker(150));
    
    painter.setBrush(mainGradient);
    painter.setPen(QPen(mainColor.lighter(180), 2));
    
    // 根据类型绘制不同形状
    switch (m_type) {
        case PowerUpType::Life:
            // 心形
            {
                QPainterPath heartPath;
                heartPath.moveTo(0, size * 0.3f);
                heartPath.cubicTo(-size, -size * 0.3f, -size * 0.5f, -size, 0, -size * 0.4f);
                heartPath.cubicTo(size * 0.5f, -size, size, -size * 0.3f, 0, size * 0.3f);
                painter.drawPath(heartPath);
            }
            break;
            
        case PowerUpType::Power:
            // 闪电形
            {
                painter.save();
                painter.rotate(m_rotationAngle);
                
                QPainterPath lightningPath;
                lightningPath.moveTo(0, -size);
                lightningPath.lineTo(-size * 0.5f, 0);
                lightningPath.lineTo(0, 0);
                lightningPath.lineTo(-size * 0.3f, size);
                lightningPath.lineTo(size * 0.3f, 0);
                lightningPath.lineTo(0, 0);
                lightningPath.lineTo(size * 0.5f, -size * 0.3f);
                lightningPath.closeSubpath();
                painter.drawPath(lightningPath);
                
                painter.restore();
            }
            break;
            
        case PowerUpType::Shield:
            // 盾形
            {
                QPainterPath shieldPath;
                shieldPath.moveTo(0, -size);
                shieldPath.lineTo(-size * 0.8f, -size * 0.5f);
                shieldPath.lineTo(-size * 0.8f, size * 0.3f);
                shieldPath.quadTo(0, size * 1.2f, size * 0.8f, size * 0.3f);
                shieldPath.lineTo(size * 0.8f, -size * 0.5f);
                shieldPath.closeSubpath();
                painter.drawPath(shieldPath);
                
                // 内部装饰
                painter.setPen(QPen(mainColor.lighter(200), 1));
                painter.setBrush(Qt::NoBrush);
                shieldPath = QPainterPath();
                shieldPath.moveTo(0, -size * 0.6f);
                shieldPath.lineTo(-size * 0.4f, -size * 0.3f);
                shieldPath.lineTo(-size * 0.4f, size * 0.1f);
                shieldPath.quadTo(0, size * 0.6f, size * 0.4f, size * 0.1f);
                shieldPath.lineTo(size * 0.4f, -size * 0.3f);
                shieldPath.closeSubpath();
                painter.drawPath(shieldPath);
            }
            break;
            
        case PowerUpType::Bomb:
            // 炸弹形
            {
                // 主体
                painter.drawEllipse(QPointF(0, size * 0.1f), size * 0.7f, size * 0.7f);
                
                // 引线
                painter.setPen(QPen(QColor(100, 100, 100), 3));
                painter.drawLine(QPointF(0, -size * 0.5f), QPointF(size * 0.3f, -size));
                
                // 火花
                float sparkle = sin(m_time * 10) * 0.5f + 0.5f;
                QRadialGradient sparkGlow(size * 0.3f, -size, 5);
                sparkGlow.setColorAt(0, QColor(255, 255, 200, int(255 * sparkle)));
                sparkGlow.setColorAt(1, QColor(255, 200, 100, 0));
                painter.setPen(Qt::NoPen);
                painter.setBrush(sparkGlow);
                painter.drawEllipse(QPointF(size * 0.3f, -size), 8, 8);
            }
            break;
    }
    
    painter.restore();
}
