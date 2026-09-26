#include <directory_service.h>
#include <user_service.h>
#include <log.h>
#include <thread_context.h>

#include "http.h"
#include <json.h>

#include <iostream>
#include <sstream>
#include <thread>




auto main(int argc, char** argv) -> int {

	bool running = true;
	char input;

	while (running && std::cin >> input) {
		if ('q' == input || 'Q' == input) {
			running = false;
		}
	}

}