#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QTimer>
#include <QKeyEvent>
#include <memory>

class GameScene;
class StartScreen;
class PauseScreen;
class GameOverScreen;

/**
 * 游戏主窗口
 * 管理所有游戏界面和场景切换
 */
class GameWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit GameWindow(QWidget *parent = nullptr);
    ~GameWindow() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onStartGame();
    void onPauseGame();
    void onResumeGame();
    void onGameOver(int finalScore, int level, int enemiesKilled);
    void onRestartGame();
    void onBackToMenu();
    void onQuitGame();
    void onLevelCompleted(int level);

private:
    void setupUI();
    void setupConnections();
    void showStartScreen();
    void showGameScene();
    void showPauseScreen();
    void showGameOverScreen(int score);

    QStackedWidget *m_stackedWidget;
    StartScreen *m_startScreen;
    GameScene *m_gameScene;
    PauseScreen *m_pauseScreen;
    GameOverScreen *m_gameOverScreen;
    
    int m_lastScore;
};

#endif // GAMEWINDOW_H
