#pragma once
#include <string>
#include <format>
#include <print>
#include "core.h"
#include <fstream>
#include <mutex>
#include <filesystem>

class Log {
public:
	template <typename... Args>
	inline static auto warn(std::string_view msg, Args... args) -> void {
		const auto logWithArgs = std::vformat(msg, std::make_format_args(args...));
		logMessage("wrn", logWithArgs);
	}

	template <typename... Args>
	inline static auto error(std::string_view msg, Args... args) -> void {
		const auto logWithArgs = std::vformat(msg, std::make_format_args(args...));
		logMessage("err", logWithArgs);
	}

	template <typename... Args>
	inline static auto debug(std::string_view msg, Args... args) -> void {
		const auto logWithArgs = std::vformat(msg, std::make_format_args(args...));
		logMessage("deb", logWithArgs);
	}

	template <typename... Args>
	inline static auto info(std::string_view msg, Args... args) -> void {
		const auto logWithArgs = std::vformat(msg, std::make_format_args(args...));
		logMessage("inf", logWithArgs);
	}

	static AGENT_API auto start(const std::filesystem::path& path) -> bool {
		sLogFile.open(path);

		return sLogFile.is_open();
	}

	static AGENT_API auto end() -> void {
		if (sLogFile.is_open()) {
			sLogFile.close();
		}
	}

private:
	inline static auto logMessage(std::string_view logType, std::string_view msg) -> void {
		std::lock_guard lock(sMutex);
		if (sLogFile.is_open()) {
			auto logString = std::format("[{}] => {}", logType, msg);
			sLogFile << logString << "\n";
		}
	}

	inline static std::ofstream sLogFile;
	inline static std::mutex sMutex;
};


