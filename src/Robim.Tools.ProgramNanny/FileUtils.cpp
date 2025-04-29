#include "FileUtils.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// 实现 JSON 序列化函数
void to_json(json& j, const ConfigEntry& entry) {
	j = json{
		{"dir_name", entry.dir_name},
		{"keep_hours", entry.keep_hours.count()},
		{"includes", entry.includes},
		{"excludes", entry.excludes},
		{"enable", entry.enable}  // 处理新增字段
	};
}

// 实现 JSON 反序列化函数
void from_json(const json& j, ConfigEntry& entry) {
	entry.dir_name = j.at("dir_name").get<std::string>();
	entry.keep_hours = std::chrono::hours(j.at("keep_hours").get<int>());
	entry.includes = j.at("includes").get<std::string>();
	entry.excludes = j.at("excludes").get<std::string>();
	entry.enable = j.at("enable").get<bool>();  // 处理新增字段
}

// 实现 JSON 反序列化函数 for RootConfig
void from_json(const json& j, RootConfig& rootConfig) {
	rootConfig.root_dir = j.at("root_dir").get<std::string>();
	rootConfig.configs = j.at("configs").get<std::vector<ConfigEntry>>();
	for (auto& config : rootConfig.configs) {
		config.dir_name = (fs::path(rootConfig.root_dir) / config.dir_name).string();
	}
}

// 从 JSON 配置文件读取多组参数
bool readConfig(const std::string& configFile, RootConfig& rootConfig) {
	std::ifstream file(configFile);
	if (!file.is_open()) {
		RP_LOG_ERROR("Failed to open config file: {}", configFile);
		return false;
	}

	try {
		json j;
		file >> j;
		rootConfig = j.get<RootConfig>();
	}
	catch (const json::parse_error& e) {
		RP_LOG_ERROR("JSON parse error: {}", e.what());
		return false;
	}
	catch (const json::type_error& e) {
		RP_LOG_ERROR("JSON type error: {}", e.what());
		return false;
	}

	file.close();
	return true;
}




// 将通配符转换为正则表达式
std::string wildcardToRegex(const std::string& wildcard) {
	std::string regexStr;
	for (char c : wildcard) {
		switch (c) {
		case '*':
			regexStr += ".*";
			break;
		case '?':
			regexStr += ".";
			break;
		case '.':
			regexStr += "\\.";
			break;
		case '\\':
			regexStr += "\\\\";
			break;
		default:
			regexStr += c;
		}
	}
	return regexStr;
}

// 检查路径是否匹配排除项
bool matchesExclusion(const fs::path& path, const std::string& exclusion) {
	std::string regexStr = wildcardToRegex(exclusion);
	std::regex regexObj(regexStr);
	return std::regex_search(path.string(), regexObj);
}

// 解析排除列表
std::vector<std::string> parseExcludes(const std::string& exludes, const std::string& dir_name) {
	std::vector<std::string> excludesList;
	std::istringstream iss(exludes);
	std::string item;
	while (std::getline(iss, item, ';')) {
		if (!item.empty()) {
			// 处理相对路径
			if (!fs::path(item).is_absolute()) {
				item = (fs::path(dir_name) / item).string();
			}
			excludesList.push_back(item);
		}
	}
	return excludesList;
}

// 解析包含列表
std::vector<std::string> parseIncludes(const std::string& includes, const std::string& dir_name) {
	std::vector<std::string> includesList;
	std::istringstream iss(includes);
	std::string item;
	while (std::getline(iss, item, ';')) {
		if (!item.empty()) {
			// 处理相对路径
			if (!fs::path(item).is_absolute()) {
				item = (fs::path(dir_name) / item).string();
			}
			includesList.push_back(item);
		}
	}
	return includesList;
}

// 检查文件或文件夹是否在排除列表中
bool isExcluded(const fs::path& path, const std::vector<std::string>& excludesList) {
	for (const auto& exclude : excludesList) {
		if (matchesExclusion(path, exclude)) {
			return true;
		}
	}
	return false;
}

// 检查文件或文件夹是否在包含列表中
bool isIncluded(const fs::path& path, const std::vector<std::string>& includesList) {
	if (includesList.empty()) {
		return true;
	}
	for (const auto& include : includesList) {
		if (matchesExclusion(path, include)) {
			return true;
		}
	}
	return false;
}

// 将 std::chrono::system_clock::time_point 格式化为友好的字符串
std::string formatTimePoint(const std::chrono::system_clock::time_point& timePoint) {
	std::time_t timeT = std::chrono::system_clock::to_time_t(timePoint);
	std::tm* localTime = std::localtime(&timeT);
	std::ostringstream oss;
	oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
	return oss.str();
}


void DeleteOldFiles(const ConfigEntry& configItem)
{
	DeleteOldFiles(configItem.dir_name, configItem.keep_hours, configItem.excludes, configItem.includes);
}


// 获取文件夹信息（大小和文件数量）
std::pair<size_t, size_t> getFolderInfo(const std::string& dir_name, const std::string& exludes /*= ""*/) {
	size_t totalSize = 0;
	size_t fileCount = 0;
	auto excludesList = parseExcludes(exludes, dir_name);

	try {
		for (const auto& entry : fs::recursive_directory_iterator(dir_name)) {
			if (isExcluded(entry.path(), excludesList)) {
				continue;
			}
			if (entry.is_regular_file()) {
				totalSize += fs::file_size(entry.path());
				++fileCount;
			}
		}
	}
	catch (const fs::filesystem_error& e) {
		RP_LOG_ERROR("Error getting folder info: {}", e.what());
	}

	return { totalSize, fileCount };
}


std::string getSizeStr(size_t totalSize)
{
	std::string sizeStr;
	if (totalSize < 1024) {
		sizeStr = std::to_string(totalSize) + " B";
	}
	else if (totalSize < 1024 * 1024) {
		sizeStr = std::to_string(totalSize / 1024) + " KB";
	}
	else if (totalSize < 1024 * 1024 * 1024) {
		sizeStr = std::to_string(totalSize / (1024 * 1024)) + " MB";
	}
	else {
		sizeStr = std::to_string(totalSize / (1024 * 1024 * 1024)) + " GB";
	}
	return sizeStr;
}

// 打印文件夹信息
void printFolderInfo(const std::string& dir_name, size_t totalSize, size_t fileCount)
{
	std::string sizeStr = getSizeStr(totalSize);
	RP_LOG_INFO("Folder: {}, Size: {}, File Count: {}", dir_name, sizeStr, fileCount);
}

// 打印文件夹信息
void printFolderInfo(const std::string& dir_name, const std::string& exludes /*= ""*/) {
	auto [totalSize, fileCount] = getFolderInfo(dir_name, exludes);
	printFolderInfo(dir_name, totalSize, fileCount);
}


// 递归删除旧文件
template<typename Rep, typename Period>
void DeleteOldFiles(const std::string& dir_name, std::chrono::duration<Rep, Period> keep_duration, const std::string& exludes, const std::string& includes) {
	auto excludesList = parseExcludes(exludes, dir_name);
	auto includesList = parseIncludes(includes, dir_name);
	auto now = std::chrono::system_clock::now();

	//std::cout << "Keep duration: " << formatDuration(keep_duration) << std::endl;
	RP_LOG_INFO("Start delete [{}]. Keep duration: {}", dir_name, formatDuration(keep_duration));
	auto [totalSize, fileCount] = getFolderInfo(dir_name, exludes);
	printFolderInfo(dir_name, totalSize, fileCount);
	for (const auto& entry : fs::recursive_directory_iterator(dir_name)) {
		if (isExcluded(entry.path(), excludesList) || !isIncluded(entry.path(), includesList))
		{
			//std::cout << "Excluded: " << entry.path() << std::endl;
			RP_LOG_INFO("Excluded: {}", entry.path());
			continue;
		}
		if (entry.is_regular_file()) {
			auto lastWriteTime = fs::last_write_time(entry.path());
			auto lastWriteTimeSys = std::chrono::clock_cast<std::chrono::system_clock>(lastWriteTime);
			auto durationSinceLastWrite = now - lastWriteTimeSys;
			if (durationSinceLastWrite > keep_duration) {
				try
				{
					fs::remove(entry.path());
					//std::cout << "Deleted:[" << formatDuration(durationSinceLastWrite) << "] " << entry.path() << " : " << formatTimePoint(lastWriteTimeSys) << std::endl;
					RP_LOG_INFO("Deleted:[{}] {} : {}", formatDuration(durationSinceLastWrite), entry.path(), formatTimePoint(lastWriteTimeSys));
				}
				catch (const fs::filesystem_error& e)
				{
					//std::cerr << "Error deleting file " << entry.path() << ": " << e.what() << std::endl;
					RP_LOG_INFO("Error deleting file {}: {}", entry.path(), e.what());
				}
			}
			else
			{
				//std::cout << "Keep:[" << formatDuration(durationSinceLastWrite) << "] " << entry.path() << " : " << formatTimePoint(lastWriteTimeSys) << std::endl;
				RP_LOG_INFO("Keep:[{}] {} : {}", formatDuration(durationSinceLastWrite), entry.path(), formatTimePoint(lastWriteTimeSys));
			}
		}
	}
	RP_LOG_INFO("Start delete [{}].", dir_name);
	auto [totalSizeNew, fileCountNew] = getFolderInfo(dir_name, exludes);
	//printFolderInfo(dir_name, totalSizeNew, fileCountNew);
	RP_LOG_WARN("{} total size: {}, File count: {}. Free size: {}, Free file count: {}",
		dir_name, getSizeStr(totalSizeNew), fileCountNew, getSizeStr(totalSize - totalSizeNew), fileCount - fileCountNew);
	RP_LOG_INFO("End delete [{}].", dir_name);
}

// 根据配置文件删除旧文件
bool DeleteOldFilesByConfigFile(const std::string& configFile) {
	RootConfig rootConfig;

	if (!readConfig(configFile, rootConfig)) {
		RP_LOG_ERROR("Failed to read config file: {}", configFile);
		return false;
	}

	for (const auto& configItem : rootConfig.configs) {
		if (!configItem.enable) {
			RP_LOG_WARN("Skipping disabled config for directory: {}", configItem.dir_name);
			continue;
		}
		RP_LOG_INFO("Processing directory: {},keep_hours={},excludes={},includes={}", configItem.dir_name, configItem.keep_hours, configItem.excludes, configItem.includes);
		DeleteOldFiles(configItem.dir_name, configItem.keep_hours, configItem.excludes, configItem.includes);
	}
	return true;
}