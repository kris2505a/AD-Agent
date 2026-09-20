#pragma once
#include <string>
#include "json.h"

struct UserInfo {
	std::string userName;
	std::string displayName;
	std::string mail;
};

inline auto to_json(nlohmann::json& j, const UserInfo& info) -> void {
	j = {
		{ "userName", info.userName },
		{ "displayName", info.displayName },
		{ "mail", info.mail }
	};
}

inline auto from_json(const nlohmann::json& j, UserInfo& info) -> void {
	j.at("userName").get_to(info.userName);
	j.at("displayName").get_to(info.displayName);
	j.at("mail").get_to(info.mail);
}

struct UserWriteInfo {
	std::string userName;
	std::string firstName;
	std::string lastName;
	std::string initials;
	std::string mail;
	std::string password;
};

inline auto to_json(nlohmann::json& j, const UserWriteInfo& info) -> void {
	j = {
		{ "userName", info.userName },
		{ "firstName", info.firstName },
		{ "lastName", info.lastName },
		{ "initials", info.initials },
		{ "mail", info.mail },
		{ "password", info.password }
	};
}

inline auto from_json(const nlohmann::json& j, UserWriteInfo& info) -> void {
	j.at("userName").get_to(info.userName);
	j.at("firstName").get_to(info.firstName);
	j.at("lastName").get_to(info.lastName);
	j.at("initials").get_to(info.initials);
	j.at("mail").get_to(info.mail);
	j.at("password").get_to(info.password);
}
