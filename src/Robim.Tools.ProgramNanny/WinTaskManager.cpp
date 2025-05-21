#include "WinTaskManager.h"
#include <iostream>
#include <windows.h>
#include <string>
#include <shlwapi.h> // 需要链接 Shlwapi.lib
#pragma comment(lib, "Shlwapi.lib")

const std::wstring SCHEDULED_TASK_NAME = L"Robim.Tools.ProgramNanny";

// 获取当前进程的可执行文件路径（宽字符版本）
std::wstring getExecutablePath() {
    wchar_t path[MAX_PATH];
    DWORD result = GetModuleFileNameW(NULL, path, MAX_PATH);
    if (result == 0) {
        std::wcerr << L"无法获取可执行文件路径，错误码: " << GetLastError() << std::endl;
        exit(1);
    }
    return std::wstring(path);
}

// 获取可执行文件所在的目录
std::wstring getExecutableDirectory(const std::wstring& exePath) {
    std::wstring directory = exePath;
    PathRemoveFileSpecW(directory.data()); // 移除文件名，只保留目录路径
    return directory;
}


void WinTaskManager::createScheduledTask(const std::wstring& exePath) {
	const std::wstring& exePath_ = exePath.empty() ? getExecutablePath() : exePath;
	const std::wstring& exeDir = getExecutableDirectory(exePath_);
    std::wstring command = L"schtasks /create /tn \"" + SCHEDULED_TASK_NAME + L"\" /tr \"";
    command += exePath_;
    command += L"\" /sc daily /st 01:00 /sd 2025-05-01";

    int result = _wsystem(command.c_str());
    if (result == 0) {
        std::wcout << L"计划任务已成功创建！" << std::endl;
    }
    else {
        std::wcerr << L"创建计划任务失败，错误码：" << result << std::endl;
    }
}


//void WinTaskManager::createScheduledTask(const std::wstring& exePath, const std::wstring& exeDir) {
//    std::wstring command = L"schtasks /create /tn \"MyDailyTask\" /tr \"";
//    command += exePath;
//    command += L"\" /sc daily /st 01:00 /sd 2025-05-01 /rl HIGHEST /ru SYSTEM /f";
//
//    int result = _wsystem(command.c_str());
//    if (result == 0) {
//        std::wcout << L"计划任务已成功创建！" << std::endl;
//    }
//    else {
//        std::wcerr << L"创建计划任务失败，错误码：" << result << std::endl;
//    }
//}



void WinTaskManager::removeScheduledTask() {
    std::wstring command = L"schtasks /delete /tn \"" + SCHEDULED_TASK_NAME + L"\" /f";
    int result = _wsystem(command.c_str());
    if (result == 0) {
        std::wcout << L"计划任务已成功删除！" << std::endl;
    }
    else {
        std::wcerr << L"删除计划任务失败，错误码：" << result << std::endl;
    }
}


//
//int wmain(int argc, wchar_t* argv[]) {
//    if (argc < 2) {
//        std::wcerr << L"用法: " << argv[0] << L" --create-task 或 " << argv[0] << L" --remove-task" << std::endl;
//        return 1;
//    }
//
//    std::wstring arg = argv[1];
//    std::wstring exePath = getExecutablePath();
//
//    if (arg == L"--create-task") {
//        createScheduledTask(exePath);
//    }
//    else if (arg == L"--remove-task") {
//        removeScheduledTask();
//    }
//    else {
//        std::wcerr << L"无效的参数: " << arg << std::endl;
//        std::wcerr << L"用法: " << argv[0] << L" --create-task 或 " << argv[0] << L" --remove-task" << std::endl;
//        return 1;
//    }
//
//    return 0;
//}