#pragma once

#include "Logging.hpp"

namespace Age::Core
{
#define BAIL(level, message, ...)                                                                                      \
    do                                                                                                                 \
    {                                                                                                                  \
        Age::Core::log<Age::Core::LogLevel::level>(message __VA_OPT__(, ) __VA_ARGS__);                                \
        return;                                                                                                        \
    } while (0)

#define VBAIL(value, level, message, ...)                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        Age::Core::log<Age::Core::LogLevel::level>(message __VA_OPT__(, ) __VA_ARGS__);                                \
        return value;                                                                                                  \
    } while (0)

#define BAIL_IF(condition, level, message, ...)                                                                        \
    do                                                                                                                 \
    {                                                                                                                  \
        if (condition)                                                                                                 \
        {                                                                                                              \
            Age::Core::log<Age::Core::LogLevel::level>(message __VA_OPT__(, ) __VA_ARGS__);                            \
            return;                                                                                                    \
        }                                                                                                              \
    } while (0)

#define VBAIL_IF(condition, value, level, message, ...)                                                                \
    do                                                                                                                 \
    {                                                                                                                  \
        if (condition)                                                                                                 \
        {                                                                                                              \
            Age::Core::log<Age::Core::LogLevel::level>(message __VA_OPT__(, ) __VA_ARGS__);                            \
            return value;                                                                                              \
        }                                                                                                              \
    } while (0)

#define LOG_IF(condition, level, message, ...)                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
        if (condition)                                                                                                 \
            Age::Core::log<Age::Core::LogLevel::level>(message __VA_OPT__(, ) __VA_ARGS__);                            \
    } while (0)

#define BAIL_ERROR(message, ...) BAIL(ERROR, message, __VA_ARGS__)
#define BAIL_WARNING(message, ...) BAIL(WARNING, message, __VA_ARGS__)
#define BAIL_INFO(message, ...) BAIL(INFO, message, __VA_ARGS__)
#define BAIL_DEBUG(message, ...) BAIL(DEBUG, message, __VA_ARGS__)

#define VBAIL_ERROR(value, message, ...) VBAIL(value, ERROR, message, __VA_ARGS__)
#define VBAIL_WARNING(value, message, ...) VBAIL(value, WARNING, message, __VA_ARGS__)
#define VBAIL_INFO(value, message, ...) VBAIL(value, INFO, message, __VA_ARGS__)
#define VBAIL_DEBUG(value, message, ...) VBAIL(value, DEBUG, message, __VA_ARGS__)

#define BAIL_ERROR_IF(condition, message, ...) BAIL_IF(condition, ERROR, message, __VA_ARGS__)
#define BAIL_WARNING_IF(condition, message, ...) BAIL_IF(condition, WARNING, message, __VA_ARGS__)
#define BAIL_INFO_IF(condition, message, ...) BAIL_IF(condition, INFO, message, __VA_ARGS__)
#define BAIL_DEBUG_IF(condition, message, ...) BAIL_IF(condition, DEBUG, message, __VA_ARGS__)

#define VBAIL_ERROR_IF(condition, value, message, ...) VBAIL_IF(condition, value, ERROR, message, __VA_ARGS__)
#define VBAIL_WARNING_IF(condition, value, message, ...) VBAIL_IF(condition, value, WARNING, message, __VA_ARGS__)
#define VBAIL_INFO_IF(condition, value, message, ...) VBAIL_IF(condition, value, INFO, message, __VA_ARGS__)
#define VBAIL_DEBUG_IF(condition, value, message, ...) VBAIL_IF(condition, value, DEBUG, message, __VA_ARGS__)

#define LOG_ERROR_IF(condition, message, ...) LOG_IF(condition, ERROR, message, __VA_ARGS__)
#define LOG_WARNING_IF(condition, message, ...) LOG_IF(condition, WARNING, message, __VA_ARGS__)
#define LOG_INFO_IF(condition, message, ...) LOG_IF(condition, INFO, message, __VA_ARGS__)
#define LOG_DEBUG_IF(condition, message, ...) LOG_IF(condition, DEBUG, message, __VA_ARGS__)
} // namespace Age::Core
