#include "win_user_service.h"
#include "public/log.h"

auto IUserService::create(IDirectoryService& ds) -> std::unique_ptr<IUserService> {
	auto& refDirService = ds.as<WinDirectoryService>();
	return std::make_unique<WinUserService>(refDirService);
}


WinUserService::WinUserService(WinDirectoryService& ds)
	: pDirectoryService(ds) {

	Log::info("Getting Directory Query interface");
	
	mDirectorySearch = pDirectoryService.getQueryInterface();
	
	mSearchAttributes = {
		{ SearchAttribute::UserName, L"sAMAccountName" },
		{ SearchAttribute::DisplayName, L"displayName" },
		{ SearchAttribute::Mail, L"mail" },
		{ SearchAttribute::SID, L"objectSid" },
		{ SearchAttribute::GUID, L"objectGUID" },
		{ SearchAttribute::LastLogin, L"lastLogonTimeStamp" }
	};

	Log::info("Directory Query interface retrived");
}

auto WinUserService::getUsers() -> std::vector <UserInfo> {

	Log::info("Retrieving users!");

	std::vector<SearchAttribute> searchAttributes{
		SearchAttribute::UserName,
		SearchAttribute::DisplayName,
		SearchAttribute::Mail
	};

	std::vector<LPWSTR> rawAttributes;

	for (auto attrib : searchAttributes) {
		rawAttributes.push_back(const_cast<LPWSTR>(mSearchAttributes.at(attrib).c_str()));
	}
		
	std::wstring filter =
		L"(&"
		L"(objectCategory=person)"
		L"(objectClass=user)"
		L"(!(sAMAccountName=Administrator))"
		L"(!(sAMAccountName=Guest))"
		L"(!(sAMAccountName=krbtgt))"
		L")";

	ADS_SEARCH_HANDLE handle{};

	Log::info("Executing search!");

	auto hr = mDirectorySearch->ExecuteSearch(
		const_cast<LPWSTR>(filter.c_str()),
		rawAttributes.data(),
		rawAttributes.size(),
		&handle
	);

	if (FAILED(hr)) {
		Log::error("Failed to retrive users: {}", getMessage(hr));
		throw std::runtime_error("failed to retrieve data");
	}

	std::vector <UserInfo> users;

	Log::info("Retrieving data from search handle");

	HRESULT status;

	while (S_OK == (status = mDirectorySearch->GetNextRow(handle))) {
		
		if (status == S_ADS_NOMORE_ROWS) {
			break;
		}

		Log::info("Row retrieved.");

		UserInfo info;

		for (auto& attrib : searchAttributes) {
			ADS_SEARCH_COLUMN column{};
			hr = mDirectorySearch->GetColumn(handle, const_cast<LPWSTR>(mSearchAttributes.at(attrib).c_str()), &column);

			if (FAILED(hr)) {
				continue;
			}

			if (0 == column.dwNumValues) {
				mDirectorySearch->FreeColumn(&column);
				continue;
			}

			Log::info("Column retrieved at attrib: {}", toNarrow(mSearchAttributes.at(attrib)));

			switch (attrib) {
			case SearchAttribute::UserName:
				info.userName = toNarrow(column.pADsValues[0].CaseIgnoreString);
				Log::info("UserName: {}", info.userName);
				break;

			case SearchAttribute::DisplayName:
				info.displayName = toNarrow(column.pADsValues[0].CaseIgnoreString);
				Log::info("DisplayName: {}", info.displayName);
				break;

			case SearchAttribute::Mail:
				info.mail = toNarrow(column.pADsValues[0].CaseIgnoreString);
				Log::info("Mail: {}", info.mail);
				break;
			}

			hr = mDirectorySearch->FreeColumn(&column);
			if (FAILED(hr)) {
				Log::error("Failed to free column!");
			}

			Log::info("Free column returned {}", getMessage(hr));
		}
		users.push_back(info);
	}
	Log::info("Successfully retrieved users!");
	hr = mDirectorySearch->CloseSearchHandle(handle);

	if (FAILED(hr)) {
		Log::error("Failed to close search handle: {}", getMessage(hr));
		throw std::runtime_error("Failed to release resource.");
	}

	Log::info("Closing handle returned: {}", getMessage(hr));

	return users;
}

auto WinUserService::createUser(UserInfo info) -> std::expected<UserInfo, UserErrors> {
	return std::unexpected{ UserErrors::UnknownError };
}

WinUserService::~WinUserService() {
}
