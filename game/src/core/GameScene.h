#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QKeyEvent>
#include <QPainter>
#include <QList>
#include <QSet>
#include <memory>
#include <random>
#include <vector>

class Player;
class Enemy;
class Bullet;
class Explosion;
class PowerUp;
class GameHUD;

/**
 * 游戏场景
 * 核心游戏逻辑处理，包括游戏循环、对象管理、碰撞检测等
 */
class GameScene : public QWidget {
    Q_OBJECT

public:
    enum class GameState {
        Ready,
        Playing,
        Paused,
        GameOver
    };

    explicit GameScene(QWidget *parent = nullptr);
    ~GameScene() override;

    // 游戏控制
    void startGame();
    void pauseGame();
    void resumeGame();
    void stopGame();
    void resetGame();
    
    // 输入处理
    void handleKeyPress(QKeyEvent *event);
    void handleKeyRelease(QKeyEvent *event);
    
    // 状态获取
    GameState state() const { return m_state; }
    int score() const { return m_score; }
    int level() const { return m_level; }

signals:
    void gamePaused();
    void gameOver(int finalScore, int level, int enemiesKilled);
    void scoreChanged(int score);
    void levelChanged(int level);
    void levelCompleted(int level);  // 关卡完成信号

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void gameLoop();

private:
    // 初始化
    void initStars();
    
    // 更新逻辑
    void updateGame(float deltaTime);
    void updatePlayer(float deltaTime);
    void updateEnemies(float deltaTime);
    void updateBullets(float deltaTime);
    void updateExplosions(float deltaTime);
    void updatePowerUps(float deltaTime);
    void updateStars(float deltaTime);
    
    // 生成逻辑
    void spawnEnemy();
    void spawnBoss();
    void spawnPowerUp(const QPointF& pos);
    void playerFire();
    void enemyFire(Enemy* enemy);
    
    // 碰撞检测
    void checkCollisions();
    void checkPlayerBulletCollisions();
    void checkEnemyBulletCollisions();
    void checkPlayerEnemyCollisions();
    void checkPlayerPowerUpCollisions();
    
    // 渲染
    void renderBackground(QPainter& painter);
    void renderStars(QPainter& painter);
    void renderPlayer(QPainter& painter);
    void renderEnemies(QPainter& painter);
    void renderBullets(QPainter& painter);
    void renderExplosions(QPainter& painter);
    void renderPowerUps(QPainter& painter);
    void renderHUD(QPainter& painter);
    
    // 游戏事件
    void onEnemyDestroyed(Enemy* enemy);
    void onPlayerHit();
    void onPlayerDeath();
    void onLevelUp();
    void useBomb();
    
    // 工具方法
    void cleanupInactiveObjects();
    float randomFloat(float min, float max);
    int randomInt(int min, int max);

    // 游戏状态
    GameState m_state;
    int m_score;
    int m_level;
    int m_enemiesKilled;
    float m_spawnTimer;
    float m_currentSpawnInterval;
    bool m_bossActive;
    
    // 游戏对象
    std::unique_ptr<Player> m_player;
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    std::vector<std::unique_ptr<Bullet>> m_playerBullets;
    std::vector<std::unique_ptr<Bullet>> m_enemyBullets;
    std::vector<std::unique_ptr<Explosion>> m_explosions;
    std::vector<std::unique_ptr<PowerUp>> m_powerUps;
    
    // 星空背景
    struct Star {
        float x, y;
        float speed;
        int size;
        int alpha;
    };
    QList<Star> m_stars;
    
    // 计时器
    QTimer *m_gameTimer;
    QElapsedTimer m_elapsedTimer;
    qint64 m_lastFrameTime;
    
    // 输入状态
    QSet<int> m_pressedKeys;
    
    // 随机数生成
    std::mt19937 m_rng;
    
    // HUD
    GameHUD *m_hud;
};

#endif // GAMESCENE_H
