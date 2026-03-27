#include "GameScene.h"
#include "GameConfig.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Explosion.h"
#include "PowerUp.h"
#include "GameHUD.h"
#include <QPainter>
#include <QDebug>
#include <cmath>
#include <algorithm>

GameScene::GameScene(QWidget *parent)
    : QWidget(parent)
    , m_state(GameState::Ready)
    , m_score(0)
    , m_level(1)
    , m_enemiesKilled(0)
    , m_spawnTimer(0)
    , m_currentSpawnInterval(GameConfig::GameProgress::SPAWN_INTERVAL_MAX)
    , m_bossActive(false)
    , m_lastFrameTime(0)
    , m_rng(std::random_device{}())
{
    setFixedSize(GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);
    setFocusPolicy(Qt::StrongFocus);
    
    // 初始化游戏计时器
    m_gameTimer = new QTimer(this);
    connect(m_gameTimer, &QTimer::timeout, this, &GameScene::gameLoop);
    
    // 初始化HUD
    m_hud = new GameHUD(this);
    m_hud->setGeometry(0, 0, width(), height());
    m_hud->raise();
    
    // 初始化星空背景
    initStars();
    
    // 初始化玩家
    resetGame();
}

GameScene::~GameScene()
{
    stopGame();
}

void GameScene::initStars()
{
    m_stars.clear();
    for (int i = 0; i < GameConfig::Stars::COUNT; ++i) {
        Star star;
        star.x = randomFloat(0, width());
        star.y = randomFloat(0, height());
        star.speed = randomFloat(GameConfig::Stars::MIN_SPEED, GameConfig::Stars::MAX_SPEED);
        star.size = randomInt(GameConfig::Stars::MIN_SIZE, GameConfig::Stars::MAX_SIZE);
        star.alpha = randomInt(100, 255);
        m_stars.append(star);
    }
}

void GameScene::startGame()
{
    m_state = GameState::Playing;
    m_elapsedTimer.start();
    m_lastFrameTime = m_elapsedTimer.elapsed();
    m_gameTimer->start(GameConfig::FRAME_TIME);
    setFocus();
}

void GameScene::pauseGame()
{
    if (m_state == GameState::Playing) {
        m_state = GameState::Paused;
        m_gameTimer->stop();
        emit gamePaused();
    }
}

void GameScene::resumeGame()
{
    if (m_state == GameState::Paused) {
        m_state = GameState::Playing;
        m_lastFrameTime = m_elapsedTimer.elapsed();
        m_gameTimer->start(GameConfig::FRAME_TIME);
        setFocus();
    }
}

void GameScene::stopGame()
{
    m_gameTimer->stop();
    m_state = GameState::Ready;
}

void GameScene::resetGame()
{
    m_state = GameState::Ready;
    m_score = 0;
    m_level = 1;
    m_enemiesKilled = 0;
    m_spawnTimer = 0;
    m_currentSpawnInterval = GameConfig::GameProgress::SPAWN_INTERVAL_MAX;
    m_bossActive = false;
    
    // 清空所有游戏对象
    m_enemies.clear();
    m_playerBullets.clear();
    m_enemyBullets.clear();
    m_explosions.clear();
    m_powerUps.clear();
    m_pressedKeys.clear();
    
    // 重置玩家
    float playerX = width() / 2.0f;
    float playerY = height() - 100.0f;
    m_player = std::make_unique<Player>(QPointF(playerX, playerY));
    
    // 更新HUD
    if (m_hud) {
        m_hud->updateScore(m_score);
        m_hud->updateLevel(m_level);
        m_hud->updateLives(m_player->lives());
        m_hud->updateBombs(m_player->bombs());
    }
    
    update();
}

void GameScene::handleKeyPress(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;
    
    int key = event->key();
    m_pressedKeys.insert(key);
    
    // 处理炸弹
    if (key == Qt::Key_B && m_state == GameState::Playing) {
        useBomb();
    }
}

void GameScene::handleKeyRelease(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;
    m_pressedKeys.remove(event->key());
}

void GameScene::gameLoop()
{
    if (m_state != GameState::Playing) return;
    
    // 计算帧时间
    qint64 currentTime = m_elapsedTimer.elapsed();
    float deltaTime = (currentTime - m_lastFrameTime) / 1000.0f;
    m_lastFrameTime = currentTime;
    
    // 限制最大帧时间，防止跳帧
    if (deltaTime > 0.1f) deltaTime = 0.1f;
    
    // 更新游戏逻辑
    updateGame(deltaTime);
    
    // 重绘
    update();
}

void GameScene::updateGame(float deltaTime)
{
    // 更新各系统
    updateStars(deltaTime);
    updatePlayer(deltaTime);
    updateEnemies(deltaTime);
    updateBullets(deltaTime);
    updateExplosions(deltaTime);
    updatePowerUps(deltaTime);
    
    // 碰撞检测
    checkCollisions();
    
    // 清理不活动的对象
    cleanupInactiveObjects();
    
    // 生成敌机
    m_spawnTimer += deltaTime;
    if (m_spawnTimer >= m_currentSpawnInterval && !m_bossActive) {
        spawnEnemy();
        m_spawnTimer = 0;
        // 随机下一次生成间隔
        m_currentSpawnInterval = randomFloat(
            GameConfig::GameProgress::SPAWN_INTERVAL_MIN,
            GameConfig::GameProgress::SPAWN_INTERVAL_MAX
        ) / (1.0f + (m_level - 1) * 0.1f);
    }
    
    // 玩家射击
    if (m_pressedKeys.contains(Qt::Key_Space)) {
        playerFire();
    }
}

void GameScene::updatePlayer(float deltaTime)
{
    if (!m_player || !m_player->isActive()) return;
    
    // 处理移动输入
    float dx = 0, dy = 0;
    if (m_pressedKeys.contains(Qt::Key_Left) || m_pressedKeys.contains(Qt::Key_A)) {
        dx -= 1;
    }
    if (m_pressedKeys.contains(Qt::Key_Right) || m_pressedKeys.contains(Qt::Key_D)) {
        dx += 1;
    }
    if (m_pressedKeys.contains(Qt::Key_Up) || m_pressedKeys.contains(Qt::Key_W)) {
        dy -= 1;
    }
    if (m_pressedKeys.contains(Qt::Key_Down) || m_pressedKeys.contains(Qt::Key_S)) {
        dy += 1;
    }
    
    // 归一化对角移动速度
    if (dx != 0 && dy != 0) {
        float len = std::sqrt(dx * dx + dy * dy);
        dx /= len;
        dy /= len;
    }
    
    m_player->setMoveDirection(dx, dy);
    m_player->update(deltaTime);
    
    // 限制在屏幕内
    float margin = GameConfig::Player::MARGIN;
    float px = m_player->x();
    float py = m_player->y();
    float hw = m_player->width() / 2;
    float hh = m_player->height() / 2;
    
    if (px - hw < margin) m_player->setX(margin + hw);
    if (px + hw > width() - margin) m_player->setX(width() - margin - hw);
    if (py - hh < margin) m_player->setY(margin + hh);
    if (py + hh > height() - margin) m_player->setY(height() - margin - hh);
}

void GameScene::updateEnemies(float deltaTime)
{
    for (auto& enemy : m_enemies) {
        if (enemy && enemy->isActive()) {
            enemy->update(deltaTime);
            
            // 大型敌机和Boss射击
            if (enemy->canFire()) {
                enemyFire(enemy.get());
            }
            
            // 检查是否离开屏幕
            if (enemy->y() > height() + enemy->height()) {
                enemy->deactivate();
            }
        }
    }
}

void GameScene::updateBullets(float deltaTime)
{
    // 更新玩家子弹
    for (auto& bullet : m_playerBullets) {
        if (bullet && bullet->isActive()) {
            bullet->update(deltaTime);
            if (bullet->isOutOfBounds(width(), height())) {
                bullet->deactivate();
            }
        }
    }
    
    // 更新敌机子弹
    for (auto& bullet : m_enemyBullets) {
        if (bullet && bullet->isActive()) {
            bullet->update(deltaTime);
            if (bullet->isOutOfBounds(width(), height())) {
                bullet->deactivate();
            }
        }
    }
}

void GameScene::updateExplosions(float deltaTime)
{
    for (auto& explosion : m_explosions) {
        if (explosion && explosion->isActive()) {
            explosion->update(deltaTime);
        }
    }
}

void GameScene::updatePowerUps(float deltaTime)
{
    for (auto& powerUp : m_powerUps) {
        if (powerUp && powerUp->isActive()) {
            powerUp->update(deltaTime);
            if (powerUp->y() > height() + powerUp->height()) {
                powerUp->deactivate();
            }
        }
    }
}

void GameScene::updateStars(float deltaTime)
{
    for (auto& star : m_stars) {
        star.y += star.speed;
        if (star.y > height()) {
            star.y = 0;
            star.x = randomFloat(0, width());
        }
    }
}

void GameScene::spawnEnemy()
{
    // 检查是否该出Boss
    if (m_level % GameConfig::GameProgress::BOSS_INTERVAL == 0 && 
        m_enemiesKilled >= GameConfig::GameProgress::ENEMIES_PER_LEVEL - 1 &&
        !m_bossActive) {
        spawnBoss();
        return;
    }
    
    // 根据等级决定敌机类型概率
    float rand = randomFloat(0, 1);
    EnemyType type;
    
    if (rand < 0.6f) {
        type = EnemyType::Small;
    } else if (rand < 0.85f) {
        type = EnemyType::Medium;
    } else {
        type = EnemyType::Large;
    }
    
    // 随机X位置
    float x = randomFloat(50, width() - 50);
    float y = -50;
    
    auto enemy = std::make_unique<Enemy>(QPointF(x, y), type);
    
    // 根据等级增加属性
    float levelMultiplier = 1.0f + (m_level - 1) * 0.1f;
    enemy->setSpeedMultiplier(levelMultiplier);
    
    m_enemies.push_back(std::move(enemy));
}

void GameScene::spawnBoss()
{
    m_bossActive = true;
    float x = width() / 2.0f;
    float y = -80;
    
    auto boss = std::make_unique<Enemy>(QPointF(x, y), EnemyType::Boss);
    
    // Boss血量随等级增加
    int bossHealth = GameConfig::Enemy::Boss::HEALTH + (m_level / 5) * 10;
    boss->setHealth(bossHealth);
    
    m_enemies.push_back(std::move(boss));
}

void GameScene::spawnPowerUp(const QPointF& pos)
{
    // 随机决定道具类型
    float rand = randomFloat(0, 1);
    PowerUpType type;
    
    if (rand < 0.35f) {
        type = PowerUpType::Power;
    } else if (rand < 0.55f) {
        type = PowerUpType::Life;
    } else if (rand < 0.8f) {
        type = PowerUpType::Shield;
    } else {
        type = PowerUpType::Bomb;
    }
    
    auto powerUp = std::make_unique<PowerUp>(pos, type);
    m_powerUps.push_back(std::move(powerUp));
}

void GameScene::playerFire()
{
    if (!m_player || !m_player->canFire()) return;
    
    m_player->resetFireTimer();
    
    QPointF pos = m_player->position();
    int powerLevel = m_player->powerLevel();
    
    // 根据火力等级发射不同数量的子弹
    if (powerLevel == 1) {
        // 单发
        auto bullet = std::make_unique<Bullet>(
            QPointF(pos.x(), pos.y() - m_player->height() / 2),
            true
        );
        m_playerBullets.push_back(std::move(bullet));
    } else if (powerLevel == 2) {
        // 双发
        auto bullet1 = std::make_unique<Bullet>(
            QPointF(pos.x() - 12, pos.y() - m_player->height() / 2),
            true
        );
        auto bullet2 = std::make_unique<Bullet>(
            QPointF(pos.x() + 12, pos.y() - m_player->height() / 2),
            true
        );
        m_playerBullets.push_back(std::move(bullet1));
        m_playerBullets.push_back(std::move(bullet2));
    } else {
        // 三发（含扩散）
        auto bullet1 = std::make_unique<Bullet>(
            QPointF(pos.x(), pos.y() - m_player->height() / 2),
            true
        );
        auto bullet2 = std::make_unique<Bullet>(
            QPointF(pos.x() - 15, pos.y() - m_player->height() / 2),
            true, -0.3f
        );
        auto bullet3 = std::make_unique<Bullet>(
            QPointF(pos.x() + 15, pos.y() - m_player->height() / 2),
            true, 0.3f
        );
        m_playerBullets.push_back(std::move(bullet1));
        m_playerBullets.push_back(std::move(bullet2));
        m_playerBullets.push_back(std::move(bullet3));
    }
}

void GameScene::enemyFire(Enemy* enemy)
{
    if (!enemy) return;
    
    enemy->resetFireTimer();
    
    QPointF pos = enemy->position();
    
    if (enemy->type() == EnemyType::Boss) {
        // Boss发射多颗子弹
        for (int i = -2; i <= 2; ++i) {
            auto bullet = std::make_unique<Bullet>(
                QPointF(pos.x() + i * 20, pos.y() + enemy->height() / 2),
                false, i * 0.15f
            );
            m_enemyBullets.push_back(std::move(bullet));
        }
    } else {
        // 大型敌机单发
        auto bullet = std::make_unique<Bullet>(
            QPointF(pos.x(), pos.y() + enemy->height() / 2),
            false
        );
        m_enemyBullets.push_back(std::move(bullet));
    }
}

void GameScene::checkCollisions()
{
    checkPlayerBulletCollisions();
    checkEnemyBulletCollisions();
    checkPlayerEnemyCollisions();
    checkPlayerPowerUpCollisions();
}

void GameScene::checkPlayerBulletCollisions()
{
    for (auto& bullet : m_playerBullets) {
        if (!bullet || !bullet->isActive()) continue;
        
        for (auto& enemy : m_enemies) {
            if (!enemy || !enemy->isActive()) continue;
            
            if (bullet->collidesWith(*enemy)) {
                bullet->deactivate();
                enemy->damage(bullet->damage());
                
                if (!enemy->isAlive()) {
                    onEnemyDestroyed(enemy.get());
                }
                break;
            }
        }
    }
}

void GameScene::checkEnemyBulletCollisions()
{
    if (!m_player || !m_player->isActive() || m_player->isInvincible()) return;
    
    for (auto& bullet : m_enemyBullets) {
        if (!bullet || !bullet->isActive()) continue;
        
        if (bullet->collidesWith(*m_player)) {
            bullet->deactivate();
            onPlayerHit();
            break;
        }
    }
}

void GameScene::checkPlayerEnemyCollisions()
{
    if (!m_player || !m_player->isActive() || m_player->isInvincible()) return;
    
    for (auto& enemy : m_enemies) {
        if (!enemy || !enemy->isActive()) continue;
        
        if (m_player->collidesWith(*enemy)) {
            onPlayerHit();
            
            // 敌机也受到伤害
            enemy->damage(1);
            if (!enemy->isAlive()) {
                onEnemyDestroyed(enemy.get());
            }
            break;
        }
    }
}

void GameScene::checkPlayerPowerUpCollisions()
{
    if (!m_player || !m_player->isActive()) return;
    
    for (auto& powerUp : m_powerUps) {
        if (!powerUp || !powerUp->isActive()) continue;
        
        if (m_player->collidesWith(*powerUp)) {
            // 应用道具效果
            switch (powerUp->type()) {
                case PowerUpType::Life:
                    m_player->addLife();
                    m_hud->updateLives(m_player->lives());
                    break;
                case PowerUpType::Power:
                    m_player->powerUp();
                    break;
                case PowerUpType::Shield:
                    m_player->activateShield(GameConfig::PowerUp::SHIELD_DURATION);
                    break;
                case PowerUpType::Bomb:
                    m_player->addBomb();
                    m_hud->updateBombs(m_player->bombs());
                    break;
            }
            powerUp->deactivate();
        }
    }
}

void GameScene::onEnemyDestroyed(Enemy* enemy)
{
    if (!enemy) return;
    
    // 创建爆炸效果
    auto explosion = std::make_unique<Explosion>(
        enemy->position(),
        enemy->type() == EnemyType::Boss ? ExplosionSize::Large :
        enemy->type() == EnemyType::Large ? ExplosionSize::Medium :
        ExplosionSize::Small
    );
    m_explosions.push_back(std::move(explosion));
    
    // 增加分数
    int scoreValue = enemy->scoreValue();
    m_score += scoreValue;
    m_hud->updateScore(m_score);
    
    // 显示分数弹出效果
    m_hud->showScorePopup(scoreValue, enemy->position());
    
    // 检查是否为Boss
    if (enemy->type() == EnemyType::Boss) {
        m_bossActive = false;
        onLevelUp();
    }
    
    // 击杀计数
    m_enemiesKilled++;
    
    // 检查升级
    if (m_enemiesKilled >= GameConfig::GameProgress::ENEMIES_PER_LEVEL && 
        !m_bossActive &&
        m_level % GameConfig::GameProgress::BOSS_INTERVAL != 0) {
        onLevelUp();
    }
    
    // 随机掉落道具
    if (randomFloat(0, 1) < GameConfig::PowerUp::DROP_CHANCE) {
        spawnPowerUp(enemy->position());
    }
}

void GameScene::onPlayerHit()
{
    if (!m_player) return;
    
    m_player->takeDamage();
    
    // 创建爆炸效果
    auto explosion = std::make_unique<Explosion>(
        m_player->position(),
        ExplosionSize::Small
    );
    m_explosions.push_back(std::move(explosion));
    
    m_hud->updateLives(m_player->lives());
    
    if (m_player->lives() <= 0) {
        onPlayerDeath();
    }
}

void GameScene::onPlayerDeath()
{
    m_state = GameState::GameOver;
    m_gameTimer->stop();
    
    // 大爆炸效果
    auto explosion = std::make_unique<Explosion>(
        m_player->position(),
        ExplosionSize::Large
    );
    m_explosions.push_back(std::move(explosion));
    
    emit gameOver(m_score, m_level, m_enemiesKilled);
}

void GameScene::onLevelUp()
{
    int completedLevel = m_level;
    m_level++;
    m_enemiesKilled = 0;
    m_hud->updateLevel(m_level);
    
    // 奖励生命
    if (m_level % 3 == 0) {
        m_player->addLife();
        m_hud->updateLives(m_player->lives());
    }
    
    emit levelChanged(m_level);
    emit levelCompleted(completedLevel);  // 发出关卡完成信号
}

void GameScene::useBomb()
{
    if (!m_player || m_player->bombs() <= 0) return;
    
    m_player->useBomb();
    m_hud->updateBombs(m_player->bombs());
    
    // 清除所有敌机和敌机子弹
    for (auto& enemy : m_enemies) {
        if (enemy && enemy->isActive()) {
            // Boss只受到伤害
            if (enemy->type() == EnemyType::Boss) {
                enemy->damage(10);
                if (!enemy->isAlive()) {
                    onEnemyDestroyed(enemy.get());
                }
            } else {
                onEnemyDestroyed(enemy.get());
                enemy->deactivate();
            }
        }
    }
    
    for (auto& bullet : m_enemyBullets) {
        if (bullet) bullet->deactivate();
    }
}

void GameScene::cleanupInactiveObjects()
{
    // 使用 erase-remove idiom 清理不活动的对象
    m_enemies.erase(
        std::remove_if(m_enemies.begin(), m_enemies.end(),
            [](const std::unique_ptr<Enemy>& e) { return !e || !e->isActive(); }),
        m_enemies.end()
    );
    
    m_playerBullets.erase(
        std::remove_if(m_playerBullets.begin(), m_playerBullets.end(),
            [](const std::unique_ptr<Bullet>& b) { return !b || !b->isActive(); }),
        m_playerBullets.end()
    );
    
    m_enemyBullets.erase(
        std::remove_if(m_enemyBullets.begin(), m_enemyBullets.end(),
            [](const std::unique_ptr<Bullet>& b) { return !b || !b->isActive(); }),
        m_enemyBullets.end()
    );
    
    m_explosions.erase(
        std::remove_if(m_explosions.begin(), m_explosions.end(),
            [](const std::unique_ptr<Explosion>& e) { return !e || !e->isActive(); }),
        m_explosions.end()
    );
    
    m_powerUps.erase(
        std::remove_if(m_powerUps.begin(), m_powerUps.end(),
            [](const std::unique_ptr<PowerUp>& p) { return !p || !p->isActive(); }),
        m_powerUps.end()
    );
}

float GameScene::randomFloat(float min, float max)
{
    std::uniform_real_distribution<float> dist(min, max);
    return dist(m_rng);
}

int GameScene::randomInt(int min, int max)
{
    std::uniform_int_distribution<int> dist(min, max);
    return dist(m_rng);
}

void GameScene::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 渲染各层
    renderBackground(painter);
    renderStars(painter);
    renderPowerUps(painter);
    renderBullets(painter);
    renderEnemies(painter);
    renderPlayer(painter);
    renderExplosions(painter);
}

void GameScene::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (m_hud) {
        m_hud->setGeometry(0, 0, width(), height());
    }
}

void GameScene::renderBackground(QPainter& painter)
{
    // 绘制渐变背景
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, QColor(10, 10, 30));
    gradient.setColorAt(0.5, GameConfig::Colors::BACKGROUND);
    gradient.setColorAt(1, QColor(20, 20, 50));
    painter.fillRect(rect(), gradient);
}

void GameScene::renderStars(QPainter& painter)
{
    painter.setPen(Qt::NoPen);
    for (const auto& star : m_stars) {
        QColor color(255, 255, 255, star.alpha);
        painter.setBrush(color);
        painter.drawEllipse(QPointF(star.x, star.y), star.size, star.size);
    }
}

void GameScene::renderPlayer(QPainter& painter)
{
    if (m_player && m_player->isActive()) {
        m_player->render(painter);
    }
}

void GameScene::renderEnemies(QPainter& painter)
{
    for (const auto& enemy : m_enemies) {
        if (enemy && enemy->isActive()) {
            enemy->render(painter);
        }
    }
}

void GameScene::renderBullets(QPainter& painter)
{
    for (const auto& bullet : m_playerBullets) {
        if (bullet && bullet->isActive()) {
            bullet->render(painter);
        }
    }
    
    for (const auto& bullet : m_enemyBullets) {
        if (bullet && bullet->isActive()) {
            bullet->render(painter);
        }
    }
}

void GameScene::renderExplosions(QPainter& painter)
{
    for (const auto& explosion : m_explosions) {
        if (explosion && explosion->isActive()) {
            explosion->render(painter);
        }
    }
}

void GameScene::renderPowerUps(QPainter& painter)
{
    for (const auto& powerUp : m_powerUps) {
        if (powerUp && powerUp->isActive()) {
            powerUp->render(painter);
        }
    }
}

void GameScene::renderHUD(QPainter& painter)
{
    // HUD由独立的widget渲染，这里留空
    Q_UNUSED(painter)
}
