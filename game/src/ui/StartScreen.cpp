#include "StartScreen.h"
#include "GameConfig.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <random>
#include <cmath>

StartScreen::StartScreen(QWidget *parent)
    : QWidget(parent)
    , m_time(0)
{
    setFocusPolicy(Qt::StrongFocus);
    setupUI();
    
    // 初始化星星
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<float> xDist(0, GameConfig::WINDOW_WIDTH);
    std::uniform_real_distribution<float> yDist(0, GameConfig::WINDOW_HEIGHT);
    std::uniform_real_distribution<float> speedDist(0.5f, 2.0f);
    std::uniform_int_distribution<int> sizeDist(1, 3);
    std::uniform_int_distribution<int> alphaDist(100, 255);
    
    for (int i = 0; i < 80; ++i) {
        Star star;
        star.x = xDist(rng);
        star.y = yDist(rng);
        star.speed = speedDist(rng);
        star.size = sizeDist(rng);
        star.alpha = alphaDist(rng);
        m_stars.append(star);
    }
    
    // 动画计时器
    m_animationTimer = new QTimer(this);
    connect(m_animationTimer, &QTimer::timeout, this, &StartScreen::onAnimationTick);
    m_animationTimer->start(16);  // ~60fps
}

void StartScreen::setupUI()
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
        "    padding: 15px 40px;"
        "    min-width: 180px;"
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
    m_startButton = new QPushButton("开始游戏", this);
    m_startButton->setStyleSheet(buttonStyle);
    m_startButton->setFocusPolicy(Qt::NoFocus);
    
    m_quitButton = new QPushButton("退出游戏", this);
    m_quitButton->setStyleSheet(buttonStyle);
    m_quitButton->setFocusPolicy(Qt::NoFocus);
    
    // 布局
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addStretch(3);
    layout->addWidget(m_startButton, 0, Qt::AlignCenter);
    layout->addSpacing(20);
    layout->addWidget(m_quitButton, 0, Qt::AlignCenter);
    layout->addStretch(2);
    
    // 连接信号
    connect(m_startButton, &QPushButton::clicked, this, &StartScreen::startGameClicked);
    connect(m_quitButton, &QPushButton::clicked, this, &StartScreen::quitGameClicked);
}

void StartScreen::onAnimationTick()
{
    m_time += 0.016f;
    
    // 更新星星位置
    for (auto& star : m_stars) {
        star.y += star.speed;
        if (star.y > height()) {
            star.y = 0;
            std::random_device rd;
            std::mt19937 rng(rd());
            std::uniform_real_distribution<float> xDist(0, width());
            star.x = xDist(rng);
        }
    }
    
    update();
}

void StartScreen::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    renderBackground(painter);
    renderStars(painter);
    renderTitle(painter);
}

void StartScreen::renderBackground(QPainter& painter)
{
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, QColor(5, 5, 20));
    gradient.setColorAt(0.5, GameConfig::Colors::BACKGROUND);
    gradient.setColorAt(1, QColor(15, 15, 40));
    painter.fillRect(rect(), gradient);
}

void StartScreen::renderStars(QPainter& painter)
{
    painter.setPen(Qt::NoPen);
    for (const auto& star : m_stars) {
        QColor color(255, 255, 255, star.alpha);
        painter.setBrush(color);
        painter.drawEllipse(QPointF(star.x, star.y), star.size, star.size);
    }
}

void StartScreen::renderTitle(QPainter& painter)
{
    // 标题文字
    QString title = "飞机大战";
    QString subtitle = "PLANE WAR";
    
    // 标题字体
    QFont titleFont;
    titleFont.setPixelSize(48);
    titleFont.setBold(true);
    
    QFont subtitleFont;
    subtitleFont.setPixelSize(24);
    subtitleFont.setBold(true);
    subtitleFont.setLetterSpacing(QFont::AbsoluteSpacing, 8);
    
    // 计算位置
    int titleY = height() / 4;
    
    // 标题阴影
    painter.setFont(titleFont);
    painter.setPen(QColor(0, 0, 0, 100));
    QRect shadowRect(0, titleY + 3, width(), 60);
    painter.drawText(shadowRect, Qt::AlignCenter, title);
    
    // 标题渐变
    QLinearGradient titleGradient(width() / 2 - 100, titleY, width() / 2 + 100, titleY + 50);
    titleGradient.setColorAt(0, GameConfig::Colors::HIGHLIGHT);
    titleGradient.setColorAt(0.5, QColor(255, 255, 200));
    titleGradient.setColorAt(1, GameConfig::Colors::HIGHLIGHT);
    
    painter.setPen(QPen(QBrush(titleGradient), 1));
    QRect titleRect(0, titleY, width(), 60);
    painter.drawText(titleRect, Qt::AlignCenter, title);
    
    // 副标题
    painter.setFont(subtitleFont);
    painter.setPen(GameConfig::Colors::TEXT_SECONDARY);
    QRect subtitleRect(0, titleY + 60, width(), 30);
    painter.drawText(subtitleRect, Qt::AlignCenter, subtitle);
    
    // 装饰线
    int lineY = titleY + 100;
    int lineWidth = 200;
    
    QLinearGradient lineGradient(width() / 2 - lineWidth / 2, lineY, 
                                  width() / 2 + lineWidth / 2, lineY);
    lineGradient.setColorAt(0, QColor(255, 255, 255, 0));
    lineGradient.setColorAt(0.5, GameConfig::Colors::HIGHLIGHT);
    lineGradient.setColorAt(1, QColor(255, 255, 255, 0));
    
    painter.setPen(QPen(QBrush(lineGradient), 2));
    painter.drawLine(width() / 2 - lineWidth / 2, lineY, 
                     width() / 2 + lineWidth / 2, lineY);
    
    // 提示文字
    QFont hintFont;
    hintFont.setPixelSize(14);
    painter.setFont(hintFont);
    
    float alpha = 0.5f + 0.5f * sin(m_time * 2);
    painter.setPen(QColor(200, 200, 200, int(alpha * 200)));
    
    QRect hintRect(0, height() - 80, width(), 30);
    painter.drawText(hintRect, Qt::AlignCenter, "按 Enter 开始游戏");
    
    // 操作说明
    painter.setPen(GameConfig::Colors::TEXT_SECONDARY);
    QRect controlsRect(0, height() - 50, width(), 30);
    painter.drawText(controlsRect, Qt::AlignCenter, 
                     "移动: WASD/方向键 | 射击: 空格 | 炸弹: B | 暂停: ESC");
}

void StartScreen::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        emit startGameClicked();
    } else if (event->key() == Qt::Key_Escape) {
        emit quitGameClicked();
    }
    QWidget::keyPressEvent(event);
}
