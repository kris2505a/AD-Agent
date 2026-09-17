#pragma once
#include "public/user_service.h"
#include "win_directory_service.h"
#include <unordered_map>
#include "com_variant.h"


class WinUserService : public IUserService {
public:
	WinUserService(WinDirectoryService& ds);
	~WinUserService() override;

	auto getUsers() -> std::vector<UserInfo> override;
	auto createUser(UserWriteInfo) -> std::expected<UserInfo, UserError> override;
	auto getUser(std::string_view userName) -> std::expected<UserInfo, UserError> override;
	

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
};