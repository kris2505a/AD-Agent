#include "User.hpp"
#include <chrono>


User::User(const UserInfo& createInfo) 
    : m_UserName(createInfo.UserName), m_DisplayName(createInfo.DisplayName), m_Email(createInfo.Email) {
    
    auto now = std::chrono::system_clock::now();
    
    m_CreatedAt = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count(); 
    m_LastActiveAt = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
}

auto User::GetUserName() const -> const std::wstring& {
    return m_UserName;
}

auto User::GetDisplayName() const -> const std::wstring& {
    return m_DisplayName;
}

auto User::GetEmail() const -> const std::wstring& {
    return m_Email;
}

auto User::CreatedAt() const -> uint64_t {
    return m_CreatedAt;
}

auto User::LastActiveAt() const -> uint64_t {
    return m_LastActiveAt;
}

auto User::GetGUID() const -> const std::wstring& {
    return m_Guid;
}

std::wostream& operator << (std::wostream& ostr, const User& u)  {
        ostr    << L"Guid: " << u.GetGUID() << L"\n"
                << L"UserName: " << u.GetUserName() << L"\n"
                << L"DisplayName: " << u.GetDisplayName() << L"\n"
                << L"Mail: " << u.GetEmail() << L"\n\n\n";
        return ostr;
    }
