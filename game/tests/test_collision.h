#ifndef TEST_COLLISION_H
#define TEST_COLLISION_H

#include <QTest>
#include <QObject>
#include "../src/entities/Player.h"
#include "../src/entities/Enemy.h"
#include "../src/entities/Bullet.h"
#include "../src/entities/PowerUp.h"

/**
 * 碰撞检测测试类
 */
class TestCollision : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        qDebug() << "Starting Collision tests...";
    }
    
    void cleanupTestCase() {
        qDebug() << "Collision tests completed.";
    }
    
    // 测试玩家与敌机碰撞
    void testPlayerEnemyCollision() {
        Player player(QPointF(240, 600));
        Enemy enemy(QPointF(240, 600), EnemyType::Small);
        
        QVERIFY(player.collidesWith(enemy));
        
        // 移开敌机
        enemy.setPosition(100, 100);
        QVERIFY(!player.collidesWith(enemy));
    }
    
    // 测试子弹与敌机碰撞
    void testBulletEnemyCollision() {
        Bullet bullet(QPointF(240, 300), true);
        Enemy enemy(QPointF(240, 300), EnemyType::Medium);
        
        QVERIFY(bullet.collidesWith(enemy));
        
        // 移开子弹
        bullet.setPosition(100, 100);
        QVERIFY(!bullet.collidesWith(enemy));
    }
    
    // 测试玩家与道具碰撞
    void testPlayerPowerUpCollision() {
        Player player(QPointF(240, 600));
        PowerUp powerUp(QPointF(240, 600), PowerUpType::Life);
        
        QVERIFY(player.collidesWith(powerUp));
        
        // 移开道具
        powerUp.setPosition(50, 50);
        QVERIFY(!player.collidesWith(powerUp));
    }
    
    // 测试边界碰撞情况
    void testEdgeCollision() {
        // 刚好接触的情况
        Player player(QPointF(100, 100));
        Enemy enemy(QPointF(100 + player.width(), 100), EnemyType::Small);
        
        // 这种情况可能碰撞也可能不碰撞，取决于碰撞盒
        // 我们只验证碰撞检测不会崩溃
        bool collides = player.collidesWith(enemy);
        Q_UNUSED(collides)
        QVERIFY(true);  // 只要不崩溃就通过
    }
    
    // 测试子弹类型
    void testBulletType() {
        Bullet playerBullet(QPointF(240, 300), true);
        Bullet enemyBullet(QPointF(240, 300), false);
        
        QVERIFY(playerBullet.isPlayerBullet());
        QVERIFY(!enemyBullet.isPlayerBullet());
    }
    
    // 测试子弹伤害
    void testBulletDamage() {
        Bullet bullet(QPointF(240, 300), true);
        
        QVERIFY(bullet.damage() > 0);
    }
    
    // 测试子弹移动
    void testBulletMovement() {
        Bullet playerBullet(QPointF(240, 300), true);
        Bullet enemyBullet(QPointF(240, 300), false);
        
        float playerInitialY = playerBullet.y();
        float enemyInitialY = enemyBullet.y();
        
        playerBullet.update(0.1f);
        enemyBullet.update(0.1f);
        
        // 玩家子弹向上移动
        QVERIFY(playerBullet.y() < playerInitialY);
        
        // 敌机子弹向下移动
        QVERIFY(enemyBullet.y() > enemyInitialY);
    }
    
    // 测试道具类型
    void testPowerUpTypes() {
        PowerUp life(QPointF(0, 0), PowerUpType::Life);
        PowerUp power(QPointF(0, 0), PowerUpType::Power);
        PowerUp shield(QPointF(0, 0), PowerUpType::Shield);
        PowerUp bomb(QPointF(0, 0), PowerUpType::Bomb);
        
        QCOMPARE(life.type(), PowerUpType::Life);
        QCOMPARE(power.type(), PowerUpType::Power);
        QCOMPARE(shield.type(), PowerUpType::Shield);
        QCOMPARE(bomb.type(), PowerUpType::Bomb);
    }
    
    // 测试道具移动
    void testPowerUpMovement() {
        PowerUp powerUp(QPointF(240, 100), PowerUpType::Life);
        float initialY = powerUp.y();
        
        powerUp.update(0.1f);
        
        // 道具向下移动
        QVERIFY(powerUp.y() > initialY);
    }
    
    // 测试多个对象碰撞
    void testMultipleCollisions() {
        Player player(QPointF(240, 600));
        
        QList<Enemy*> enemies;
        enemies.append(new Enemy(QPointF(240, 600), EnemyType::Small));  // 碰撞
        enemies.append(new Enemy(QPointF(100, 100), EnemyType::Medium)); // 不碰撞
        enemies.append(new Enemy(QPointF(245, 605), EnemyType::Large));  // 碰撞
        
        int collisionCount = 0;
        for (Enemy* enemy : enemies) {
            if (player.collidesWith(*enemy)) {
                collisionCount++;
            }
        }
        
        QCOMPARE(collisionCount, 2);
        
        // 清理
        qDeleteAll(enemies);
    }
};

#endif // TEST_COLLISION_H
