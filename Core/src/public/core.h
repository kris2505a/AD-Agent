#pragma once

#ifdef BUILD_DLL
	#define AGENT_API __declspec(dllexport)
#else
	#define AGENT_API __declspec(dllimport)
#endif