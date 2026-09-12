#include "win_directory_service.h"
#include <stdexcept>
#include "public/log.h"

#include <iostream>

auto IDirectoryService::create(std::string_view connectionString) -> std::unique_ptr<IDirectoryService> {
	return std::make_unique<WinDirectoryService>(connectionString);
}

WinDirectoryService::WinDirectoryService(std::string_view connectionString) {
	auto connectionStringW = toWide(connectionString);
	HRESULT hr;
	
	std::wcout << "connecting to AD: " << connectionStringW << std::endl;

	hr = ADsOpenObject(
		connectionStringW.c_str(),
		nullptr,
		nullptr,
		ADS_SECURE_AUTHENTICATION,
		IID_IADsContainer,
		reinterpret_cast<void**>(mADService.ReleaseAndGetAddressOf())
	);

	if (FAILED(hr)) {
		Log::error("Failed to connect to ad: {}", getMessage(hr));
		CoUninitialize();
		throw std::runtime_error("Failed to connect to ad. check logs for more info");
	}
	Log::info("Connecte to ad: {}", connectionString);
}

WinDirectoryService::~WinDirectoryService() {
}


auto WinDirectoryService::getQueryInterface() -> Microsoft::WRL::ComPtr<IDirectorySearch> {
	Microsoft::WRL::ComPtr<IDirectorySearch> directorySearch;
	auto hr = mADService->QueryInterface(IID_PPV_ARGS(directorySearch.GetAddressOf()));

	if (FAILED(hr)) {
		Log::error("Failed to query IDirectorySearch: {}", getMessage(hr));
		throw std::runtime_error("Failed to retrieve resource.");
	}

	return directorySearch;
}
