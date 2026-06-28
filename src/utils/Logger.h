#pragma once

#include <QString>
#include <QFile>
#include <QTextStream>
#include <mutex>
#include <memory>

namespace RetailMS {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

class Logger {
public:
    static Logger& instance();

    void init(const QString& logFilePath);
    void log(LogLevel level, const QString& message, const QString& context = "");
    void log(LogLevel level, const char* file, int line, const QString& message);

    // Helpers
    static void debug(const QString& msg, const QString& context = "") { instance().log(LogLevel::Debug, msg, context); }
    static void info(const QString& msg, const QString& context = "") { instance().log(LogLevel::Info, msg, context); }
    static void warning(const QString& msg, const QString& context = "") { instance().log(LogLevel::Warning, msg, context); }
    static void error(const QString& msg, const QString& context = "") { instance().log(LogLevel::Error, msg, context); }

private:
    Logger() = default;
    ~Logger();
    
    QFile m_file;
    std::unique_ptr<QTextStream> m_stream;
    std::mutex m_mutex;
};

} // namespace RetailMS

#define LOG_DEBUG(msg) RetailMS::Logger::instance().log(RetailMS::LogLevel::Debug, __FILE__, __LINE__, msg)
#define LOG_INFO(msg)  RetailMS::Logger::instance().log(RetailMS::LogLevel::Info,  __FILE__, __LINE__, msg)
#define LOG_WARN(msg)  RetailMS::Logger::instance().log(RetailMS::LogLevel::Warning, __FILE__, __LINE__, msg)
#define LOG_ERROR(msg) RetailMS::Logger::instance().log(RetailMS::LogLevel::Error, __FILE__, __LINE__, msg)
