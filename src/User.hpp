#pragma once

#include <string>
#include <cstdint>

struct UserInfo {
    std::wstring UserName;
    std::wstring DisplayName;
    std::wstring Email;
    std::wstring Guid;
};

class User {
    friend std::wostream& operator <<(std::wostream& ostr, const User& u);

public:
    User(const UserInfo& createInfo);
    ~User() = default; 
    
    auto GetUserName() const -> const std::wstring&;
    auto GetDisplayName() const -> const std::wstring&;
    auto GetEmail() const -> const std::wstring&;
    auto CreatedAt() const -> uint64_t;
    auto LastActiveAt() const -> uint64_t;
    auto GetGUID() const -> const std::wstring&;

private:
    std::wstring m_UserName;
    std::wstring m_DisplayName;
    std::wstring m_Email;
    std::wstring m_Guid;
    uint64_t m_CreatedAt;
    uint64_t m_LastActiveAt;
    bool m_Active;
};
