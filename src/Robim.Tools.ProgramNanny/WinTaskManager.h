#pragma once
#include <string>

class WinTaskManager
{
public:
	// 创建计划任务，sc= MINUTE、 HOURLY、DAILY、WEEKLY、MONTHLY, ONCE, ONSTART, ONLOGON, ONIDLE, ONEVENT
	static void createScheduledTask(const std::wstring& taskName, const std::wstring& exePath = L"", const std::wstring& params = L"", const std::wstring& sc = L"daily", const std::wstring& st=L"01:00");
	static void removeScheduledTask(const std::wstring& taskName);
};

