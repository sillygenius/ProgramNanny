#pragma once

#include <iostream>
#include <filesystem>
#include <chrono>
#include <sstream>
#include <vector>
#include <string>
#include <regex>

#include "rp_log.h"

namespace fs = std::filesystem;

// 根据配置文件删除旧文件
bool DeleteOldFilesByConfigFile(const std::string& configFile);

// 定义结构体
struct ConfigEntry {
    std::string dir_name;
    std::chrono::hours keep_hours;
    std::string includes;
    std::string excludes;
    bool enable;  // 新增字段，用来表示配置是否生效
};

// 检查路径是否匹配排除项
bool matchesExclusion(const fs::path& path, const std::string& exclusion);
// 解析排除列表
std::vector<std::string> parseExcludes(const std::string& exludes, const std::string& dir_name);
// 解析包含列表
std::vector<std::string> parseIncludes(const std::string& includes, const std::string& dir_name);
// 检查文件或文件夹是否在排除列表中
bool isExcluded(const fs::path& path, const std::vector<std::string>& excludesList);
// 检查文件或文件夹是否在包含列表中
bool isIncluded(const fs::path& path, const std::vector<std::string>& includesList);
// 将 std::chrono::system_clock::time_point 格式化为友好的字符串
std::string formatTimePoint(const std::chrono::system_clock::time_point& timePoint);
// 打印文件夹信息
void printFolderInfo(const std::string& dir_name, const std::string& exludes = "");
// 打印文件夹信息
void printFolderInfo(const std::string& dir_name, size_t totalSize, size_t fileCount);

// 获取文件夹信息（大小和文件数量）
std::pair<size_t, size_t> getFolderInfo(const std::string& dir_name, const std::string& exludes = "");

std::string getSizeStr(size_t totalSize);

// 格式化时长为天:时:分:秒
template<typename Rep, typename Period>
std::string formatDuration(const std::chrono::duration<Rep, Period>& duration) {
    auto totalSeconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    auto days = totalSeconds / (24 * 60 * 60);
    totalSeconds %= (24 * 60 * 60);
    auto hours = totalSeconds / (60 * 60);
    totalSeconds %= (60 * 60);
    auto minutes = totalSeconds / 60;
    auto seconds = totalSeconds % 60;

    std::ostringstream oss;
    oss << days << ":" << hours << ":" << minutes << ":" << seconds;
    return oss.str();
}


// 递归删除旧文件
template<typename Rep, typename Period>
void DeleteOldFiles(const std::string& dir_name, std::chrono::duration<Rep, Period> keep_duration, const std::string& exludes = "", const std::string& includes = "");