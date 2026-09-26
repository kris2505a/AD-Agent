#pragma once
#include <directory_service.h>
#include <user_service.h>
#include <log.h>
#include <thread_context.h>

#include "http.h"
#include <json.h>
#include "user_controller.h"

class Agent {
public:
	Agent();
	~Agent();
	Agent(const Agent&) = delete;
	Agent& operator=(const Agent&) = delete;


	static auto start() -> void;
	static auto stop() -> void;

private:
	auto startImpl() -> void;
	auto endImpl() -> void;

private:
	ThreadContext						mThreadContext;
	std::unique_ptr <IDirectoryService> mDirectoryService;
	std::unique_ptr <IUserService>		mUserService;
	
	std::unique_ptr <UserController>	mUserController;

	httplib::Server						mServer;
	std::jthread						mServerThread;

	static Agent						sInstance;
};