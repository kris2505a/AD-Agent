#include "public/thread_context.h"
#include <Windows.h>
#include "public/log.h"


ThreadContext::ThreadContext() {
	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	if (FAILED(hr)) {
		Log::error("Failed to init com object for the thread");
		throw std::runtime_error("failed to init objects");
	}
}


ThreadContext::~ThreadContext() {
	CoUninitialize();
}

