#pragma once

#include <chrono>
#include <cstddef>
#include <format>
#include <iostream>
#include <string_view>
#include <utility>

namespace Age::Core
{
enum struct LogLevel
{
    ERROR,
    WARNING,
    INFO,
    DEBUG
};

constexpr std::string_view LOG_LEVEL_LABELS[] = {"ERROR", "WARNING", "INFO", "DEBUG"};

constexpr std::size_t LOG_BUFFER_SIZE{1024};

extern const std::chrono::time_zone *g_current_zone;

void init_logging();

template <LogLevel Level, typename... TArgs>
void log(std::format_string<TArgs...> format, TArgs &&...args)
{
    char buffer[LOG_BUFFER_SIZE];
    std::format_to_n_result<char *> output;

    auto now = std::chrono::system_clock::now();
    if (g_current_zone != nullptr)
        output = std::format_to_n(buffer, sizeof(buffer), "{:%T}: ", g_current_zone->to_local(now));
    else
        output = std::format_to_n(buffer, sizeof(buffer), "{:%T}: ", now);

    constexpr auto level = LOG_LEVEL_LABELS[static_cast<std::size_t>(Level)];
    output = std::format_to_n(output.out, sizeof(buffer) - (output.out - buffer), "{}: ", level);

    output = std::format_to_n(output.out, sizeof(buffer) - (output.out - buffer), format, std::forward<TArgs>(args)...);

    std::string_view message{buffer, output.out};
    std::cerr << message << '\n';
}

template <typename... TArgs>
void log_error(std::format_string<TArgs...> format, TArgs &&...args)
{
    log<LogLevel::ERROR>(format, std::forward<TArgs>(args)...);
}

template <typename... TArgs>
void log_warning(std::format_string<TArgs...> format, TArgs &&...args)
{
    log<LogLevel::WARNING>(format, std::forward<TArgs>(args)...);
}

template <typename... TArgs>
void log_info(std::format_string<TArgs...> format, TArgs &&...args)
{
    log<LogLevel::INFO>(format, std::forward<TArgs>(args)...);
}

template <typename... TArgs>
void log_debug(std::format_string<TArgs...> format, TArgs &&...args)
{
    log<LogLevel::DEBUG>(format, std::forward<TArgs>(args)...);
}
} // namespace Age::Core
