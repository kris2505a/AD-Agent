#pragma once

#include "core.h"
#include <memory>
#include <string_view>

class AGENT_API IDirectoryService {
public:
	IDirectoryService() = default;
	virtual ~IDirectoryService() = default;

	static auto create(std::string_view connectionString) -> std::unique_ptr<IDirectoryService>;

	template <typename T>
	requires std::is_base_of_v<IDirectoryService, T>
	auto as() -> T& {
		return dynamic_cast<T&>(*this);
	}
};