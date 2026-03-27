#include "Logger.h"
#include <QDir>
#include <QCoreApplication>
#include <QSysInfo>
#include <QStandardPaths>

Logger& Logger::instance()
{
    static Logger instance;
    return instance;
}

Logger::Logger()
    : m_logLevel(LogLevel::Debug)
    , m_consoleOutput(true)
    , m_initialized(false)
    , m_maxLogSize(10 * 1024 * 1024)  // 10MB
    , m_maxLogFiles(5)
{
}

Logger::~Logger()
{
    shutdown();
}

bool Logger::init(const QString& logDir, bool consoleOutput)
{
    QMutexLocker locker(&m_mutex);
    
    if (m_initialized) {
        return true;
    }
    
    m_consoleOutput = consoleOutput;
    m_logDir = logDir;
    
    // 创建日志目录
    QDir dir;
    if (!dir.exists(logDir)) {
        if (!dir.mkpath(logDir)) {
            qWarning() << "Failed to create log directory:" << logDir;
            return false;
        }
    }
    
    // 创建日志文件
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString logFileName = QString("%1/planewar_%2.log").arg(logDir).arg(timestamp);
    
    m_logFile.setFileName(logFileName);
    if (!m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        qWarning() << "Failed to open log file:" << logFileName;
        return false;
    }
    
    m_logStream.setDevice(&m_logFile);
    m_initialized = true;
    
    // 写入日志头
    m_logStream << "========================================\n";
    m_logStream << "  Plane War Game Log\n";
    m_logStream << "  Started: " << QDateTime::currentDateTime().toString(Qt::ISODate) << "\n";
    m_logStream << "========================================\n\n";
    m_logStream.flush();
    
    // 记录系统信息
    logSystemInfo();
    
    info("Logger initialized successfully", "System");
    
    return true;
}

void Logger::shutdown()
{
    QMutexLocker locker(&m_mutex);
    
    if (!m_initialized) {
        return;
    }
    
    m_logStream << "\n========================================\n";
    m_logStream << "  Log ended: " << QDateTime::currentDateTime().toString(Qt::ISODate) << "\n";
    m_logStream << "========================================\n";
    m_logStream.flush();
    
    m_logFile.close();
    m_initialized = false;
}

void Logger::debug(const QString& message, const QString& category)
{
    writeLog(LogLevel::Debug, message, category);
}

void Logger::info(const QString& message, const QString& category)
{
    writeLog(LogLevel::Info, message, category);
}

void Logger::warning(const QString& message, const QString& category)
{
    writeLog(LogLevel::Warning, message, category);
}

void Logger::error(const QString& message, const QString& category)
{
    writeLog(LogLevel::Error, message, category);
}

void Logger::fatal(const QString& message, const QString& category)
{
    writeLog(LogLevel::Fatal, message, category);
}

void Logger::log(LogLevel level, const QString& message, 
                  const QString& category, const QString& file, int line)
{
    QString fullMessage = message;
    if (!file.isEmpty()) {
        fullMessage = QString("%1 [%2:%3]").arg(message).arg(file).arg(line);
    }
    writeLog(level, fullMessage, category);
}

void Logger::logGameEvent(const QString& event, const QString& details)
{
    QString message = details.isEmpty() ? event : QString("%1 - %2").arg(event).arg(details);
    writeLog(LogLevel::Info, message, "GameEvent");
}

void Logger::logPlayerAction(const QString& action, const QString& details)
{
    QString message = details.isEmpty() ? action : QString("%1 - %2").arg(action).arg(details);
    writeLog(LogLevel::Debug, message, "PlayerAction");
}

void Logger::logSystemInfo()
{
    info(QString("OS: %1 %2").arg(QSysInfo::productType()).arg(QSysInfo::productVersion()), "System");
    info(QString("Kernel: %1").arg(QSysInfo::kernelVersion()), "System");
    info(QString("Architecture: %1").arg(QSysInfo::currentCpuArchitecture()), "System");
    info(QString("Qt Version: %1").arg(qVersion()), "System");
    
#ifdef QT_DEBUG
    info("Build Type: Debug", "System");
#else
    info("Build Type: Release", "System");
#endif
}

void Logger::writeLog(LogLevel level, const QString& message, const QString& category)
{
    // 检查日志级别
    if (level < m_logLevel) {
        return;
    }
    
    QMutexLocker locker(&m_mutex);
    
    QString formattedMessage = formatMessage(level, message, category);
    
    // 控制台输出
    if (m_consoleOutput) {
        QString colorCode = levelToColor(level);
        QString resetCode = "\033[0m";
        
        // 根据级别选择输出流
        QTextStream stream(level >= LogLevel::Warning ? stderr : stdout);
        stream << colorCode << formattedMessage << resetCode << "\n";
        stream.flush();
    }
    
    // 文件输出
    if (m_initialized) {
        m_logStream << formattedMessage << "\n";
        m_logStream.flush();
        
        // 检查是否需要轮转日志
        if (m_logFile.size() > m_maxLogSize) {
            rotateLogFile();
        }
    }
    
    // 发送信号
    emit logMessageWritten(formattedMessage, level);
}

QString Logger::levelToString(LogLevel level) const
{
    switch (level) {
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO ";
        case LogLevel::Warning: return "WARN ";
        case LogLevel::Error:   return "ERROR";
        case LogLevel::Fatal:   return "FATAL";
        default:                return "?????";
    }
}

QString Logger::levelToColor(LogLevel level) const
{
    switch (level) {
        case LogLevel::Debug:   return "\033[36m";  // 青色
        case LogLevel::Info:    return "\033[32m";  // 绿色
        case LogLevel::Warning: return "\033[33m";  // 黄色
        case LogLevel::Error:   return "\033[31m";  // 红色
        case LogLevel::Fatal:   return "\033[35m";  // 紫色
        default:                return "\033[0m";   // 默认
    }
}

QString Logger::formatMessage(LogLevel level, const QString& message, const QString& category) const
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
    return QString("[%1] [%2] [%3] %4")
        .arg(timestamp)
        .arg(levelToString(level))
        .arg(category, -12)
        .arg(message);
}

void Logger::rotateLogFile()
{
    m_logFile.close();
    
    // 删除最旧的日志文件
    QDir dir(m_logDir);
    QStringList filters;
    filters << "planewar_*.log";
    QFileInfoList files = dir.entryInfoList(filters, QDir::Files, QDir::Time);
    
    while (files.size() >= m_maxLogFiles) {
        QFile::remove(files.last().absoluteFilePath());
        files.removeLast();
    }
    
    // 创建新的日志文件
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString logFileName = QString("%1/planewar_%2.log").arg(m_logDir).arg(timestamp);
    
    m_logFile.setFileName(logFileName);
    m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    m_logStream.setDevice(&m_logFile);
    
    m_logStream << "========================================\n";
    m_logStream << "  Log rotated: " << QDateTime::currentDateTime().toString(Qt::ISODate) << "\n";
    m_logStream << "========================================\n\n";
    m_logStream.flush();
}
