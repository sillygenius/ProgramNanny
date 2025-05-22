#pragma once	

#include <spdlog/spdlog.h>
#include <spdlog/fmt/std.h>

#define RP_LOG_INFO SPDLOG_INFO
#define RP_LOG_DEBUG SPDLOG_DEBUG
#define RP_LOG_WARN SPDLOG_WARN
#define RP_LOG_ERROR SPDLOG_ERROR

void RP_LOG_Init(const std::string& logFilePath);
