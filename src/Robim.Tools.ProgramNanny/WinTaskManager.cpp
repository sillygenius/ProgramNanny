#include "WinTaskManager.h"
#include <iostream>
#include <windows.h>
#include <string>
#include <shlwapi.h> // ��Ҫ���� Shlwapi.lib
#pragma comment(lib, "Shlwapi.lib")

const std::wstring SCHEDULED_TASK_NAME = L"Robim.Tools.ProgramNanny";

// ��ȡ��ǰ���̵Ŀ�ִ���ļ�·�������ַ��汾��
std::wstring getExecutablePath() {
    wchar_t path[MAX_PATH];
    DWORD result = GetModuleFileNameW(NULL, path, MAX_PATH);
    if (result == 0) {
        std::wcerr << L"�޷���ȡ��ִ���ļ�·����������: " << GetLastError() << std::endl;
        exit(1);
    }
    return std::wstring(path);
}

// ��ȡ��ִ���ļ����ڵ�Ŀ¼
std::wstring getExecutableDirectory(const std::wstring& exePath) {
    std::wstring directory = exePath;
    PathRemoveFileSpecW(directory.data()); // �Ƴ��ļ�����ֻ����Ŀ¼·��
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
        std::wcout << L"�ƻ������ѳɹ�������" << std::endl;
    }
    else {
        std::wcerr << L"�����ƻ�����ʧ�ܣ������룺" << result << std::endl;
    }
}


//void WinTaskManager::createScheduledTask(const std::wstring& exePath, const std::wstring& exeDir) {
//    std::wstring command = L"schtasks /create /tn \"MyDailyTask\" /tr \"";
//    command += exePath;
//    command += L"\" /sc daily /st 01:00 /sd 2025-05-01 /rl HIGHEST /ru SYSTEM /f";
//
//    int result = _wsystem(command.c_str());
//    if (result == 0) {
//        std::wcout << L"�ƻ������ѳɹ�������" << std::endl;
//    }
//    else {
//        std::wcerr << L"�����ƻ�����ʧ�ܣ������룺" << result << std::endl;
//    }
//}



void WinTaskManager::removeScheduledTask() {
    std::wstring command = L"schtasks /delete /tn \"" + SCHEDULED_TASK_NAME + L"\" /f";
    int result = _wsystem(command.c_str());
    if (result == 0) {
        std::wcout << L"�ƻ������ѳɹ�ɾ����" << std::endl;
    }
    else {
        std::wcerr << L"ɾ���ƻ�����ʧ�ܣ������룺" << result << std::endl;
    }
}


//
//int wmain(int argc, wchar_t* argv[]) {
//    if (argc < 2) {
//        std::wcerr << L"�÷�: " << argv[0] << L" --create-task �� " << argv[0] << L" --remove-task" << std::endl;
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
//        std::wcerr << L"��Ч�Ĳ���: " << arg << std::endl;
//        std::wcerr << L"�÷�: " << argv[0] << L" --create-task �� " << argv[0] << L" --remove-task" << std::endl;
//        return 1;
//    }
//
//    return 0;
//}