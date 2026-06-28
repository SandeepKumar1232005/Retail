#include "Logger.h"
#include <QDateTime>
#include <QDebug>
#include <iostream>

namespace RetailMS {

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

Logger::~Logger() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_file.isOpen()) {
        m_file.close();
    }
}

void Logger::init(const QString& logFilePath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_file.setFileName(logFilePath);
    if (m_file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        m_stream = std::make_unique<QTextStream>(&m_file);
    } else {
        qWarning() << "Failed to open log file:" << logFilePath;
    }
}

void Logger::log(LogLevel level, const QString& message, const QString& context) {
    QString levelStr;
    switch (level) {
        case LogLevel::Debug:   levelStr = "DEBUG"; break;
        case LogLevel::Info:    levelStr = "INFO "; break;
        case LogLevel::Warning: levelStr = "WARN "; break;
        case LogLevel::Error:   levelStr = "ERROR"; break;
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
    QString ctxStr = context.isEmpty() ? "" : QString("[%1] ").arg(context);
    QString logLine = QString("%1 | %2 | %3%4").arg(timestamp, levelStr, ctxStr, message);

    // Console output
    if (level == LogLevel::Error || level == LogLevel::Warning) {
        std::cerr << logLine.toStdString() << std::endl;
    } else {
        std::cout << logLine.toStdString() << std::endl;
    }

    // File output
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_stream) {
        *m_stream << logLine << "\n";
        m_stream->flush();
    }
}

void Logger::log(LogLevel level, const char* file, int line, const QString& message) {
    QString context = QString("%1:%2").arg(QString(file).section('\\', -1).section('/', -1)).arg(line);
    log(level, message, context);
}

} // namespace RetailMS
