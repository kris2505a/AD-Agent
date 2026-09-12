#pragma once


#include <cstdint>
#include <memory>
#include <string>

#include <comdef.h>


#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wrl/client.h>


using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using f32 = float;
using f64 = double;


inline auto toNarrow(std::wstring_view str) -> std::string {
    if (str.empty()) {
        return {};
    }

    int size = WideCharToMultiByte(
        CP_UTF8,
        0,
        str.data(),
        static_cast<int>(str.size()),
        nullptr,
        0,
        nullptr,
        0
    );

    std::string result(size, '\0');

    WideCharToMultiByte(
        CP_UTF8,
        0,
        str.data(),
        static_cast<int>(str.size()),
        result.data(),
        size,
        nullptr,
        nullptr
    );

    return result;

}

inline auto toWide(std::string_view str) -> std::wstring {
    if (str.empty()) {
        return {};
    }

    int size = MultiByteToWideChar(
        CP_UTF8,
        0,
        str.data(),
        static_cast<int>(str.size()),
        nullptr,
        0
    );

    std::wstring result(size, L'\0');
    MultiByteToWideChar(
        CP_UTF8,
        0,
        str.data(),
        static_cast<int>(str.size()),
        result.data(),
        size
    );

    return result;
}

inline auto getMessage(HRESULT hr) -> std::string {
    _com_error err(hr);

    return toNarrow(err.ErrorMessage());
}