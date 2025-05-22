#include "rp_log.h"
#include "FileUtils.h"

#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <filesystem>
namespace fs = std::filesystem;


void RP_LOG_Init(const std::string& logFilePath)
{
    //// Create a file rotating logger with 5 MB size max and 3 rotated files
    //auto max_size = 1048576 * 5;
    //auto max_files = 3;
    //auto logger = spdlog::rotating_logger_mt("rp_log", logFilePath, max_size, max_files);
    //spdlog::set_default_logger(logger);

	std::string logAbsolutePath = getAbslutePath(logFilePath);

    auto sink1 = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
    //sink1->set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");
    //auto sink1 = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    //auto sink2 = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logAbsolutePath);
    
    // 创建 rotating_file_sink
    auto sink2 = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        logAbsolutePath,//"logs/ProgramNanny_%Y%m%d.log",    // 文件名包含日期
        1048576 * 10,                      // 10MB 每个文件
        3,                                // 最多保留 3 个文件
        false                             // 每次启动不重新生成一个log文件
    );

    spdlog::sinks_init_list sinks = { sink1,sink2 };

    auto logger = std::make_shared<spdlog::logger>("rp_logger", sinks.begin(), sinks.end());
    spdlog::set_default_logger(logger);
	logger->set_pattern("[%Y-%m-%d %H:%M:%S] %^[%L]%$ %v");
}
