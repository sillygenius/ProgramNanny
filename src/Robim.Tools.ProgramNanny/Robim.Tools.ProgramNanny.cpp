// Robim.Tools.ProgramNanny.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <chrono>

#include "MyApp.h"
#include "FileUtils.h"
#include "rp_log.h"
#include "WinTaskManager.h"

//int main(int argc, char ** argv)
//{
//    //std::cout << "Starting wxWidgets window from console..." << std::endl;
//    //MyApp::StartAppWindow(argc, argv);
//
//	RP_LOG_Init("logs/ProgramNanny.log");
//
//	//DeleteOldFiles(R"(F:\src\robim\web_cam\bin\Debug\net8.0-windows\Dump)", std::chrono::days(10), "NestWorkflow");
//
//	//DeleteOldFiles(R"(F:\src\robim\web_cam\bin\Debug\net8.0-windows\Resources)", std::chrono::days(10), "scene_*");
//	
//    std::string configFile = "config.json";
//    if (DeleteOldFilesByConfigFile(configFile)) {
//        // 处理成功
//    }
//    else {
//        // 处理失败
//    }
//}


int main(int argc, char* argv[]) {
	RP_LOG_Init("logs/ProgramNanny.log");

    if (argc < 2) {
        //std::cerr << "用法: " << argv[0] << " --create-task 或 " << argv[0] << " --remove-task" << std::endl;
        //return 1;

		std::string configFile = "config.json";
		if (DeleteOldFilesByConfigFile(configFile)) {
			// 处理成功
		}
		else {
			// 处理失败
		}
        return 0;
    }

    std::string arg = argv[1];
    if (arg == "--create-task") {
        WinTaskManager::createScheduledTask();
    }
    else if (arg == "--remove-task") {
        WinTaskManager::removeScheduledTask();
    }
    else {
        std::cerr << "无效的参数: " << arg << std::endl;
        std::cerr << "用法: " << argv[0] << " --create-task 或 " << argv[0] << " --remove-task" << std::endl;
        return 1;
    }

    return 0;
}
