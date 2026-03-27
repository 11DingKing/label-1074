#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QTimer>

/**
 * 游戏开始界面
 * 显示游戏标题和开始/退出按钮
 */
class StartScreen : public QWidget {
    Q_OBJECT

public:
    explicit StartScreen(QWidget *parent = nullptr);
    ~StartScreen() override = default;

signals:
    void startGameClicked();
    void quitGameClicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onAnimationTick();

private:
    void setupUI();
    void renderBackground(QPainter& painter);
    void renderTitle(QPainter& painter);
    void renderStars(QPainter& painter);
    
    QPushButton *m_startButton;
    QPushButton *m_quitButton;
    QTimer *m_animationTimer;
    
    float m_time;
    
    // 装饰星星
    struct Star {
        float x, y;
        float speed;
        int size;
        int alpha;
    };
    QList<Star> m_stars;
};

#endif // STARTSCREEN_H
