#include "GameWindow.h"
#include "GameScene.h"
#include "StartScreen.h"
#include "PauseScreen.h"
#include "GameOverScreen.h"
#include "GameConfig.h"
#include <QApplication>

GameWindow::GameWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_stackedWidget(nullptr)
    , m_startScreen(nullptr)
    , m_gameScene(nullptr)
    , m_pauseScreen(nullptr)
    , m_gameOverScreen(nullptr)
    , m_lastScore(0)
{
    setupUI();
    setupConnections();
    showStartScreen();
}

GameWindow::~GameWindow()
{
    // Qt自动管理子对象的内存
}

void GameWindow::setupUI()
{
    // 设置窗口属性
    setWindowTitle("飞机大战 - Plane War");
    setFixedSize(GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);
    
    // 设置窗口样式
    setStyleSheet(QString("QMainWindow { background-color: %1; }")
                  .arg(GameConfig::Colors::BACKGROUND.name()));
    
    // 创建堆叠窗口管理器
    m_stackedWidget = new QStackedWidget(this);
    setCentralWidget(m_stackedWidget);
    
    // 创建各个界面
    m_startScreen = new StartScreen(this);
    m_gameScene = new GameScene(this);
    m_pauseScreen = new PauseScreen(this);
    m_gameOverScreen = new GameOverScreen(this);
    
    // 添加到堆叠窗口
    m_stackedWidget->addWidget(m_startScreen);
    m_stackedWidget->addWidget(m_gameScene);
    m_stackedWidget->addWidget(m_pauseScreen);
    m_stackedWidget->addWidget(m_gameOverScreen);
}

void GameWindow::setupConnections()
{
    // 开始界面信号
    connect(m_startScreen, &StartScreen::startGameClicked, 
            this, &GameWindow::onStartGame);
    connect(m_startScreen, &StartScreen::quitGameClicked,
            this, &GameWindow::onQuitGame);
    
    // 游戏场景信号
    connect(m_gameScene, &GameScene::gamePaused,
            this, &GameWindow::onPauseGame);
    connect(m_gameScene, &GameScene::gameOver,
            this, &GameWindow::onGameOver);
    connect(m_gameScene, &GameScene::levelCompleted,
            this, &GameWindow::onLevelCompleted);
    
    // 暂停界面信号
    connect(m_pauseScreen, &PauseScreen::resumeClicked,
            this, &GameWindow::onResumeGame);
    connect(m_pauseScreen, &PauseScreen::restartClicked,
            this, &GameWindow::onRestartGame);
    connect(m_pauseScreen, &PauseScreen::menuClicked,
            this, &GameWindow::onBackToMenu);
    
    // 游戏结束界面信号
    connect(m_gameOverScreen, &GameOverScreen::restartClicked,
            this, &GameWindow::onRestartGame);
    connect(m_gameOverScreen, &GameOverScreen::menuClicked,
            this, &GameWindow::onBackToMenu);
}

void GameWindow::keyPressEvent(QKeyEvent *event)
{
    // 如果在游戏场景中，传递按键事件
    if (m_stackedWidget->currentWidget() == m_gameScene) {
        if (event->key() == Qt::Key_Escape || event->key() == Qt::Key_P) {
            onPauseGame();
            return;
        }
        m_gameScene->handleKeyPress(event);
    }
    // 暂停界面中按ESC恢复游戏
    else if (m_stackedWidget->currentWidget() == m_pauseScreen) {
        if (event->key() == Qt::Key_Escape) {
            onResumeGame();
            return;
        }
    }
    
    QMainWindow::keyPressEvent(event);
}

void GameWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (m_stackedWidget->currentWidget() == m_gameScene) {
        m_gameScene->handleKeyRelease(event);
    }
    QMainWindow::keyReleaseEvent(event);
}

void GameWindow::closeEvent(QCloseEvent *event)
{
    m_gameScene->stopGame();
    QMainWindow::closeEvent(event);
}

void GameWindow::onStartGame()
{
    m_gameScene->startGame();
    showGameScene();
}

void GameWindow::onPauseGame()
{
    m_gameScene->pauseGame();
    showPauseScreen();
}

void GameWindow::onResumeGame()
{
    showGameScene();
    m_gameScene->resumeGame();
}

void GameWindow::onGameOver(int finalScore, int level, int enemiesKilled)
{
    m_lastScore = finalScore;
    m_gameOverScreen->setGameStats(finalScore, level, enemiesKilled);
    m_stackedWidget->setCurrentWidget(m_gameOverScreen);
    m_gameOverScreen->setFocus();
}

void GameWindow::onLevelCompleted(int level)
{
    // 关卡完成时可以添加庆祝效果
    // 这里暂时只是日志，后续可以添加弹窗或动画
    Q_UNUSED(level)
}

void GameWindow::onRestartGame()
{
    m_gameScene->resetGame();
    m_gameScene->startGame();
    showGameScene();
}

void GameWindow::onBackToMenu()
{
    m_gameScene->stopGame();
    m_gameScene->resetGame();
    showStartScreen();
}

void GameWindow::onQuitGame()
{
    m_gameScene->stopGame();
    QApplication::quit();
}

void GameWindow::showStartScreen()
{
    m_stackedWidget->setCurrentWidget(m_startScreen);
    m_startScreen->setFocus();
}

void GameWindow::showGameScene()
{
    m_stackedWidget->setCurrentWidget(m_gameScene);
    m_gameScene->setFocus();
}

void GameWindow::showPauseScreen()
{
    m_stackedWidget->setCurrentWidget(m_pauseScreen);
    m_pauseScreen->setFocus();
}

void GameWindow::showGameOverScreen(int score)
{
    m_gameOverScreen->setFinalScore(score);
    m_stackedWidget->setCurrentWidget(m_gameOverScreen);
    m_gameOverScreen->setFocus();
}
