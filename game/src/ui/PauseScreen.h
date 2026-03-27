#ifndef PAUSESCREEN_H
#define PAUSESCREEN_H

#include <QWidget>
#include <QPushButton>

/**
 * 暂停界面
 * 显示暂停菜单和选项
 */
class PauseScreen : public QWidget {
    Q_OBJECT

public:
    explicit PauseScreen(QWidget *parent = nullptr);
    ~PauseScreen() override = default;

signals:
    void resumeClicked();
    void restartClicked();
    void menuClicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void setupUI();

    QPushButton *m_resumeButton;
    QPushButton *m_restartButton;
    QPushButton *m_menuButton;
};

#endif // PAUSESCREEN_H
