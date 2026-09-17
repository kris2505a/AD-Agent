#pragma once
#include <string>
#include <format>
#include <print>
#include "core.h"


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

private:
	inline static auto logMessage(std::string_view logType, std::string_view msg) -> void {
		std::println("[{}] >> {}", logType, msg);
	}

};


