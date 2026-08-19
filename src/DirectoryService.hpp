#pragma once

#include "Helper.hpp"
#include "User.hpp"
#include <vector> 

class DirectoryService {
public: 
    DirectoryService();
    static auto Init(const std::wstring& connectionString) -> void;
    static auto Shutdown() -> void;
    static auto QueryUsers() -> std::vector<User>;
    static auto CreateUser(const std::wstring& sAMAccountName) -> void;

private:
    auto InitImpl(const std::wstring& connectionString) -> void;
    auto ShutdownImpl() -> void;
    auto QueryUserImpl() -> std::vector<User>;
    auto CreateUserImpl(const std::wstring& sAMAccountName) -> void;

private:
    Microsoft::WRL::ComPtr<IADsContainer> m_ADContainer;
    
    inline static DirectoryService* s_Instance = nullptr;
};
