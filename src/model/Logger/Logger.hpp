#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <format>
#include <string>
#include <utility>
// Enum to represent log levels
enum LogLevel { DEBUG, INFO, WARNING, ERROR, NONE };

class Logger {

    public:
        static Logger& Instance()
        {
            static Logger instance;
            return instance;
        };

        void SetLevel(LogLevel level) { level_ = level; };

        template <typename... Args>
        void Log(LogLevel level, std::format_string<Args...> fmt, Args&&... args)
        {
                // if (level < level_) return;
                std::cout   << "[" << LevelName(level) << "] "
                            << std::format(fmt, std::forward<Args>(args)...)
                            << "\n";
        }

    private:
        LogLevel level_ = LogLevel::INFO;
        static const char* LevelName(LogLevel l) {
                switch (l) {
                    case LogLevel::DEBUG: return "DEBUG";
                    case LogLevel::INFO:  return "INFO";
                    case LogLevel::WARNING:  return "WARNING";
                    case LogLevel::ERROR: return "ERROR";
                    case LogLevel::NONE:  return "NONE";
                    default: return "";
                }
            }


};
#define LOG_DEBUG(...) Logger::Instance().Log(LogLevel::DEBUG, __VA_ARGS__)
#define LOG_INFO(...)  Logger::Instance().Log(LogLevel::INFO, __VA_ARGS__)
#define LOG_WARN(...)  Logger::Instance().Log(LogLevel::WARNING, __VA_ARGS__)
#define LOG_ERROR(...) Logger::Instance().Log(LogLevel::ERROR, __VA_ARGS__)

#endif
