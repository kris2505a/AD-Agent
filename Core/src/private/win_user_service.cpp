#include "win_user_service.h"
#include "public/log.h"
#include "com_string.h"

auto IUserService::create(IDirectoryService& ds) -> std::unique_ptr<IUserService> {
	auto& refDirService = ds.as<WinDirectoryService>();
	return std::make_unique<WinUserService>(refDirService);
}


WinUserService::WinUserService(WinDirectoryService& ds)
	: pDirectoryService(ds) {

	Log::info("Getting Directory Query interface");
	
	mDirectorySearch = pDirectoryService.getQueryInterface();
	
	mSearchAttributes = {
		{ SearchAttribute::UserName,			L"sAMAccountName" },
		{ SearchAttribute::DisplayName,			L"displayName" },
		{ SearchAttribute::Mail,				L"mail" },
		{ SearchAttribute::SID,					L"objectSid" },
		{ SearchAttribute::GUID,				L"objectGUID" },
		{ SearchAttribute::UserPrincipalName,	L"userPrincipalName" }
	};

	mUserAttributes = {
		{ UserAttribute::FirstName, L"givenName" },
		{ UserAttribute::LastName,	L"sn" },
		{ UserAttribute::Initial,	L"initials"}
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


auto WinUserService::createUser(UserCreateInfo info) -> std::expected<UserInfo, UserError> {

	ComString relativeName (L"CN=" + toWide(info.firstName) + L" " + toWide(info.lastName) + L",CN=Users");

	auto rawUser = pDirectoryService.createUser(relativeName);

	if (!rawUser) {
		Log::error("{}", rawUser.error());
		return std::unexpected{ UserError::FailedToCreateUserObject };
	}
	
	Log::info("raw user created successfully");

	Microsoft::WRL::ComPtr<IADsUser> user;

	HRESULT hr = rawUser->As(&user);

	if (FAILED(hr)) {
		Log::error("Failed to COM cast IDispatch into IADsUser: {}", getMessage(hr));
		return std::unexpected{ UserError::OtherErrors };
	}

	if (!putUserData(user.Get(), SearchAttribute::UserName, ComVariant{ ComString{ toWide(info.userName) } })) {
		return std::unexpected{ UserError::FailedToSetAttribute };
	}

	if (!putUserData(user.Get(), SearchAttribute::DisplayName, ComVariant{ ComString{ toWide(info.firstName) + L" " + toWide(info.lastName)}})) {
		return std::unexpected{ UserError::FailedToSetAttribute };
	}

	if (!putUserData(user.Get(), SearchAttribute::Mail, ComVariant{ ComString{ toWide(info.mail) } })) {
		return std::unexpected{ UserError::FailedToSetAttribute };
	}

	if (!putUserData(user.Get(), UserAttribute::FirstName, ComVariant{ ComString{ toWide(info.firstName) } })) {
		return std::unexpected {UserError::FailedToSetAttribute};
	}

	if (!putUserData(user.Get(), UserAttribute::LastName, ComVariant{ ComString{ toWide(info.lastName) } })) {
		return std::unexpected{ UserError::FailedToSetAttribute };
	}

	if (!putUserData(user.Get(), UserAttribute::Initial, ComVariant{ ComString{ toWide(info.initials) } })) {
		return std::unexpected{ UserError::FailedToSetAttribute };
	}

	if (!putUserData(user.Get(), SearchAttribute::UserPrincipalName, ComVariant{ ComString{toWide(info.userName) + L"@dafaq.isdis"} })) {
		return std::unexpected{ UserError::FailedToSetAttribute };
	}

	hr = user->SetInfo();

	if (FAILED(hr)) {
		Log::error(
			"Failed to set attributes: {}",
			getMessage(hr)
		);
		return std::unexpected{ UserError::FailedToSetAttribute };
	}

	ComString password{ toWide(info.password) };
	
	hr = user->SetPassword(password.get());

	if (FAILED(hr)) {
		Log::error("Failed to set password: {}", getMessage(hr));

		if (HRESULT_FROM_WIN32(ERROR_PASSWORD_RESTRICTION) == hr) {
			return std::unexpected{ UserError::PasswordPolicy };
		}
		
		if (E_ACCESSDENIED == hr) {
			return std::unexpected{ UserError::PermissionDenied };
		}

		return std::unexpected{ UserError::FailedToSetPassword };
	}

	hr = user->put_AccountDisabled(VARIANT_FALSE);

	if (FAILED(hr)) {
		Log::error("Failed to enable account: {}", getMessage(hr));
		return std::unexpected{ UserError::FailedToEnableUser };
	}

	user->SetInfo();

	if (FAILED(hr)) {
		Log::error(
			"Failed to set attributes: {}",
			getMessage(hr)
		);
		return std::unexpected{ UserError::FailedToSetAttribute };
	}

	return UserInfo{
		.userName = info.userName,
		.displayName = info.firstName + " " + info.lastName,
		.mail = info.mail
	};

}

auto WinUserService::getUser(std::string_view userName) -> std::expected<UserInfo, UserError> {
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
		L"((sAMAccountName=" + toWide(userName) + L"))"
		L")";

	ADS_SEARCH_HANDLE handle = {};

	Log::info("Executing search!");
	HRESULT hr;

	hr = mDirectorySearch->ExecuteSearch(
		filter.data(),
		rawAttributes.data(),
		static_cast<DWORD>(rawAttributes.size()),
		&handle
	);

	if (FAILED(hr)) {
		Log::error("Failed to retrive users: {}", getMessage(hr));
		return std::unexpected{ UserError::UnknownError };
	}

	hr = mDirectorySearch->GetFirstRow(handle);
	if (S_ADS_NOMORE_ROWS == hr) {
		Log::error("Failed to retrive user: {}", getMessage(hr));
		return std::unexpected{ UserError::NotFound };
	}

	if (FAILED(hr)) {
		Log::error("Failed to retrive users: {}", getMessage(hr));
		return std::unexpected{ UserError::UnknownError };
	}
	
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
	return info;
}

auto WinUserService::putUserData(IADsUser* user, UserAttribute attribute, const ComVariant& variant) -> bool {
	HRESULT hr = user->Put(
		ComString{ mUserAttributes.at(attribute) }.get(),
		variant.get()
	);

	if (FAILED(hr)) {
		Log::error("Failed to set attribute via VARIANT for {} : {}", toNarrow(mUserAttributes.at(attribute)), getMessage(hr));
		return false;
	}


	return true;
}

auto WinUserService::putUserData(IADsUser* user, SearchAttribute attribute, const ComVariant& variant) -> bool {
	HRESULT hr = user->Put(
		ComString{ mSearchAttributes.at(attribute) }.get(),
		variant.get()
	);

	if (FAILED(hr)) {
		Log::error("Failed to set attribute via VARIANT for {} : {}", toNarrow(mSearchAttributes.at(attribute)), getMessage(hr));
		return false;
	}
	return true;
}

WinUserService::~WinUserService() {
}
