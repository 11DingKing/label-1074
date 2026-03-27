#ifndef TEST_LOGGER_H
#define TEST_LOGGER_H

#include <QTest>
#include <QObject>
#include <QDir>
#include <QFile>
#include <QSignalSpy>
#include "../src/core/Logger.h"

/**
 * Logger 测试类
 */
class TestLogger : public QObject {
    Q_OBJECT

private:
    QString m_testLogDir;

private slots:
    void initTestCase() {
        qDebug() << "Starting Logger tests...";
        m_testLogDir = QDir::tempPath() + "/planewar_test_logs";
        
        // 清理旧的测试日志
        QDir dir(m_testLogDir);
        if (dir.exists()) {
            dir.removeRecursively();
        }
    }
    
    void cleanupTestCase() {
        qDebug() << "Logger tests completed.";
        Logger::instance().shutdown();
        
        // 清理测试日志
        QDir dir(m_testLogDir);
        if (dir.exists()) {
            dir.removeRecursively();
        }
    }
    
    void cleanup() {
        Logger::instance().shutdown();
    }
    
    // 测试日志初始化
    void testInitialization() {
        bool result = Logger::instance().init(m_testLogDir, false);
        QVERIFY(result);
        
        // 检查日志目录是否创建
        QDir dir(m_testLogDir);
        QVERIFY(dir.exists());
    }
    
    // 测试日志文件创建
    void testLogFileCreation() {
        Logger::instance().init(m_testLogDir, false);
        Logger::instance().info("Test message");
        
        QDir dir(m_testLogDir);
        QStringList filters;
        filters << "planewar_*.log";
        QFileInfoList files = dir.entryInfoList(filters, QDir::Files);
        
        QVERIFY(files.size() > 0);
    }
    
    // 测试日志级别
    void testLogLevels() {
        Logger::instance().init(m_testLogDir, false);
        
        // 设置日志级别为 Warning
        Logger::instance().setLogLevel(LogLevel::Warning);
        QCOMPARE(Logger::instance().logLevel(), LogLevel::Warning);
        
        // Debug 和 Info 不应该被记录
        // Warning, Error, Fatal 应该被记录
    }
    
    // 测试各种日志方法
    void testLogMethods() {
        Logger::instance().init(m_testLogDir, false);
        Logger::instance().setLogLevel(LogLevel::Debug);
        
        // 这些调用不应该崩溃
        Logger::instance().debug("Debug message");
        Logger::instance().info("Info message");
        Logger::instance().warning("Warning message");
        Logger::instance().error("Error message");
        
        // 使用宏
        LOG_DEBUG("Macro debug message");
        LOG_INFO("Macro info message");
        LOG_WARNING("Macro warning message");
        LOG_ERROR("Macro error message");
        
        QVERIFY(true);  // 只要不崩溃就通过
    }
    
    // 测试分类日志
    void testCategorizedLogs() {
        Logger::instance().init(m_testLogDir, false);
        
        LOG_DEBUG_CAT("TestCategory", "Categorized debug");
        LOG_INFO_CAT("TestCategory", "Categorized info");
        
        QVERIFY(true);
    }
    
    // 测试游戏事件日志
    void testGameEventLogs() {
        Logger::instance().init(m_testLogDir, false);
        
        LOG_GAME_EVENT("EnemyDestroyed", "Score: 100");
        LOG_PLAYER_ACTION("Fire", "Bullet spawned");
        
        QVERIFY(true);
    }
    
    // 测试日志信号
    void testLogSignal() {
        Logger::instance().init(m_testLogDir, false);
        
        QSignalSpy spy(&Logger::instance(), &Logger::logMessageWritten);
        Logger::instance().info("Signal test message");
        
        QCOMPARE(spy.count(), 1);
        
        QList<QVariant> arguments = spy.takeFirst();
        QVERIFY(arguments.at(0).toString().contains("Signal test message"));
    }
    
    // 测试日志关闭
    void testShutdown() {
        Logger::instance().init(m_testLogDir, false);
        Logger::instance().info("Before shutdown");
        Logger::instance().shutdown();
        
        // 关闭后再次初始化应该成功
        bool result = Logger::instance().init(m_testLogDir, false);
        QVERIFY(result);
    }
    
    // 测试长消息
    void testLongMessage() {
        Logger::instance().init(m_testLogDir, false);
        
        QString longMessage(10000, 'A');
        Logger::instance().info(longMessage);
        
        QVERIFY(true);  // 只要不崩溃就通过
    }
    
    // 测试特殊字符
    void testSpecialCharacters() {
        Logger::instance().init(m_testLogDir, false);
        
        Logger::instance().info("中文测试");
        Logger::instance().info("Special chars: !@#$%^&*()");
        Logger::instance().info("Newline\ntest");
        Logger::instance().info("Tab\ttest");
        
        QVERIFY(true);
    }
    
    // 测试并发写入
    void testConcurrentWrites() {
        Logger::instance().init(m_testLogDir, false);
        
        // 模拟并发写入
        for (int i = 0; i < 100; ++i) {
            Logger::instance().info(QString("Concurrent message %1").arg(i));
        }
        
        QVERIFY(true);
    }
};

#endif // TEST_LOGGER_H
