#pragma once
#include "public/user_service.h"
#include "win_directory_service.h"
#include <unordered_map>

class WinUserService : public IUserService {
public:
	WinUserService(WinDirectoryService& ds);
	~WinUserService() override;

	auto getUsers() -> std::vector<UserInfo> override;
	auto createUser(UserInfo info) -> std::expected <UserInfo, UserErrors> override;



private:
	enum class SearchAttribute {
		UserName,
		DisplayName,
		Mail,
		SID,
		GUID,
		LastLogin
	};

	
private:
	WinDirectoryService& pDirectoryService;
	Microsoft::WRL::ComPtr<IDirectorySearch> mDirectorySearch;
	std::unordered_map<SearchAttribute, std::wstring> mSearchAttributes;
};