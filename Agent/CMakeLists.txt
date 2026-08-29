cmake_minimum_required(VERSION 3.20)

project(Agent LANGUAGES CXX)


# ---------------------------------
# Source Files
# ---------------------------------
file(GLOB_RECURSE AGENT_SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Sources/*.cpp
)


# ---------------------------------
# Executable
# ---------------------------------
add_executable(Agent
    ${AGENT_SOURCES}
)


# ---------------------------------
# Link Core
# ---------------------------------
target_link_libraries(Agent PRIVATE
    Core
)
