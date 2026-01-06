#pragma once

#include <string>

namespace ZKT
{
namespace LOG
{
enum class Level
{
    Debug,
    Info,
    Warn,
    Error,
};

struct LogMessage
{
    std::string tag;
    std::string text;
    std::string color_code;  // ANSI escape, applied only to the tag. Leave empty for default.
};

/**
 * @brief Simple static logger with levels and basic ANSI formatting.
 */
class Logger
{
public:
    /**
     * @brief Logs a debug-level message (only in debug builds).
     */
    static void Debug(const std::string& message);
    /**
     * @brief Logs an info-level message.
     */
    static void Info(const std::string& message);
    /**
     * @brief Logs a warning-level message.
     */
    static void Warn(const std::string& message);
    /**
     * @brief Logs an error-level message.
     */
    static void Error(const std::string& message);
    /**
     * @brief Logs with a custom tag and optional color.
     */
    static void Custom(const LogMessage& message);

private:
    /**
     * @brief Internal formatter/dispatcher to stdout/stderr.
     */
    static void Print(Level level, const std::string& tag, const std::string& message, const char* tag_color);
    static const char* LevelColor(Level level);
    static const char* LevelTag(Level level);
};
}  // namespace LOG
}  // namespace ZKT

#ifdef NDEBUG
#define ZLOG_DEBUG(msg) ((void)0)
#else
#define ZLOG_DEBUG(msg) ::ZKT::LOG::Logger::Debug(msg)
#endif

#define ZLOG_INFO(msg) ::ZKT::LOG::Logger::Info(msg)
#define ZLOG_WARN(msg) ::ZKT::LOG::Logger::Warn(msg)
#define ZLOG_ERROR(msg) ::ZKT::LOG::Logger::Error(msg)
#define ZLOG_CUSTOM(logMessage) ::ZKT::LOG::Logger::Custom(logMessage)
