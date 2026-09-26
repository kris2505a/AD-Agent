#include "agent.h"
#include "config.h"
#include <log.h>

Agent Agent::sInstance;

auto Agent::startImpl() -> void {
	mServerThread = std::jthread([this]() {
		mServer.listen(Config::url, Config::port);
	});
}

auto Agent::endImpl() -> void {
	mServer.stop();
	
	if (mServerThread.joinable()) {
		mServerThread.join();
	}
}


Agent::Agent() {
	Config::load();

	auto logPath = Config::getExecutableDirectory() / "Log.log";

	Log::start(logPath);

	mDirectoryService = IDirectoryService::create(Config::connectionString);
	mUserService = IUserService::create(*mDirectoryService.get());

	mUserController = std::make_unique<UserController>(*mUserService.get(), mServer);
}

Agent::~Agent() {
	Log::end();
}

auto Agent::start() -> void {
	sInstance.startImpl();
}

auto Agent::stop() -> void {
	sInstance.endImpl();
}