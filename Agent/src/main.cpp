#include <directory_service.h>
#include <user_service.h>
#include <log.h>
#include <thread_context.h>

#include <iostream>

auto main(int argc, char** argv) -> int {

	ThreadContext context;

	if (argc < 2) {
		Log::error("no args found!");
		return -1;
	}
	std::string_view connStr = argv[1];
	auto adService = IDirectoryService::create(connStr);
	auto userService = IUserService::create(*adService);

	auto user = userService->createUser({
		.userName = "valukkathala",
		.firstName = "Valukka",
		.lastName = "Thala",
		.initials = "X",
		.mail = "valukkathala69@dafaq.isdis",
		.password = "Krishna@2505@2505"
	});

	if (!user) {
		Log::error("Failed to create user!");
	}
	
	Log::debug("UserName: {}", user->userName);
	Log::debug("DisplayName: {}", user->displayName);
	Log::debug("Mail: {}", user->mail);
	

	Log::debug("----------------------------------------------------");
	Log::debug("Finding user: valukkathala");
	userService->getUser("valukkathala");

}