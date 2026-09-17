#pragma once
#include <string>

struct UserInfo {
	std::string userName;
	std::string displayName;
	std::string mail;
};

struct UserWriteInfo {
	std::string userName;
	std::string firstName;
	std::string lastName;
	std::string initials;
	std::string mail;
	std::string password;
};