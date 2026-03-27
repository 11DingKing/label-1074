#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QDateTime>
#include <QDebug>

/**
 * 日志级别枚举
 */
enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

/**
 * 日志系统
 * 单例模式，支持文件输出和控制台输出
 */
class Logger : public QObject {
    Q_OBJECT

public:
    static Logger& instance();
    
    // 禁止拷贝
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // 初始化日志系统
    bool init(const QString& logDir = "logs", bool consoleOutput = true);
    
    // 关闭日志系统
    void shutdown();
    
    // 设置日志级别
    void setLogLevel(LogLevel level) { m_logLevel = level; }
    LogLevel logLevel() const { return m_logLevel; }
    
    // 日志记录方法
    void debug(const QString& message, const QString& category = "General");
    void info(const QString& message, const QString& category = "General");
    void warning(const QString& message, const QString& category = "General");
    void error(const QString& message, const QString& category = "General");
    void fatal(const QString& message, const QString& category = "General");
    
    // 带上下文的日志
    void log(LogLevel level, const QString& message, 
             const QString& category = "General",
             const QString& file = "", int line = 0);
    
    // 游戏事件专用日志
    void logGameEvent(const QString& event, const QString& details = "");
    void logPlayerAction(const QString& action, const QString& details = "");
    void logSystemInfo();

signals:
    void logMessageWritten(const QString& message, LogLevel level);

private:
    Logger();
    ~Logger();
    
    void writeLog(LogLevel level, const QString& message, const QString& category);
    QString levelToString(LogLevel level) const;
    QString levelToColor(LogLevel level) const;
    QString formatMessage(LogLevel level, const QString& message, const QString& category) const;
    void rotateLogFile();
    
    QFile m_logFile;
    QTextStream m_logStream;
    QMutex m_mutex;
    LogLevel m_logLevel;
    bool m_consoleOutput;
    bool m_initialized;
    QString m_logDir;
    int m_maxLogSize;  // 最大日志文件大小（字节）
    int m_maxLogFiles; // 最大日志文件数量
};

// 便捷宏定义
#define LOG_DEBUG(msg) Logger::instance().debug(msg, "General")
#define LOG_INFO(msg) Logger::instance().info(msg, "General")
#define LOG_WARNING(msg) Logger::instance().warning(msg, "General")
#define LOG_ERROR(msg) Logger::instance().error(msg, "General")
#define LOG_FATAL(msg) Logger::instance().fatal(msg, "General")

#define LOG_DEBUG_CAT(cat, msg) Logger::instance().debug(msg, cat)
#define LOG_INFO_CAT(cat, msg) Logger::instance().info(msg, cat)
#define LOG_WARNING_CAT(cat, msg) Logger::instance().warning(msg, cat)
#define LOG_ERROR_CAT(cat, msg) Logger::instance().error(msg, cat)

#define LOG_GAME_EVENT(event, details) Logger::instance().logGameEvent(event, details)
#define LOG_PLAYER_ACTION(action, details) Logger::instance().logPlayerAction(action, details)

#endif // LOGGER_H
