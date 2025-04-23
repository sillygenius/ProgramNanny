#include "rp_log.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

void RP_LOG_Init(const std::string& logFilePath)
{
    //// Create a file rotating logger with 5 MB size max and 3 rotated files
    //auto max_size = 1048576 * 5;
    //auto max_files = 3;
    //auto logger = spdlog::rotating_logger_mt("rp_log", logFilePath, max_size, max_files);
    //spdlog::set_default_logger(logger);

    auto sink1 = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
    //sink1->set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");
    //auto sink1 = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto sink2 = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath);
    spdlog::sinks_init_list sinks = { sink1,sink2 };

    auto logger = std::make_shared<spdlog::logger>("rp_logger", sinks.begin(), sinks.end());
    spdlog::set_default_logger(logger);
	logger->set_pattern("[%Y-%m-%d %H:%M:%S] %^[%L]%$ %v");
}
