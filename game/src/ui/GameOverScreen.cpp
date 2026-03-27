#include "GameOverScreen.h"
#include "GameConfig.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QStandardPaths>
#include <cmath>
#include <random>

GameOverScreen::GameOverScreen(QWidget *parent)
    : QWidget(parent)
    , m_finalScore(0)
    , m_displayScore(0)
    , m_highScore(0)
    , m_level(1)
    , m_enemiesKilled(0)
    , m_isNewHighScore(false)
    , m_time(0)
    , m_shakeIntensity(0)
    , m_shakeOffsetX(0)
    , m_shakeOffsetY(0)
{
    setFocusPolicy(Qt::StrongFocus);
    setupUI();
    loadHighScore();
    
    m_animationTimer = new QTimer(this);
    connect(m_animationTimer, &QTimer::timeout, this, &GameOverScreen::onAnimationTick);
}

void GameOverScreen::setupUI()
{
    // 按钮样式
    QString buttonStyle = QString(
        "QPushButton {"
        "    background-color: %1;"
        "    color: %2;"
        "    border: 2px solid %3;"
        "    border-radius: %4px;"
        "    font-size: %5px;"
        "    font-weight: bold;"
        "    padding: 12px 30px;"
        "    min-width: 160px;"
        "}"
        "QPushButton:hover {"
        "    background-color: %6;"
        "    border-color: %7;"
        "}"
        "QPushButton:pressed {"
        "    background-color: %8;"
        "}"
    ).arg(GameConfig::Colors::BUTTON_NORMAL.name())
     .arg(GameConfig::Colors::TEXT.name())
     .arg(GameConfig::Colors::PRIMARY.name())
     .arg(GameConfig::UI::BORDER_RADIUS)
     .arg(GameConfig::UI::FONT_SIZE_MEDIUM)
     .arg(GameConfig::Colors::BUTTON_HOVER.name())
     .arg(GameConfig::Colors::HIGHLIGHT.name())
     .arg(GameConfig::Colors::BUTTON_PRESSED.name());
    
    // 创建按钮
    m_restartButton = new QPushButton("再玩一次", this);
    m_restartButton->setStyleSheet(buttonStyle);
    m_restartButton->setFocusPolicy(Qt::NoFocus);
    
    m_menuButton = new QPushButton("返回主菜单", this);
    m_menuButton->setStyleSheet(buttonStyle);
    m_menuButton->setFocusPolicy(Qt::NoFocus);
    
    // 布局
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addStretch(4);
    layout->addWidget(m_restartButton, 0, Qt::AlignCenter);
    layout->addSpacing(15);
    layout->addWidget(m_menuButton, 0, Qt::AlignCenter);
    layout->addStretch(1);
    
    // 连接信号
    connect(m_restartButton, &QPushButton::clicked, this, &GameOverScreen::restartClicked);
    connect(m_menuButton, &QPushButton::clicked, this, &GameOverScreen::menuClicked);
}

void GameOverScreen::loadHighScore()
{
    QSettings settings("PlaneWar", "Game");
    m_highScore = settings.value("highScore", 0).toInt();
}

void GameOverScreen::saveHighScore()
{
    if (m_finalScore > m_highScore) {
        m_highScore = m_finalScore;
        QSettings settings("PlaneWar", "Game");
        settings.setValue("highScore", m_highScore);
    }
}

QString GameOverScreen::getRankTitle() const
{
    if (m_finalScore >= 50000) return "传奇王牌";
    if (m_finalScore >= 30000) return "超级精英";
    if (m_finalScore >= 20000) return "空战大师";
    if (m_finalScore >= 10000) return "熟练飞行员";
    if (m_finalScore >= 5000) return "初级飞行员";
    if (m_finalScore >= 2000) return "新手学员";
    return "菜鸟入门";
}

QColor GameOverScreen::getRankColor() const
{
    if (m_finalScore >= 50000) return QColor(255, 215, 0);    // 金色
    if (m_finalScore >= 30000) return QColor(255, 100, 255);  // 紫色
    if (m_finalScore >= 20000) return QColor(255, 100, 100);  // 红色
    if (m_finalScore >= 10000) return QColor(100, 200, 255);  // 蓝色
    if (m_finalScore >= 5000) return QColor(100, 255, 100);   // 绿色
    if (m_finalScore >= 2000) return QColor(200, 200, 200);   // 白色
    return QColor(150, 150, 150);  // 灰色
}

void GameOverScreen::setFinalScore(int score)
{
    setGameStats(score, 1, 0);
}

void GameOverScreen::setGameStats(int score, int level, int enemiesKilled)
{
    m_finalScore = score;
    m_level = level;
    m_enemiesKilled = enemiesKilled;
    m_displayScore = 0;
    m_time = 0;
    m_isNewHighScore = (score > m_highScore);
    
    if (m_isNewHighScore) {
        saveHighScore();
        m_shakeIntensity = 15.0f;  // 新纪录时震动
    }
}

void GameOverScreen::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    m_displayScore = 0;
    m_time = 0;
    m_particles.clear();
    
    // 创建初始爆炸粒子
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<float> angleDist(0, 2 * M_PI);
    std::uniform_real_distribution<float> speedDist(2.0f, 8.0f);
    std::uniform_real_distribution<float> lifeDist(1.0f, 3.0f);
    std::uniform_int_distribution<int> sizeDist(2, 6);
    
    // 从中心爆发粒子
    for (int i = 0; i < 50; ++i) {
        Particle p;
        p.x = width() / 2.0f;
        p.y = height() / 3.0f;
        float angle = angleDist(rng);
        float speed = speedDist(rng);
        p.vx = cos(angle) * speed;
        p.vy = sin(angle) * speed;
        p.maxLife = lifeDist(rng);
        p.life = p.maxLife;
        p.size = sizeDist(rng);
        p.color = GameConfig::Colors::ACCENT;
        m_particles.append(p);
    }
    
    m_animationTimer->start(16);
}

void GameOverScreen::onAnimationTick()
{
    m_time += 0.016f;
    
    // 分数滚动动画
    if (m_displayScore < m_finalScore) {
        int increment = std::max(1, (m_finalScore - m_displayScore) / 15);
        m_displayScore = std::min(m_displayScore + increment, m_finalScore);
        
        // 分数增加时产生粒子
        if (m_displayScore % 500 == 0) {
            std::random_device rd;
            std::mt19937 rng(rd());
            std::uniform_real_distribution<float> angleDist(0, 2 * M_PI);
            std::uniform_real_distribution<float> speedDist(1.0f, 4.0f);
            
            for (int i = 0; i < 5; ++i) {
                Particle p;
                p.x = width() / 2.0f;
                p.y = height() / 3.0f + 80;
                float angle = angleDist(rng);
                float speed = speedDist(rng);
                p.vx = cos(angle) * speed;
                p.vy = sin(angle) * speed - 2;
                p.maxLife = 1.0f;
                p.life = p.maxLife;
                p.size = 3;
                p.color = GameConfig::Colors::HIGHLIGHT;
                m_particles.append(p);
            }
        }
    }
    
    // 更新粒子
    for (int i = m_particles.size() - 1; i >= 0; --i) {
        Particle& p = m_particles[i];
        p.x += p.vx;
        p.y += p.vy;
        p.vy += 0.1f;  // 重力
        p.life -= 0.016f;
        
        if (p.life <= 0) {
            m_particles.removeAt(i);
        }
    }
    
    // 新纪录时持续产生金色粒子
    if (m_isNewHighScore && m_displayScore >= m_finalScore) {
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_real_distribution<float> xDist(0, (float)width());
        std::uniform_real_distribution<float> speedDist(1.0f, 3.0f);
        
        if (m_particles.size() < 100) {
            Particle p;
            p.x = xDist(rng);
            p.y = height() + 10;
            p.vx = 0;
            p.vy = -speedDist(rng);
            p.maxLife = 3.0f;
            p.life = p.maxLife;
            p.size = 4;
            p.color = QColor(255, 215, 0, 200);
            m_particles.append(p);
        }
    }
    
    // 屏幕震动衰减
    if (m_shakeIntensity > 0) {
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_real_distribution<float> shakeDist(-1.0f, 1.0f);
        m_shakeOffsetX = shakeDist(rng) * m_shakeIntensity;
        m_shakeOffsetY = shakeDist(rng) * m_shakeIntensity;
        m_shakeIntensity *= 0.9f;
        if (m_shakeIntensity < 0.5f) m_shakeIntensity = 0;
    }
    
    update();
}

void GameOverScreen::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 应用屏幕震动
    painter.translate(m_shakeOffsetX, m_shakeOffsetY);
    
    // 背景渐变
    QLinearGradient bgGradient(0, 0, 0, height());
    bgGradient.setColorAt(0, QColor(30, 10, 10));
    bgGradient.setColorAt(0.5, QColor(50, 20, 30));
    bgGradient.setColorAt(1, QColor(20, 10, 20));
    painter.fillRect(rect(), bgGradient);
    
    // 半透明遮罩
    painter.fillRect(rect(), QColor(0, 0, 0, 100));
    
    // 绘制粒子
    for (const auto& p : m_particles) {
        float alpha = (p.life / p.maxLife) * 255;
        QColor color = p.color;
        color.setAlpha(int(alpha));
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        painter.drawEllipse(QPointF(p.x, p.y), p.size, p.size);
    }
    
    // 标题 - GAME OVER
    QFont titleFont;
    titleFont.setPixelSize(48);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    
    int titleY = height() / 6;
    
    // 标题阴影
    painter.setPen(QColor(100, 0, 0, 150));
    painter.drawText(QRect(3, titleY + 3, width(), 60), Qt::AlignCenter, "GAME OVER");
    
    // 标题渐变
    QLinearGradient titleGradient(width() / 2 - 150, titleY, 
                                   width() / 2 + 150, titleY + 50);
    titleGradient.setColorAt(0, GameConfig::Colors::ACCENT);
    titleGradient.setColorAt(0.5, QColor(255, 150, 150));
    titleGradient.setColorAt(1, GameConfig::Colors::ACCENT);
    
    painter.setPen(QPen(QBrush(titleGradient), 1));
    painter.drawText(QRect(0, titleY, width(), 60), Qt::AlignCenter, "GAME OVER");
    
    // 评价标题
    QString rankTitle = getRankTitle();
    QColor rankColor = getRankColor();
    
    QFont rankFont;
    rankFont.setPixelSize(28);
    rankFont.setBold(true);
    painter.setFont(rankFont);
    
    float rankGlow = 0.7f + 0.3f * sin(m_time * 4);
    QColor glowColor = rankColor;
    glowColor.setAlpha(int(rankGlow * 150));
    
    // 评价发光效果
    painter.setPen(glowColor);
    for (int i = 0; i < 3; ++i) {
        painter.drawText(QRect(0, titleY + 70 + i, width(), 40), Qt::AlignCenter, rankTitle);
    }
    painter.setPen(rankColor);
    painter.drawText(QRect(0, titleY + 70, width(), 40), Qt::AlignCenter, rankTitle);
    
    // 分数面板
    int panelWidth = 300;
    int panelHeight = 180;
    int panelX = (width() - panelWidth) / 2;
    int panelY = titleY + 130;
    
    QRect panelRect(panelX, panelY, panelWidth, panelHeight);
    
    // 面板背景
    painter.setPen(QPen(GameConfig::Colors::ACCENT.darker(150), 2));
    painter.setBrush(QColor(40, 20, 30, 220));
    painter.drawRoundedRect(panelRect, 12, 12);
    
    // 分数标签
    QFont labelFont;
    labelFont.setPixelSize(16);
    painter.setFont(labelFont);
    painter.setPen(GameConfig::Colors::TEXT_SECONDARY);
    painter.drawText(QRect(panelX, panelY + 15, panelWidth, 25), 
                     Qt::AlignCenter, "最终得分");
    
    // 分数数字
    QFont scoreFont;
    scoreFont.setPixelSize(46);
    scoreFont.setBold(true);
    painter.setFont(scoreFont);
    
    // 分数发光效果
    float glow = 0.5f + 0.5f * sin(m_time * 3);
    painter.setPen(QColor(255, 200, 100, int(glow * 100)));
    painter.drawText(QRect(panelX, panelY + 40, panelWidth, 55), 
                     Qt::AlignCenter, QString::number(m_displayScore));
    
    painter.setPen(GameConfig::Colors::HIGHLIGHT);
    painter.drawText(QRect(panelX, panelY + 40, panelWidth, 55), 
                     Qt::AlignCenter, QString::number(m_displayScore));
    
    // 统计信息
    QFont statsFont;
    statsFont.setPixelSize(14);
    painter.setFont(statsFont);
    painter.setPen(GameConfig::Colors::TEXT_SECONDARY);
    
    QString statsText = QString("关卡 %1  |  击杀 %2").arg(m_level).arg(m_enemiesKilled);
    painter.drawText(QRect(panelX, panelY + 100, panelWidth, 25), 
                     Qt::AlignCenter, statsText);
    
    // 最高分
    if (m_isNewHighScore) {
        // 新纪录闪烁效果
        float newRecordAlpha = 0.5f + 0.5f * sin(m_time * 5);
        painter.setPen(QColor(255, 215, 0, int(newRecordAlpha * 255)));
        QFont newRecordFont;
        newRecordFont.setPixelSize(18);
        newRecordFont.setBold(true);
        painter.setFont(newRecordFont);
        painter.drawText(QRect(panelX, panelY + 130, panelWidth, 30), 
                         Qt::AlignCenter, "★ 新纪录！★");
    } else {
        painter.setPen(GameConfig::Colors::TEXT_SECONDARY);
        painter.setFont(statsFont);
        painter.drawText(QRect(panelX, panelY + 130, panelWidth, 25), 
                         Qt::AlignCenter, QString("最高分: %1").arg(m_highScore));
    }
    
    // 提示文字
    QFont hintFont;
    hintFont.setPixelSize(14);
    painter.setFont(hintFont);
    
    float alpha = 0.5f + 0.5f * sin(m_time * 2);
    painter.setPen(QColor(200, 200, 200, int(alpha * 200)));
    
    QRect hintRect(0, height() - 50, width(), 30);
    painter.drawText(hintRect, Qt::AlignCenter, "按 Enter 重新开始  |  按 ESC 返回菜单");
}

void GameOverScreen::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        emit restartClicked();
    } else if (event->key() == Qt::Key_Escape) {
        emit menuClicked();
    }
    QWidget::keyPressEvent(event);
}
