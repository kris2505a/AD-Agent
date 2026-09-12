#include "public/thread_context.h"
#include <Windows.h>
#include "public/log.h"


auto ThreadContext::init() -> void {
	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	if (FAILED(hr)) {
		Log::error("Failed to init com object for the thread");
		throw std::runtime_error("failed to init objects");
	}
}


auto ThreadContext::shutDown() -> void {
	CoUninitialize();
}

