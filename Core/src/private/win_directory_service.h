#pragma once
#include "public/directory_service.h"
#include "types.h"
#include <string>
#include <ActiveDS.h>

class WinDirectoryService : public IDirectoryService {
public:
	WinDirectoryService(std::string_view connectionString);
	~WinDirectoryService() override;


	auto getQueryInterface() -> Microsoft::WRL::ComPtr<IDirectorySearch>;

private:
	Microsoft::WRL::ComPtr<IADsContainer> mADService;
};