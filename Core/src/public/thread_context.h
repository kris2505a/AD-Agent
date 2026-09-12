#pragma once
#include "core.h"

class ThreadContext {
public:
	AGENT_API inline static auto init() -> void;
	AGENT_API inline static auto shutDown() -> void;
};