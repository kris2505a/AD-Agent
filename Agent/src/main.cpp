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
	
	std::vector<UserInfo> users;

	try {
		users = userService->getUsers();
		for (auto& user : users) {
			Log::debug("UserName: {}", user.userName);
			Log::debug("\tDisplayName: {}", user.displayName);
			Log::debug("\tMail: {}\n", user.mail);

		}
	}

	catch (std::exception& e) {
		Log::error("{}", e.what());
	}

	ThreadContext::shutDown();

}