#include <directory_service.h>
#include <user_service.h>
#include <log.h>
#include <thread_context.h>

#include <iostream>

auto main(int argc, char** argv) -> int {

	ThreadContext context;

	/*
	if (argc < 2) {
		Log::error("no args found!");
		return -1;
	}
	std::string_view connStr = argv[1];
	
	auto adService = IDirectoryService::create(connStr);
	*/
	auto adService = IDirectoryService::create("LDAP://DC=dafaq,DC=isdis");
	auto userService = IUserService::create(*adService);

	UserWriteInfo valukkaWriteInfo{
		.userName = "valukkathala",
		.firstName = "Valukka Manda",
		.lastName = "Thala",
		.initials = "X",
		.mail = "valukkamanda69@dafaq.isdis",
		.password = "Krishna@2505@2505"
	};

	auto user = userService->createUser(valukkaWriteInfo);

	if (user) {

		Log::debug("UserName: {}", user->userName);
		Log::debug("DisplayName: {}", user->displayName);
		Log::debug("Mail: {}", user->mail);
	}

	else {
		Log::error("Failed to create user!");
	}


	Log::debug("----------------------------------------------------");
	Log::debug("Finding user: valukkathala");
	auto searchUser = userService->getUser("valukkathala");

	if (searchUser) {
		Log::debug("UserName: {}", searchUser->userName);
		Log::debug("DisplayName: {}", searchUser->displayName);
		Log::debug("Mail: {}", searchUser->mail);
	}
	
	else {
		Log::error("Failed to create user!");
	}

	userService->deleteUser("valukkathala");

}