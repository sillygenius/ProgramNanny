#pragma once
#include <string>

class WinTaskManager
{
public:
	static void createScheduledTask(const std::wstring& exePath = L"");
	static void removeScheduledTask();
};

