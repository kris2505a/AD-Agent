#pragma once
#include "public/user_service.h"
#include "win_directory_service.h"
#include <unordered_map>
#include "com_variant.h"

#include <shared_mutex>


class WinUserService : public IUserService {
public:
	WinUserService(WinDirectoryService& ds);
	~WinUserService() override;

	auto getUsers() -> std::vector<UserInfo> override;
	auto createUser(UserWriteInfo) -> std::expected<UserInfo, Error> override;
	auto getUser(std::string_view userName) -> std::expected<UserInfo, Error> override;
	auto modifyUser(UserWriteInfo) -> std::expected<UserInfo, Error> override;
	auto deleteUser(std::string_view) -> std::expected<void, Error> override;

private:
	auto setUserAttributes(IADsUser*, UserWriteInfo&) -> std::expected<UserInfo, Error>;

private:
	enum class SearchAttribute {
		UserName,
		DisplayName,
		Mail,
		SID,
		GUID,
		UserPrincipalName
	};

	auto putUserData(IADsUser*, UserAttribute, const ComVariant&) -> bool;
	auto putUserData(IADsUser*, SearchAttribute, const ComVariant&) -> bool;

private:
	WinDirectoryService& pDirectoryService;
	Microsoft::WRL::ComPtr<IDirectorySearch> mDirectorySearch;
	std::unordered_map<SearchAttribute, std::wstring> mSearchAttributes;
	std::unordered_map<UserAttribute, std::wstring> mUserAttributes;

	std::shared_mutex mMutex;
};