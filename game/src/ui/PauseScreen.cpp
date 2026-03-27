#include "PauseScreen.h"
#include "GameConfig.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QKeyEvent>

PauseScreen::PauseScreen(QWidget *parent)
    : QWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setupUI();
}

void PauseScreen::setupUI()
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
    m_resumeButton = new QPushButton("继续游戏", this);
    m_resumeButton->setStyleSheet(buttonStyle);
    m_resumeButton->setFocusPolicy(Qt::NoFocus);
    
    m_restartButton = new QPushButton("重新开始", this);
    m_restartButton->setStyleSheet(buttonStyle);
    m_restartButton->setFocusPolicy(Qt::NoFocus);
    
    m_menuButton = new QPushButton("返回主菜单", this);
    m_menuButton->setStyleSheet(buttonStyle);
    m_menuButton->setFocusPolicy(Qt::NoFocus);
    
    // 布局
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addStretch(2);
    layout->addWidget(m_resumeButton, 0, Qt::AlignCenter);
    layout->addSpacing(15);
    layout->addWidget(m_restartButton, 0, Qt::AlignCenter);
    layout->addSpacing(15);
    layout->addWidget(m_menuButton, 0, Qt::AlignCenter);
    layout->addStretch(2);
    
    // 连接信号
    connect(m_resumeButton, &QPushButton::clicked, this, &PauseScreen::resumeClicked);
    connect(m_restartButton, &QPushButton::clicked, this, &PauseScreen::restartClicked);
    connect(m_menuButton, &QPushButton::clicked, this, &PauseScreen::menuClicked);
}

void PauseScreen::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 半透明背景
    painter.fillRect(rect(), QColor(0, 0, 0, 180));
    
    // 中央面板
    int panelWidth = 300;
    int panelHeight = 350;
    int panelX = (width() - panelWidth) / 2;
    int panelY = (height() - panelHeight) / 2;
    
    QRect panelRect(panelX, panelY, panelWidth, panelHeight);
    
    // 面板背景
    QLinearGradient panelGradient(panelX, panelY, panelX, panelY + panelHeight);
    panelGradient.setColorAt(0, QColor(30, 40, 60, 240));
    panelGradient.setColorAt(1, QColor(20, 25, 40, 240));
    
    painter.setPen(QPen(GameConfig::Colors::PRIMARY, 2));
    painter.setBrush(panelGradient);
    painter.drawRoundedRect(panelRect, 15, 15);
    
    // 标题
    QFont titleFont;
    titleFont.setPixelSize(32);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    painter.setPen(GameConfig::Colors::HIGHLIGHT);
    
    QRect titleRect(panelX, panelY + 30, panelWidth, 50);
    painter.drawText(titleRect, Qt::AlignCenter, "游戏暂停");
    
    // 装饰线
    int lineY = panelY + 85;
    QLinearGradient lineGradient(panelX + 30, lineY, panelX + panelWidth - 30, lineY);
    lineGradient.setColorAt(0, QColor(255, 255, 255, 0));
    lineGradient.setColorAt(0.5, GameConfig::Colors::PRIMARY);
    lineGradient.setColorAt(1, QColor(255, 255, 255, 0));
    
    painter.setPen(QPen(QBrush(lineGradient), 1));
    painter.drawLine(panelX + 30, lineY, panelX + panelWidth - 30, lineY);
    
    // 提示
    QFont hintFont;
    hintFont.setPixelSize(12);
    painter.setFont(hintFont);
    painter.setPen(GameConfig::Colors::TEXT_SECONDARY);
    
    QRect hintRect(panelX, panelY + panelHeight - 40, panelWidth, 30);
    painter.drawText(hintRect, Qt::AlignCenter, "按 ESC 继续游戏");
}

void PauseScreen::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        emit resumeClicked();
    } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        emit resumeClicked();
    }
    QWidget::keyPressEvent(event);
}
