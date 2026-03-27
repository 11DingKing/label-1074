#ifndef GAMEHUD_H
#define GAMEHUD_H

#include <QWidget>
#include <QLabel>
#include <QTimer>

/**
 * 游戏HUD（抬头显示）
 * 显示分数、生命、等级等信息
 * 支持各种视觉反馈效果
 */
class GameHUD : public QWidget {
    Q_OBJECT

public:
    explicit GameHUD(QWidget *parent = nullptr);
    ~GameHUD() override = default;

    void updateScore(int score);
    void updateLives(int lives);
    void updateLevel(int level);
    void updateBombs(int bombs);
    
    // 特效触发
    void showDamageEffect();           // 受伤红屏
    void showLevelUpEffect(int level); // 关卡完成
    void showScorePopup(int score, const QPointF& pos);  // 分数弹出
    void showCombo(int combo);         // 连击提示

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onEffectTick();

private:
    void renderTopBar(QPainter& painter);
    void renderBottomBar(QPainter& painter);
    void renderLives(QPainter& painter);
    void renderBombs(QPainter& painter);
    void renderEffects(QPainter& painter);

    int m_score;
    int m_lives;
    int m_level;
    int m_bombs;
    
    // 特效系统
    QTimer *m_effectTimer;
    float m_damageAlpha;       // 受伤红屏透明度
    float m_levelUpAlpha;      // 升级效果透明度
    int m_levelUpNumber;       // 升级的关卡号
    int m_comboCount;          // 连击数
    float m_comboAlpha;        // 连击显示透明度
    
    // 分数弹出
    struct ScorePopup {
        int score;
        float x, y;
        float alpha;
        float vy;
    };
    QList<ScorePopup> m_scorePopups;
};

#endif // GAMEHUD_H
