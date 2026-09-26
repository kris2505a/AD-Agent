#pragma once
#include <string>
#include <filesystem>
#include <Windows.h>
#include <log.h>
#include <fstream>
#include <json.h>

class Config {
public:
	inline static std::string connectionString{ "" };
	inline static std::string url{ "" };
	inline static int port{ 0 };

	static auto load() {
		auto configPath = getExecutableDirectory() / "settings.json";
		std::ifstream config(configPath);


		if (!config.is_open()) {
			Log::error("Failed to find settings.json!");
			return -1;
		}

		std::string settingsContents;
		std::getline(config, settingsContents, '\0');

		nlohmann::json settings = nlohmann::json::parse(settingsContents);

		connectionString = settings.at("ADConnURL");
		url = settings.at("URL");
		port = settings.at("port");
	}

	inline static std::filesystem::path getExecutableDirectory() {
		wchar_t buffer[MAX_PATH];
		DWORD length = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
		if (0 == length) {
			Log::error("Failed to get module file name: {}", GetLastError());
			throw std::system_error(GetLastError(), std::system_category());
		}

		return std::filesystem::path(buffer).parent_path();
	}


};