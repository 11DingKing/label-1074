#ifndef TEST_ENEMY_H
#define TEST_ENEMY_H

#include <QTest>
#include <QObject>
#include "../src/entities/Enemy.h"
#include "../src/config/GameConfig.h"

/**
 * Enemy 测试类
 */
class TestEnemy : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        qDebug() << "Starting Enemy tests...";
    }
    
    void cleanupTestCase() {
        qDebug() << "Enemy tests completed.";
    }
    
    // 测试小型敌机
    void testSmallEnemy() {
        Enemy enemy(QPointF(240, 0), EnemyType::Small);
        
        QCOMPARE(enemy.type(), EnemyType::Small);
        QCOMPARE(enemy.health(), GameConfig::Enemy::Small::HEALTH);
        QCOMPARE(enemy.scoreValue(), GameConfig::Enemy::Small::SCORE);
        QCOMPARE(enemy.width(), (float)GameConfig::Enemy::Small::WIDTH);
        QCOMPARE(enemy.height(), (float)GameConfig::Enemy::Small::HEIGHT);
        QVERIFY(!enemy.canFire());  // 小型敌机不能射击
    }
    
    // 测试中型敌机
    void testMediumEnemy() {
        Enemy enemy(QPointF(240, 0), EnemyType::Medium);
        
        QCOMPARE(enemy.type(), EnemyType::Medium);
        QCOMPARE(enemy.health(), GameConfig::Enemy::Medium::HEALTH);
        QCOMPARE(enemy.scoreValue(), GameConfig::Enemy::Medium::SCORE);
        QVERIFY(!enemy.canFire());  // 中型敌机不能射击
    }
    
    // 测试大型敌机
    void testLargeEnemy() {
        Enemy enemy(QPointF(240, 0), EnemyType::Large);
        
        QCOMPARE(enemy.type(), EnemyType::Large);
        QCOMPARE(enemy.health(), GameConfig::Enemy::Large::HEALTH);
        QCOMPARE(enemy.scoreValue(), GameConfig::Enemy::Large::SCORE);
        // 大型敌机可以射击
    }
    
    // 测试Boss
    void testBossEnemy() {
        Enemy enemy(QPointF(240, 0), EnemyType::Boss);
        
        QCOMPARE(enemy.type(), EnemyType::Boss);
        QCOMPARE(enemy.health(), GameConfig::Enemy::Boss::HEALTH);
        QCOMPARE(enemy.scoreValue(), GameConfig::Enemy::Boss::SCORE);
        QCOMPARE(enemy.width(), (float)GameConfig::Enemy::Boss::WIDTH);
        QCOMPARE(enemy.height(), (float)GameConfig::Enemy::Boss::HEIGHT);
    }
    
    // 测试敌机移动
    void testEnemyMovement() {
        Enemy enemy(QPointF(240, 0), EnemyType::Small);
        float initialY = enemy.y();
        
        // 敌机应该向下移动
        enemy.update(0.1f);
        QVERIFY(enemy.y() > initialY);
    }
    
    // 测试敌机受伤
    void testEnemyDamage() {
        Enemy enemy(QPointF(240, 0), EnemyType::Medium);
        int initialHealth = enemy.health();
        
        enemy.damage(1);
        QCOMPARE(enemy.health(), initialHealth - 1);
        QVERIFY(enemy.isAlive());
        
        // 继续受伤直到死亡
        enemy.damage(initialHealth);
        QVERIFY(!enemy.isAlive());
        QVERIFY(!enemy.isActive());
    }
    
    // 测试速度倍率
    void testSpeedMultiplier() {
        Enemy enemy1(QPointF(240, 0), EnemyType::Small);
        Enemy enemy2(QPointF(240, 0), EnemyType::Small);
        
        enemy2.setSpeedMultiplier(2.0f);
        
        enemy1.update(0.1f);
        enemy2.update(0.1f);
        
        // 速度加倍的敌机应该移动更远
        QVERIFY(enemy2.y() > enemy1.y());
    }
    
    // 测试射击计时器
    void testFireTimer() {
        Enemy enemy(QPointF(240, 100), EnemyType::Large);
        
        // 初始状态不能立即射击
        QVERIFY(!enemy.canFire());
        
        // 经过足够时间后可以射击
        for (int i = 0; i < 100; ++i) {
            enemy.update(0.02f);
        }
        QVERIFY(enemy.canFire());
        
        // 射击后重置计时器
        enemy.resetFireTimer();
        QVERIFY(!enemy.canFire());
    }
    
    // 测试不同类型敌机的分数
    void testScoreValues() {
        Enemy small(QPointF(0, 0), EnemyType::Small);
        Enemy medium(QPointF(0, 0), EnemyType::Medium);
        Enemy large(QPointF(0, 0), EnemyType::Large);
        Enemy boss(QPointF(0, 0), EnemyType::Boss);
        
        // 分数应该递增
        QVERIFY(small.scoreValue() < medium.scoreValue());
        QVERIFY(medium.scoreValue() < large.scoreValue());
        QVERIFY(large.scoreValue() < boss.scoreValue());
    }
};

#endif // TEST_ENEMY_H
