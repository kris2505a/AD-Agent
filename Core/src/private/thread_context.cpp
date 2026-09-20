#include "public/thread_context.h"
#include <Windows.h>
#include "public/log.h"
#include <thread>


ThreadContext::ThreadContext() {
	Log::info("Initializing COM for Thread: {}", std::this_thread::get_id());
	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	if (FAILED(hr)) {
		Log::error("Failed to init com object for the thread");
		throw std::runtime_error("failed to init objects");
	}
}


ThreadContext::~ThreadContext() {
	Log::info("Uninitializing COM for Thread: {}", std::this_thread::get_id());
	CoUninitialize();
}

