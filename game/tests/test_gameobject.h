#ifndef TEST_GAMEOBJECT_H
#define TEST_GAMEOBJECT_H

#include <QTest>
#include <QObject>
#include "../src/core/GameObject.h"

/**
 * 测试用的具体 GameObject 实现
 */
class MockGameObject : public GameObject {
public:
    MockGameObject(const QPointF& pos, const QSizeF& size)
        : GameObject(pos, size), updateCalled(false), renderCalled(false) {}
    
    void update(float deltaTime) override {
        updateCalled = true;
        lastDeltaTime = deltaTime;
    }
    
    void render(QPainter& painter) override {
        Q_UNUSED(painter)
        renderCalled = true;
    }
    
    bool updateCalled;
    bool renderCalled;
    float lastDeltaTime = 0;
};

/**
 * GameObject 测试类
 */
class TestGameObject : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        qDebug() << "Starting GameObject tests...";
    }
    
    void cleanupTestCase() {
        qDebug() << "GameObject tests completed.";
    }
    
    // 测试构造函数
    void testConstruction() {
        MockGameObject obj(QPointF(100, 200), QSizeF(50, 60));
        
        QCOMPARE(obj.x(), 100.0f);
        QCOMPARE(obj.y(), 200.0f);
        QCOMPARE(obj.width(), 50.0f);
        QCOMPARE(obj.height(), 60.0f);
        QVERIFY(obj.isActive());
        QVERIFY(obj.isAlive());
    }
    
    // 测试位置设置
    void testPosition() {
        MockGameObject obj(QPointF(0, 0), QSizeF(10, 10));
        
        obj.setPosition(50, 100);
        QCOMPARE(obj.x(), 50.0f);
        QCOMPARE(obj.y(), 100.0f);
        
        obj.setPosition(QPointF(200, 300));
        QCOMPARE(obj.position(), QPointF(200, 300));
        
        obj.setX(150);
        obj.setY(250);
        QCOMPARE(obj.x(), 150.0f);
        QCOMPARE(obj.y(), 250.0f);
    }
    
    // 测试碰撞盒
    void testBoundingRect() {
        MockGameObject obj(QPointF(100, 100), QSizeF(40, 60));
        
        QRectF rect = obj.getBoundingRect();
        
        // 碰撞盒应该以位置为中心
        QCOMPARE(rect.left(), 80.0);   // 100 - 40/2
        QCOMPARE(rect.top(), 70.0);    // 100 - 60/2
        QCOMPARE(rect.width(), 40.0);
        QCOMPARE(rect.height(), 60.0);
    }
    
    // 测试碰撞检测
    void testCollision() {
        MockGameObject obj1(QPointF(100, 100), QSizeF(50, 50));
        MockGameObject obj2(QPointF(120, 120), QSizeF(50, 50));
        MockGameObject obj3(QPointF(300, 300), QSizeF(50, 50));
        
        // obj1 和 obj2 应该碰撞
        QVERIFY(obj1.collidesWith(obj2));
        QVERIFY(obj2.collidesWith(obj1));
        
        // obj1 和 obj3 不应该碰撞
        QVERIFY(!obj1.collidesWith(obj3));
    }
    
    // 测试生命值系统
    void testHealth() {
        MockGameObject obj(QPointF(0, 0), QSizeF(10, 10));
        obj.setHealth(10);
        
        QCOMPARE(obj.health(), 10);
        QVERIFY(obj.isAlive());
        
        obj.damage(3);
        QCOMPARE(obj.health(), 7);
        QVERIFY(obj.isAlive());
        
        obj.damage(10);
        QCOMPARE(obj.health(), 0);
        QVERIFY(!obj.isAlive());
        QVERIFY(!obj.isActive());
    }
    
    // 测试活动状态
    void testActiveState() {
        MockGameObject obj(QPointF(0, 0), QSizeF(10, 10));
        
        QVERIFY(obj.isActive());
        
        obj.setActive(false);
        QVERIFY(!obj.isActive());
        
        obj.setActive(true);
        QVERIFY(obj.isActive());
        
        obj.deactivate();
        QVERIFY(!obj.isActive());
    }
    
    // 测试边界检测
    void testOutOfBounds() {
        MockGameObject obj(QPointF(100, 100), QSizeF(20, 20));
        
        // 在屏幕内
        QVERIFY(!obj.isOutOfBounds(800, 600));
        
        // 超出左边界
        obj.setPosition(-50, 100);
        QVERIFY(obj.isOutOfBounds(800, 600));
        
        // 超出右边界
        obj.setPosition(850, 100);
        QVERIFY(obj.isOutOfBounds(800, 600));
        
        // 超出上边界
        obj.setPosition(100, -50);
        QVERIFY(obj.isOutOfBounds(800, 600));
        
        // 超出下边界
        obj.setPosition(100, 650);
        QVERIFY(obj.isOutOfBounds(800, 600));
    }
    
    // 测试速度
    void testVelocity() {
        MockGameObject obj(QPointF(0, 0), QSizeF(10, 10));
        
        obj.setVelocity(5.0f, -3.0f);
        QCOMPARE(obj.velocity(), QPointF(5.0f, -3.0f));
        
        obj.setVelocity(QPointF(10.0f, 20.0f));
        QCOMPARE(obj.velocity(), QPointF(10.0f, 20.0f));
    }
    
    // 测试 update 被调用
    void testUpdate() {
        MockGameObject obj(QPointF(0, 0), QSizeF(10, 10));
        
        QVERIFY(!obj.updateCalled);
        obj.update(0.016f);
        QVERIFY(obj.updateCalled);
        QCOMPARE(obj.lastDeltaTime, 0.016f);
    }
};

#endif // TEST_GAMEOBJECT_H
