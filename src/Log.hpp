#pragma once
#include <format>
#include <fstream>
#include <Windows.h>
#include <comdef.h>

class Log {
public:
    static auto Init(std::string_view path) -> void {
        s_LogFilePath = path;
        s_LogFile.open(s_LogFilePath, std::ios::trunc);
        if (s_LogFile.is_open()) {
            s_IsOpen = true;
        }
    }

    static auto Shutdown() -> void {
        if (s_IsOpen) {
            s_LogFile.close();
        }
    }

    template<typename... Args>
    static auto Warn(std::format_string<Args...> logStr, Args&&... args) -> void {
        if (!s_IsOpen) {
            return;
        }
        auto msg = std::format(logStr, std::forward<Args>(args)...);
        LogMessage(" WARN", msg);
    }

    template<typename... Args>
    static auto Error(std::format_string<Args...> logStr, Args&&... args) -> void {
        if (!s_IsOpen) {
            return;
        }

        auto msg = std::format(logStr, std::forward<Args>(args)...);
        LogMessage("ERROR", msg);
    }

    template<typename... Args>
    static auto Info(std::format_string<Args...> logStr, Args&&... args) -> void {
        if (!s_IsOpen) {
            return;
        }

        auto msg = std::format(logStr, std::forward<Args>(args)...);
        LogMessage(" INFO", msg);
    }

    template<typename... Args>
    static auto Debug(std::format_string<Args...> logStr, Args&&... args) -> void {
        if (!s_IsOpen) {
            return;
        }

        auto msg = std::format(logStr, std::forward<Args>(args)...);
        LogMessage("DEBUG", msg);
    }

    static auto HRtoStr(HRESULT hr) -> std::string {
        _com_error error(hr);
        
        std::wstring msg = error.ErrorMessage();
        return std::format("HRESULT: 0x{:08x} | Msg: {}", static_cast<uint32_t>(hr), std::string(msg.begin(), msg.end()));
    }
private:
    static auto LogMessage(std::string_view type, std::string_view msg) -> void {
        s_LogFile << "[" << type << "]: " << msg << "\n";
        s_LogFile.flush();
    }

private:
    inline static std::string s_LogFilePath;
    inline static std::ofstream s_LogFile;
    inline static bool s_IsOpen = false;
};
