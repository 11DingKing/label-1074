#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QPointF>
#include <QSizeF>
#include <QRectF>
#include <QPainter>

/**
 * 游戏对象基类
 * 所有游戏实体的抽象基类，提供通用属性和接口
 */
class GameObject {
public:
    GameObject(const QPointF& pos, const QSizeF& size)
        : m_position(pos)
        , m_size(size)
        , m_health(1)
        , m_active(true)
        , m_velocity(0, 0)
    {}
    
    virtual ~GameObject() = default;
    
    // 纯虚函数 - 子类必须实现
    virtual void update(float deltaTime) = 0;
    virtual void render(QPainter& painter) = 0;
    
    // 位置相关
    QPointF position() const { return m_position; }
    void setPosition(const QPointF& pos) { m_position = pos; }
    void setPosition(float x, float y) { m_position = QPointF(x, y); }
    
    float x() const { return m_position.x(); }
    float y() const { return m_position.y(); }
    void setX(float x) { m_position.setX(x); }
    void setY(float y) { m_position.setY(y); }
    
    // 尺寸相关
    QSizeF size() const { return m_size; }
    void setSize(const QSizeF& size) { m_size = size; }
    float width() const { return m_size.width(); }
    float height() const { return m_size.height(); }
    
    // 速度相关
    QPointF velocity() const { return m_velocity; }
    void setVelocity(const QPointF& vel) { m_velocity = vel; }
    void setVelocity(float vx, float vy) { m_velocity = QPointF(vx, vy); }
    
    // 碰撞检测
    QRectF getBoundingRect() const {
        return QRectF(m_position.x() - m_size.width() / 2,
                      m_position.y() - m_size.height() / 2,
                      m_size.width(),
                      m_size.height());
    }
    
    bool collidesWith(const GameObject& other) const {
        return getBoundingRect().intersects(other.getBoundingRect());
    }
    
    // 中心点
    QPointF center() const {
        return m_position;
    }
    
    // 生命值相关
    int health() const { return m_health; }
    void setHealth(int health) { m_health = health; }
    void damage(int amount) { 
        m_health -= amount;
        if (m_health <= 0) {
            m_health = 0;
            m_active = false;
        }
    }
    bool isAlive() const { return m_health > 0; }
    
    // 活动状态
    bool isActive() const { return m_active; }
    void setActive(bool active) { m_active = active; }
    void deactivate() { m_active = false; }
    
    // 边界检测
    bool isOutOfBounds(int screenWidth, int screenHeight) const {
        return x() < -width() || x() > screenWidth + width() ||
               y() < -height() || y() > screenHeight + height();
    }

protected:
    QPointF m_position;     // 中心位置
    QSizeF m_size;          // 尺寸
    int m_health;           // 生命值
    bool m_active;          // 是否活动
    QPointF m_velocity;     // 速度向量
};

#endif // GAMEOBJECT_H
