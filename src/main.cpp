#include <iostream>
#include <fstream>
#include "DirectoryService.hpp"
#include "Log.hpp"

int main() {
    Log::Init("Log.log");
    std::wifstream file("conn.txt");
    std::wstring connStr;
    if (!std::getline(file, connStr)) {
        std::cout << "Failed to read line!" << std::endl;
    }

    auto service = new DirectoryService();
    
    try {    
        DirectoryService::Init(connStr);

        DirectoryService::CreateUser(L"krisptest02");

        auto users = DirectoryService::QueryUsers();

        for (int i = 0; i < users.size(); i++) {
            std::wcout << users.at(i);
        }

    }

    catch(std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    DirectoryService::Shutdown(); 
    delete service;
    Log::Shutdown();
}
