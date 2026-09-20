#pragma once
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <mutex>

namespace sentinel::nexus::core {

enum class LogLevel { DEBUG, INFO, WARN, ERROR, CRITICAL };

class Logger {
public:
    static Logger& instance() {
        static Logger inst;
        return inst;
    }

    void log(LogLevel level, const std::string& msg) {
        std::lock_guard<std::mutex> lock(log_mutex_);
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        std::cout << "\033[90m[" << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S")
                  << '.' << std::setfill('0') << std::setw(3) << ms.count() << "]\033[0m ";

        switch (level) {
            case LogLevel::DEBUG:    std::cout << "\033[36m[DEBUG]\033[0m "; break;
            case LogLevel::INFO:     std::cout << "\033[32m[INFO] \033[0m "; break;
            case LogLevel::WARN:     std::cout << "\033[33m[WARN] \033[0m "; break;
            case LogLevel::ERROR:    std::cout << "\033[31m[ERROR]\033[0m "; break;
            case LogLevel::CRITICAL: std::cout << "\033[41;37m[CRIT] \033[0m "; break;
        }

        std::cout << msg << std::endl;
    }

private:
    Logger() = default;
    std::mutex log_mutex_;
};

#define NEXUS_LOG_INFO(msg)    sentinel::nexus::core::Logger::instance().log(sentinel::nexus::core::LogLevel::INFO, msg)
#define NEXUS_LOG_WARN(msg)    sentinel::nexus::core::Logger::instance().log(sentinel::nexus::core::LogLevel::WARN, msg)
#define NEXUS_LOG_ERROR(msg)   sentinel::nexus::core::Logger::instance().log(sentinel::nexus::core::LogLevel::ERROR, msg)
#define NEXUS_LOG_CRIT(msg)    sentinel::nexus::core::Logger::instance().log(sentinel::nexus::core::LogLevel::CRITICAL, msg)

} // namespace sentinel::nexus::core