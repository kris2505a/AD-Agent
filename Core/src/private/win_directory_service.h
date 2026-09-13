#pragma once
#include "public/directory_service.h"
#include "types.h"
#include <string>
#include <ActiveDS.h>
#include "com_string.h"
#include <expected>

class WinDirectoryService : public IDirectoryService {
public:
	WinDirectoryService(std::string_view);
	~WinDirectoryService() override;


	auto getQueryInterface() -> Microsoft::WRL::ComPtr<IDirectorySearch>;
	auto createUser(const ComString&) -> std::expected<Microsoft::WRL::ComPtr<IDispatch>, std::string>;
	auto getRawService() const -> IADsContainer*;

private:
	Microsoft::WRL::ComPtr<IADsContainer> mADService;
};