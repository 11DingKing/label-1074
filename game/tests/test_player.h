#ifndef TEST_PLAYER_H
#define TEST_PLAYER_H

#include <QTest>
#include <QObject>
#include "../src/entities/Player.h"
#include "../src/config/GameConfig.h"

/**
 * Player 测试类
 */
class TestPlayer : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        qDebug() << "Starting Player tests...";
    }
    
    void cleanupTestCase() {
        qDebug() << "Player tests completed.";
    }
    
    // 测试玩家初始化
    void testInitialization() {
        Player player(QPointF(240, 600));
        
        QCOMPARE(player.x(), 240.0f);
        QCOMPARE(player.y(), 600.0f);
        QCOMPARE(player.lives(), GameConfig::Player::INITIAL_LIVES);
        QCOMPARE(player.bombs(), GameConfig::Player::INITIAL_BOMBS);
        QCOMPARE(player.powerLevel(), 1);
        QVERIFY(player.isActive());
        QVERIFY(!player.isInvincible());
    }
    
    // 测试移动方向设置
    void testMoveDirection() {
        Player player(QPointF(240, 600));
        
        // 向右移动
        player.setMoveDirection(1.0f, 0);
        player.update(0.1f);
        QVERIFY(player.x() > 240.0f);
        
        // 向左移动
        float currentX = player.x();
        player.setMoveDirection(-1.0f, 0);
        player.update(0.1f);
        QVERIFY(player.x() < currentX);
    }
    
    // 测试射击系统
    void testFireSystem() {
        Player player(QPointF(240, 600));
        
        // 初始状态可以射击
        QVERIFY(player.canFire());
        
        // 射击后不能立即再次射击
        player.resetFireTimer();
        QVERIFY(!player.canFire());
        
        // 经过足够时间后可以再次射击
        for (int i = 0; i < 20; ++i) {
            player.update(0.01f);
        }
        QVERIFY(player.canFire());
    }
    
    // 测试生命系统
    void testLivesSystem() {
        Player player(QPointF(240, 600));
        int initialLives = player.lives();
        
        // 增加生命
        player.addLife();
        QCOMPARE(player.lives(), initialLives + 1);
        
        // 不能超过最大生命
        for (int i = 0; i < 10; ++i) {
            player.addLife();
        }
        QCOMPARE(player.lives(), GameConfig::Player::MAX_LIVES);
        
        // 受到伤害
        player.takeDamage();
        QCOMPARE(player.lives(), GameConfig::Player::MAX_LIVES - 1);
        QVERIFY(player.isInvincible());  // 受伤后进入无敌状态
    }
    
    // 测试无敌状态
    void testInvincibility() {
        Player player(QPointF(240, 600));
        
        QVERIFY(!player.isInvincible());
        
        // 受伤后进入无敌状态
        player.takeDamage();
        QVERIFY(player.isInvincible());
        
        // 无敌期间再次受伤不会减少生命
        int livesAfterFirstHit = player.lives();
        player.takeDamage();
        QCOMPARE(player.lives(), livesAfterFirstHit);
    }
    
    // 测试护盾
    void testShield() {
        Player player(QPointF(240, 600));
        
        player.activateShield(5.0f);
        QVERIFY(player.isInvincible());
        
        // 护盾期间受伤不会减少生命
        int initialLives = player.lives();
        player.takeDamage();
        QCOMPARE(player.lives(), initialLives);
    }
    
    // 测试火力升级
    void testPowerUp() {
        Player player(QPointF(240, 600));
        
        QCOMPARE(player.powerLevel(), 1);
        
        player.powerUp();
        QCOMPARE(player.powerLevel(), 2);
        
        player.powerUp();
        QCOMPARE(player.powerLevel(), 3);
        
        // 不能超过最大火力等级
        player.powerUp();
        QCOMPARE(player.powerLevel(), 3);
        
        // 重置火力
        player.resetPower();
        QCOMPARE(player.powerLevel(), 1);
    }
    
    // 测试炸弹系统
    void testBombSystem() {
        Player player(QPointF(240, 600));
        int initialBombs = player.bombs();
        
        // 增加炸弹
        player.addBomb();
        QCOMPARE(player.bombs(), initialBombs + 1);
        
        // 使用炸弹
        player.useBomb();
        QCOMPARE(player.bombs(), initialBombs);
        
        // 炸弹数不能为负
        for (int i = 0; i < 10; ++i) {
            player.useBomb();
        }
        QVERIFY(player.bombs() >= 0);
        
        // 不能超过最大炸弹数
        for (int i = 0; i < 20; ++i) {
            player.addBomb();
        }
        QCOMPARE(player.bombs(), GameConfig::Player::MAX_BOMBS);
    }
    
    // 测试受伤后火力重置
    void testPowerResetOnDamage() {
        Player player(QPointF(240, 600));
        
        player.powerUp();
        player.powerUp();
        QCOMPARE(player.powerLevel(), 3);
        
        // 受伤后等待无敌结束
        player.takeDamage();
        
        // 经过无敌时间
        for (int i = 0; i < 200; ++i) {
            player.update(0.02f);
        }
        
        // 再次受伤
        player.takeDamage();
        
        // 火力应该被重置
        QCOMPARE(player.powerLevel(), 1);
    }
};

#endif // TEST_PLAYER_H
