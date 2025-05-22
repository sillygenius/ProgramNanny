// Robim.Tools.ProgramNanny.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <chrono>

#include "MyApp.h"
#include "FileUtils.h"
#include "rp_log.h"
#include "WinTaskManager.h"

const std::wstring PRE_DELETE_TASK_NAME = L"Robim.Tools.ProgramNanny_PreDelete";
const std::wstring CLEARN_UP_TASK_NAME = L"Robim.Tools.ProgramNanny_CleanUp";

namespace fs = std::filesystem;


// 显示帮助信息
void showHelp() {
    std::cout << "ProgramNanny 文件管理工具使用说明:" << std::endl;
    std::cout << "用法:" << std::endl;
    std::cout << "  --pre-delete                           执行预删除操作" << std::endl;
    std::cout << "  --clean-up-pre-deleted                 清理预删除的文件" << std::endl;
    std::cout << "  --create-task --all                    创建预删除和清理预删除文件任务" << std::endl;
    std::cout << "  --create-task --pre-delete             创建预删除任务" << std::endl;
    std::cout << "  --create-task --clean-up-pre-deleted   创建清理预删除文件任务" << std::endl;
    std::cout << "  --remove-task --all                    删除预删除和清理预删除文件任务" << std::endl;
    std::cout << "  --remove-task --pre-delete             删除预删除任务" << std::endl;
    std::cout << "  --remove-task --clean-up-pre-deleted   删除清理预删除文件任务" << std::endl;
    std::cout << "  --help                                 显示此帮助信息" << std::endl;
}

int main(int argc, char* argv[]) {

    // 设置控制台为 UTF-8 编码
    SetConsoleOutputCP(CP_UTF8);

    // 初始化日志系统
    RP_LOG_Init("logs/ProgramNanny.log");

    // 解析命令行参数
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }

    // 处理空参数情况
    if (args.empty()) {
        std::cerr << "错误: 未提供任何参数" << std::endl;
        showHelp();
        return 1;
    }

    // 处理 --help 参数
    if (std::find(args.begin(), args.end(), "--help") != args.end()) {
        showHelp();
        return 0;
    }

    try {
        // 处理 --remove-task 参数
        if (args[0] == "--remove-task") {
            if (args.size() != 2) {
                std::cerr << "错误: --remove-task 需要一个参数(--pre-delete/--clean-up-pre-deleted/--all)" << std::endl;
                return 1;
            }
            const std::string& taskTypeArg = args[1];
            if (taskTypeArg == "--pre-delete")
            {
                WinTaskManager::removeScheduledTask(PRE_DELETE_TASK_NAME);
            }
            else if (taskTypeArg == "--clean-up-pre-deleted")
            {
                WinTaskManager::removeScheduledTask(CLEARN_UP_TASK_NAME);
            }
            else if (taskTypeArg == "--all")
            {
                WinTaskManager::removeScheduledTask(PRE_DELETE_TASK_NAME);
                WinTaskManager::removeScheduledTask(CLEARN_UP_TASK_NAME);
            }
            else
            {
                std::cerr << "参数错误!" << std::endl;
                return 1;
            }
            return 0;
        }

        // 处理 --create-task 参数
        if (args[0] == "--create-task") {
            if (args.size() != 2) {
                std::cerr << "错误: --create-task 需要一个参数 (--pre-delete/--clean-up-pre-deleted/--all)" << std::endl;
                return 1;
            }

            const std::string& taskTypeArg = args[1];
            if (taskTypeArg == "--pre-delete")
            {
				WinTaskManager::createScheduledTask(PRE_DELETE_TASK_NAME, L"", L"--pre-delete", L"daily");
			}
			else if (taskTypeArg == "--clean-up-pre-deleted")
			{
                WinTaskManager::createScheduledTask(CLEARN_UP_TASK_NAME, L"", L"--clean-up-pre-deleted", L"WEEKLY", L"00:00");
            }
            else if (taskTypeArg == "--all")
            {
                WinTaskManager::createScheduledTask(PRE_DELETE_TASK_NAME, L"", L"--pre-delete", L"daily");
                WinTaskManager::createScheduledTask(CLEARN_UP_TASK_NAME, L"", L"--clean-up-pre-deleted", L"WEEKLY", L"00:00");
            }
            else
            {
				std::cerr << "参数错误!" << std::endl;
                return 1;
            }
            return 0;
        }

        // 处理直接执行模式
        if (args.size() != 1) {
            std::cerr << "错误: 直接执行模式需要一个参数 (--pre-delete/--clean-up-pre-deleted)" << std::endl;
            return 1;
        }

        const std::string& command = args[0];

        if (command == "--pre-delete") {
            std::string configFile = "config.json";
            if (DeleteOldFilesByConfigFile(configFile)) {
                // 处理成功
            }
            else {
                // 处理失败
            }
            return 0;
        }
        else if (command == "--clean-up-pre-deleted") {
            if (!CleanUpPreDeletedFiles()) {
                return 1;
            }
        }
        else {
            std::cerr << "错误: 未知命令 - " << command << std::endl;
            return 1;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "运行时错误: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
