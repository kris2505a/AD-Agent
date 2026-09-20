#pragma once
#include "core.h"
#include "directory_service.h"
#include "user.h"
#include <vector>
#include <expected>

class AGENT_API IUserService {
public:

	enum class Error {
		NotFound,
		UserNameNotAvailable,
		FailedToSetAttribute,
		FailedToCreateUserObject,
		OtherErrors,
		FailedToSetPassword,
		PasswordPolicy,
		PermissionDenied,
		FailedToEnableUser,
		FailedToRetrieveUserObject,
		UnknownError,
		AlreadyExists
	};

	enum class UserAttribute {
		FirstName,
		LastName,
		Initial,
	};


	IUserService() = default;
	virtual ~IUserService() = default;
	virtual auto getUsers() -> std::vector<UserInfo> = 0;
	virtual auto createUser(UserWriteInfo) -> std::expected<UserInfo, Error> = 0;
	virtual auto getUser(std::string_view) -> std::expected<UserInfo, Error> = 0;
	virtual auto modifyUser(UserWriteInfo) -> std::expected<UserInfo, Error> = 0;
	virtual auto deleteUser(std::string_view) -> std::expected<void, Error> = 0;


	static auto create(IDirectoryService& ds) -> std::unique_ptr<IUserService>;


};