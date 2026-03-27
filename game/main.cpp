#include <QApplication>
#include <QFontDatabase>
#include <QScreen>
#include <QDir>
#include <QTimer>
#include "GameWindow.h"
#include "GameConfig.h"
#include "Logger.h"

int main(int argc, char *argv[])
{
    // 在 macOS 上确保作为前台 GUI 应用运行
#ifdef Q_OS_MAC
    // 设置环境变量确保使用正确的 QPA 平台插件
    qputenv("QT_MAC_WANTS_LAYER", "1");
#endif

    QApplication app(argc, argv);
    
    // 设置应用程序信息
    app.setApplicationName("飞机大战");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("PlaneWar");
    
    // 初始化日志系统
    QString logDir = QDir::currentPath() + "/logs";
    if (!Logger::instance().init(logDir, true)) {
        qWarning() << "Failed to initialize logger, continuing without file logging";
    }
    
    LOG_INFO("=== Plane War Game Starting ===");
    LOG_INFO(QString("Application: %1 v%2")
             .arg(app.applicationName())
             .arg(app.applicationVersion()));
    
    // 设置全局样式
    app.setStyle("Fusion");
    LOG_DEBUG("UI style set to Fusion");
    
    // 创建并显示主窗口
    GameWindow window;
    LOG_INFO("Main window created");
    
    // 居中显示窗口
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - GameConfig::WINDOW_WIDTH) / 2;
    int y = (screenGeometry.height() - GameConfig::WINDOW_HEIGHT) / 2;
    window.move(x, y);
    
    LOG_INFO(QString("Window positioned at (%1, %2)").arg(x).arg(y));
    LOG_INFO(QString("Screen resolution: %1x%2")
             .arg(screenGeometry.width())
             .arg(screenGeometry.height()));
    
    // 显示并激活窗口
    window.show();
    window.raise();
    window.activateWindow();
    
#ifdef Q_OS_MAC
    // macOS: 延迟确保窗口正确显示
    QTimer::singleShot(100, [&window]() {
        window.raise();
        window.activateWindow();
    });
#endif
    
    LOG_INFO("Game window shown, entering main loop");
    
    int result = app.exec();
    
    LOG_INFO(QString("Application exiting with code: %1").arg(result));
    Logger::instance().shutdown();
    
    return result;
}
