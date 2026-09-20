#include <directory_service.h>
#include <user_service.h>
#include <log.h>
#include <thread_context.h>

#include "http.h"
#include <json.h>

#include <iostream>
#include <sstream>
#include <thread>


auto getUserString(std::vector<UserInfo>& userInfo) -> std::string {
	std::string result = "";

	std::stringstream ss;
	ss << "[\n";

	for (auto& user : userInfo) {
		ss << "\t{";
		ss << "\t\tuserName : " + user.userName + ", \n";
		ss << "\t\tdisplayName : " + user.displayName + ", \n";
		ss << "\t\tmail : " + user.mail + "\n";
		ss << "\t}, ";
	}

	ss << "]";
	

	return ss.str();
}


auto main(int argc, char** argv) -> int {

	bool running = true;

	ThreadContext context;

	std::ifstream inFile("settings.json");

	if (!inFile.is_open()) {
		Log::error("Failed to find settings.json!");
		return -1;
	}

	std::string settingsContents;
	std::getline(inFile, settingsContents, '\0');

	nlohmann::json settings = nlohmann::json::parse(settingsContents);

	std::string connStr = settings.at("ADConnURL");
	std::string url = settings.at("URL");
	int port = settings.at("port");


	auto adService = IDirectoryService::create(settings.at("ADConnURL"));
	auto userService = IUserService::create(*adService);


	httplib::Server server;

	server.Get("/api/v1/users", [&](const httplib::Request& request, httplib::Response& response) {

		ThreadContext context;

		Log::info("Getting Users on {}:{}", url, port);
		
		auto users = userService->getUsers();

		nlohmann::json res = users;

		response.status = 200;
		response.set_content(
			res.dump(), "application/json"
		);
	});

	server.Get("/api/v1/test", [&](const httplib::Request& request, httplib::Response& response) {
		
		Log::debug("READING Async");
		
		ThreadContext context;

		Log::info("Getting Users on {}:{}", url, port);

		auto users = userService->getUsers();

		nlohmann::json res = users;

		std::this_thread::sleep_for(std::chrono::seconds(5));

		Log::debug("READING Finished Async");

		response.status = 200;
		response.set_content(
			res.dump(), "application/json"
		);

	});

	server.Get("/api/v1/user/:userName", [&](const httplib::Request& request, httplib::Response& response) {

		ThreadContext context;

		Log::info("Getting user on {}:{}", url, port);
		std::string userName = request.path_params.at("userName");
		auto user = userService->getUser(userName);

		if (user) {
			nlohmann::json j = *user;
			response.status = 200;
			response.set_content(j.dump(), "application/json");
			return;
		}

		nlohmann::json err = {
			{"error", "_YET_TO_FILL_"}
		};

		switch (user.error()) {
		case IUserService::Error::OtherErrors:
		case IUserService::Error::UnknownError: {
			response.status = 500;
			err.at("error") = "Internal Server side error!";
			break;
		}

		case IUserService::Error::NotFound: {
			response.status = 404;
			err.at("error") = "User not found!";
			break;
		}

		default: {
			response.status = 500;
			err.at("error") = "Unknown error!";
			break;
		}
		}

		response.set_content(err.dump(), "application/json");
	});

	server.Post("/api/v1/users", [&](const httplib::Request& request, httplib::Response& response) {

		ThreadContext context;

		Log::info("Posting user on {}:{}", url, port);

		auto j = nlohmann::json::parse(request.body);

		auto writeInfo = j.get<UserWriteInfo>();
		auto result = userService->createUser(writeInfo);

		if (result) {
			nlohmann::json res = *result;
			response.status = 201;
			response.set_content(res.dump(), "application/json");
			return;
		}

		nlohmann::json err = {
			{"error", "_YET_TO_FILL_"}
		};

		switch (result.error()) {
		case IUserService::Error::AlreadyExists: {
			response.status = 409;
			err.at("error") = "User already exists!";
			break;
		}

		case IUserService::Error::PermissionDenied: {
			response.status = 403;
			err.at("error") = "Access denied";
			break;
		}

		case IUserService::Error::PasswordPolicy: {
			response.status = 400;
			err.at("error") = "Password policy. Please enter other password";
			break;
		}

		case IUserService::Error::FailedToSetPassword: {
			response.status = 500;
			err.at("error") = "Failed to set password!";
			break;
		}

		case IUserService::Error::OtherErrors:
		case IUserService::Error::FailedToCreateUserObject:
		case IUserService::Error::UnknownError: {
			response.status = 500;
			err.at("error") = "Internal server error";
			break;

		}

		default: {
			response.status = 500;
			err.at("error") = "Unknown error!";
			break;
		}
		}
		
		response.set_content(err.dump(), "application/json");
	});

	server.Put("/api/v1/user/:userName", [&](const httplib::Request& request, httplib::Response& response) {
		ThreadContext context;

		Log::info("Putting user on {}:{}", url, port);
		std::string userName = request.path_params.at("userName");

		auto j = nlohmann::json::parse(request.body);

		auto writeInfo = j.get<UserWriteInfo>();

		writeInfo.userName = userName;
		auto result = userService->modifyUser(writeInfo);

		if (result) {
			nlohmann::json res = *result;
			response.status = 201;
			response.set_content(res.dump(), "application/json");
			return;
		}

		nlohmann::json err = {
			{"error", "_YET_TO_FILL_"}
		};

		//TODO: need to handle "Not found" case;
		switch (result.error()) {
		case IUserService::Error::FailedToSetAttribute:
		{
			response.status = 500;
			err.at("error") = "Failed to set attributes!";
			break;
		}

		case IUserService::Error::FailedToRetrieveUserObject: {
			response.status = 404;
			err.at("error") = "Failed to set attributes!";
			break;
		}

		default: {
			response.status = 500;
			err.at("error") = "Unknown error!";
			break;
		}
		}

		response.set_content(err.dump(), "application/json");

	});

	server.Delete("/api/v1/user/:userName", [&](const httplib::Request& request, httplib::Response& response) {
		ThreadContext context;

		Log::info("Putting user on {}:{}", url, port);
		std::string userName = request.path_params.at("userName"); 

		auto result = userService->deleteUser(userName);

		if (result) {
			response.status = 204;
			return;
		}


		nlohmann::json err = {
			{"error", "_YET_TO_FILL_"}
		};

		switch (result.error()) {
		case IUserService::Error::UnknownError: {
			response.status = 500;
			err.at("error") = "Not found or Internal Error!";
			break;
		}

		default: {
			response.status = 500;
			err.at("error") = "Unknown error!";
			break;
		}
		}
		response.set_content(err.dump(), "application/json");
	});

	std::jthread serverThread([&]() { server.listen(url, port); });

	char input;

	while (running && std::cin >> input) {
		if ('q' == input || 'Q' == input) {
			running = false;
		}
	}

	server.stop();
}