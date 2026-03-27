#ifndef GAMEOVERSCREEN_H
#define GAMEOVERSCREEN_H

#include <QWidget>
#include <QPushButton>
#include <QTimer>
#include <QList>
#include <QSettings>

/**
 * 游戏结束界面
 * 显示最终分数、评价、最高分和重玩选项
 */
class GameOverScreen : public QWidget {
    Q_OBJECT

public:
    explicit GameOverScreen(QWidget *parent = nullptr);
    ~GameOverScreen() override = default;

    void setFinalScore(int score);
    void setGameStats(int score, int level, int enemiesKilled);

signals:
    void restartClicked();
    void menuClicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void onAnimationTick();

private:
    void setupUI();
    void loadHighScore();
    void saveHighScore();
    QString getRankTitle() const;
    QColor getRankColor() const;

    QPushButton *m_restartButton;
    QPushButton *m_menuButton;
    
    int m_finalScore;
    int m_displayScore;
    int m_highScore;
    int m_level;
    int m_enemiesKilled;
    bool m_isNewHighScore;
    
    QTimer *m_animationTimer;
    float m_time;
    
    // 粒子系统
    struct Particle {
        float x, y;
        float vx, vy;
        float life;
        float maxLife;
        int size;
        QColor color;
    };
    QList<Particle> m_particles;
    
    // 屏幕震动
    float m_shakeIntensity;
    float m_shakeOffsetX;
    float m_shakeOffsetY;
};

#endif // GAMEOVERSCREEN_H
