#include <directory_service.h>
#include <user_service.h>
#include <log.h>
#include <thread_context.h>
#include <thread_context.h>

#include <iostream>

auto main(int argc, char** argv) -> int {

	ThreadContext::init();

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
		.password = "SmoothValukka@67"
	});

	if (!user) {
		Log::error("Failed to create user!");
	}
	
	Log::info("UserName: {}", user->userName);
	Log::info("DisplayName: {}", user->displayName);
	Log::info("Mail: {}", user->mail);
	

	ThreadContext::shutDown();
}