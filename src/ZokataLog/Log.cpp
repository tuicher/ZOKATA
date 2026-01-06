#include "ZokataLog/Log.h"

#include <iostream>

namespace
{
constexpr const char* kReset = "\033[0m";
constexpr const char* kRed = "\033[31m";
constexpr const char* kYellow = "\033[33m";  // closest to orange in ANSI basic palette
constexpr const char* kCyan = "\033[36m";
constexpr const char* kGrey = "\033[90m";
}  // namespace

namespace ZKT
{
namespace LOG
{
const char* Logger::LevelColor(Level level)
{
    switch (level)
    {
    case Level::Debug:
        return kCyan;
    case Level::Warn:
        return kYellow;
    case Level::Error:
        return kRed;
    case Level::Info:
    default:
        return kGrey;
    }
}

const char* Logger::LevelTag(Level level)
{
    switch (level)
    {
    case Level::Debug:
        return "[DEBUG]";
    case Level::Warn:
        return "[WARN]";
    case Level::Error:
        return "[ERROR]";
    case Level::Info:
    default:
        return "[INFO]";
    }
}

void Logger::Print(Level level, const std::string& tag, const std::string& message, const char* tag_color)
{
    std::ostream& out = (level == Level::Error) ? std::cerr : std::cout;
    const char* color = tag_color ? tag_color : LevelColor(level);
    out << color << tag << kReset << " " << message << '\n';
}

void Logger::Debug(const std::string& message)
{
    Print(Level::Debug, LevelTag(Level::Debug), message, LevelColor(Level::Debug));
}

void Logger::Info(const std::string& message)
{
    Print(Level::Info, LevelTag(Level::Info), message, nullptr);
}

void Logger::Warn(const std::string& message)
{
    Print(Level::Warn, LevelTag(Level::Warn), message, LevelColor(Level::Warn));
}

void Logger::Error(const std::string& message)
{
    Print(Level::Error, LevelTag(Level::Error), message, LevelColor(Level::Error));
}

void Logger::Custom(const LogMessage& message)
{
    const std::string tag = message.tag.empty() ? "[CUSTOM]" : "[" + message.tag + "]";
    const char* color = message.color_code.empty() ? LevelColor(Level::Info) : message.color_code.c_str();
    Print(Level::Info, tag, message.text, color);
}
}  // namespace LOG
}  // namespace ZKT
