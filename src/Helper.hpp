#pragma once
#include <Windows.h>
#include <ActiveDS.h>
#include <AdsHlp.h>
#include <string>
#include <wrl/client.h>
#include <stdexcept>

inline auto ThrowOnFail(HRESULT hr, const std::string& str) -> void {
    if (FAILED(hr)) {
        throw std::runtime_error(str);
    }
}
