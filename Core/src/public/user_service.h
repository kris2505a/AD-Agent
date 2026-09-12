#pragma once
#include "core.h"
#include "directory_service.h"
#include "user.h"
#include <vector>
#include <expected>

class AGENT_API IUserService {
public:

	enum class UserErrors {
		NotFound,
		UserNameNotAvailable,
		UnknownError
	};

	IUserService() = default;
	virtual ~IUserService() = default;
	virtual auto getUsers() -> std::vector<UserInfo> = 0;
	virtual auto createUser(UserInfo info) -> std::expected <UserInfo, UserErrors> = 0;


	static auto create(IDirectoryService& ds) -> std::unique_ptr<IUserService>;


};