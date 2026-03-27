#include "GameHUD.h"
#include "GameConfig.h"
#include <QPainter>
#include <QPainterPath>
#include <cmath>

GameHUD::GameHUD(QWidget *parent)
    : QWidget(parent)
    , m_score(0)
    , m_lives(3)
    , m_level(1)
    , m_bombs(3)
    , m_damageAlpha(0)
    , m_levelUpAlpha(0)
    , m_levelUpNumber(0)
    , m_comboCount(0)
    , m_comboAlpha(0)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_TranslucentBackground);
    
    m_effectTimer = new QTimer(this);
    connect(m_effectTimer, &QTimer::timeout, this, &GameHUD::onEffectTick);
    m_effectTimer->start(16);  // 60fps
}

void GameHUD::updateScore(int score)
{
    m_score = score;
    update();
}

void GameHUD::updateLives(int lives)
{
    // 如果生命减少，触发受伤效果
    if (lives < m_lives) {
        showDamageEffect();
    }
    m_lives = lives;
    update();
}

void GameHUD::updateLevel(int level)
{
    if (level > m_level) {
        showLevelUpEffect(level);
    }
    m_level = level;
    update();
}

void GameHUD::updateBombs(int bombs)
{
    m_bombs = bombs;
    update();
}

void GameHUD::showDamageEffect()
{
    m_damageAlpha = 0.6f;
}

void GameHUD::showLevelUpEffect(int level)
{
    m_levelUpAlpha = 1.0f;
    m_levelUpNumber = level;
}

void GameHUD::showScorePopup(int score, const QPointF& pos)
{
    ScorePopup popup;
    popup.score = score;
    popup.x = pos.x();
    popup.y = pos.y();
    popup.alpha = 1.0f;
    popup.vy = -2.0f;
    m_scorePopups.append(popup);
}

void GameHUD::showCombo(int combo)
{
    m_comboCount = combo;
    m_comboAlpha = 1.0f;
}

void GameHUD::onEffectTick()
{
    bool needUpdate = false;
    
    // 衰减受伤效果
    if (m_damageAlpha > 0) {
        m_damageAlpha -= 0.03f;
        if (m_damageAlpha < 0) m_damageAlpha = 0;
        needUpdate = true;
    }
    
    // 衰减升级效果
    if (m_levelUpAlpha > 0) {
        m_levelUpAlpha -= 0.015f;
        if (m_levelUpAlpha < 0) m_levelUpAlpha = 0;
        needUpdate = true;
    }
    
    // 衰减连击显示
    if (m_comboAlpha > 0) {
        m_comboAlpha -= 0.02f;
        if (m_comboAlpha < 0) m_comboAlpha = 0;
        needUpdate = true;
    }
    
    // 更新分数弹出
    for (int i = m_scorePopups.size() - 1; i >= 0; --i) {
        ScorePopup& p = m_scorePopups[i];
        p.y += p.vy;
        p.alpha -= 0.02f;
        
        if (p.alpha <= 0) {
            m_scorePopups.removeAt(i);
        }
        needUpdate = true;
    }
    
    if (needUpdate) {
        update();
    }
}

void GameHUD::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    renderTopBar(painter);
    renderBottomBar(painter);
    renderEffects(painter);
}

void GameHUD::renderTopBar(QPainter& painter)
{
    int barHeight = 55;
    int padding = GameConfig::UI::PADDING_MEDIUM;
    
    // 顶部背景
    QLinearGradient bgGradient(0, 0, 0, barHeight);
    bgGradient.setColorAt(0, QColor(0, 0, 0, 200));
    bgGradient.setColorAt(0.7, QColor(0, 0, 0, 100));
    bgGradient.setColorAt(1, QColor(0, 0, 0, 0));
    painter.fillRect(0, 0, width(), barHeight, bgGradient);
    
    // === 左侧：分数 ===
    QFont labelFont;
    labelFont.setPixelSize(12);
    labelFont.setBold(false);
    painter.setFont(labelFont);
    
    // 分数标签
    painter.setPen(GameConfig::Colors::TEXT_SECONDARY);
    painter.drawText(padding, 18, "SCORE");
    
    // 分数值 - 更大更醒目
    QFont scoreFont;
    scoreFont.setPixelSize(22);
    scoreFont.setBold(true);
    painter.setFont(scoreFont);
    painter.setPen(GameConfig::Colors::HIGHLIGHT);
    QString scoreText = QString::number(m_score);
    painter.drawText(padding, 40, scoreText);
    
    // === 右侧：等级 ===
    painter.setFont(labelFont);
    painter.setPen(GameConfig::Colors::TEXT_SECONDARY);
    int levelLabelX = width() - padding - 50;
    painter.drawText(levelLabelX, 18, "LEVEL");
    
    // 等级值 - 居右对齐
    QFont levelFont;
    levelFont.setPixelSize(22);
    levelFont.setBold(true);
    painter.setFont(levelFont);
    painter.setPen(GameConfig::Colors::PLAYER);
    QString levelText = QString::number(m_level);
    QFontMetrics fm(levelFont);
    int levelTextWidth = fm.horizontalAdvance(levelText);
    painter.drawText(width() - padding - levelTextWidth, 40, levelText);
}

void GameHUD::renderBottomBar(QPainter& painter)
{
    int barHeight = 40;
    int barY = height() - barHeight;
    int padding = GameConfig::UI::PADDING_MEDIUM;
    Q_UNUSED(padding)
    
    // 底部背景
    QLinearGradient bgGradient(0, barY, 0, height());
    bgGradient.setColorAt(0, QColor(0, 0, 0, 0));
    bgGradient.setColorAt(1, QColor(0, 0, 0, 150));
    painter.fillRect(0, barY, width(), barHeight, bgGradient);
    
    // 生命图标和数量
    renderLives(painter);
    
    // 炸弹图标和数量
    renderBombs(painter);
}

void GameHUD::renderLives(QPainter& painter)
{
    int startX = GameConfig::UI::PADDING_MEDIUM;
    int y = height() - 25;
    int iconSize = 15;
    int spacing = 20;
    
    // 绘制生命图标（小飞机）
    for (int i = 0; i < m_lives; ++i) {
        int x = startX + i * spacing;
        
        painter.save();
        painter.translate(x, y);
        
        // 简化的飞机图标
        QPainterPath planePath;
        planePath.moveTo(0, -iconSize / 2);
        planePath.lineTo(-iconSize / 3, iconSize / 3);
        planePath.lineTo(0, iconSize / 6);
        planePath.lineTo(iconSize / 3, iconSize / 3);
        planePath.closeSubpath();
        
        painter.setPen(QPen(GameConfig::Colors::PLAYER.lighter(150), 1));
        painter.setBrush(GameConfig::Colors::PLAYER);
        painter.drawPath(planePath);
        
        painter.restore();
    }
    
    // 显示数字
    QFont font;
    font.setPixelSize(14);
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(GameConfig::Colors::TEXT);
    painter.drawText(startX + m_lives * spacing + 5, y + 5, QString("x%1").arg(m_lives));
}

void GameHUD::renderBombs(QPainter& painter)
{
    int startX = width() - 100;
    int y = height() - 25;
    int iconSize = 12;
    int spacing = 18;
    
    // 绘制炸弹图标
    for (int i = 0; i < m_bombs; ++i) {
        int x = startX + i * spacing;
        
        painter.save();
        painter.translate(x, y);
        
        // 炸弹主体
        painter.setPen(QPen(GameConfig::Colors::POWERUP_BOMB.lighter(150), 1));
        painter.setBrush(GameConfig::Colors::POWERUP_BOMB);
        painter.drawEllipse(QPointF(0, 2), iconSize / 2, iconSize / 2);
        
        // 引线
        painter.setPen(QPen(QColor(100, 100, 100), 2));
        painter.drawLine(0, -iconSize / 2 + 2, 3, -iconSize / 2 - 3);
        
        painter.restore();
    }
    
    // 显示数字
    QFont font;
    font.setPixelSize(14);
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(GameConfig::Colors::TEXT);
    painter.drawText(startX + m_bombs * spacing + 5, y + 5, QString("x%1").arg(m_bombs));
}

void GameHUD::renderEffects(QPainter& painter)
{
    // 受伤红屏效果
    if (m_damageAlpha > 0) {
        QColor damageColor(255, 0, 0, int(m_damageAlpha * 100));
        
        // 边缘渐变红色
        QRadialGradient radial(width() / 2, height() / 2, 
                               std::max(width(), height()) * 0.8);
        radial.setColorAt(0, QColor(255, 0, 0, 0));
        radial.setColorAt(0.7, QColor(255, 0, 0, int(m_damageAlpha * 50)));
        radial.setColorAt(1, QColor(255, 0, 0, int(m_damageAlpha * 150)));
        painter.fillRect(rect(), radial);
        
        // 边框闪红
        painter.setPen(QPen(damageColor, 8));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(rect().adjusted(4, 4, -4, -4));
    }
    
    // 升级效果
    if (m_levelUpAlpha > 0) {
        // 金色闪光
        int flashAlpha = int(m_levelUpAlpha * 50);
        painter.fillRect(rect(), QColor(255, 215, 0, flashAlpha));
        
        // 升级文字
        QFont levelFont;
        levelFont.setPixelSize(48);
        levelFont.setBold(true);
        painter.setFont(levelFont);
        
        QString levelText = QString("LEVEL %1").arg(m_levelUpNumber);
        
        // 文字发光效果
        int textAlpha = int(m_levelUpAlpha * 255);
        
        // 发光层
        painter.setPen(QColor(255, 215, 0, textAlpha / 2));
        for (int i = 0; i < 3; ++i) {
            painter.drawText(QRect(i, height() / 2 - 30 + i, width(), 60), 
                           Qt::AlignCenter, levelText);
        }
        
        // 主文字
        painter.setPen(QColor(255, 255, 255, textAlpha));
        painter.drawText(QRect(0, height() / 2 - 30, width(), 60), 
                        Qt::AlignCenter, levelText);
        
        // 副标题
        QFont subFont;
        subFont.setPixelSize(20);
        painter.setFont(subFont);
        painter.setPen(QColor(255, 215, 0, textAlpha));
        painter.drawText(QRect(0, height() / 2 + 25, width(), 30), 
                        Qt::AlignCenter, "COMPLETE!");
    }
    
    // 连击显示
    if (m_comboAlpha > 0 && m_comboCount > 1) {
        QFont comboFont;
        comboFont.setPixelSize(36);
        comboFont.setBold(true);
        painter.setFont(comboFont);
        
        int comboAlpha = int(m_comboAlpha * 255);
        QString comboText = QString("%1 COMBO!").arg(m_comboCount);
        
        // 连击颜色根据数量变化
        QColor comboColor;
        if (m_comboCount >= 10) {
            comboColor = QColor(255, 50, 50, comboAlpha);  // 红色
        } else if (m_comboCount >= 5) {
            comboColor = QColor(255, 150, 50, comboAlpha); // 橙色
        } else {
            comboColor = QColor(255, 255, 100, comboAlpha); // 黄色
        }
        
        painter.setPen(comboColor);
        painter.drawText(QRect(0, height() / 3, width(), 50), 
                        Qt::AlignCenter, comboText);
    }
    
    // 分数弹出
    QFont popupFont;
    popupFont.setPixelSize(18);
    popupFont.setBold(true);
    painter.setFont(popupFont);
    
    for (const auto& p : m_scorePopups) {
        int alpha = int(p.alpha * 255);
        painter.setPen(QColor(255, 255, 100, alpha));
        painter.drawText(QPointF(p.x, p.y), QString("+%1").arg(p.score));
    }
}
