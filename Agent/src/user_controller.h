#pragma once
#include <user_service.h>
#include "http.h"

class UserController {
public:
	UserController(IUserService& service, httplib::Server& server);
	~UserController() = default;

private:
	IUserService& rUserService;
	httplib::Server& rServer;
};