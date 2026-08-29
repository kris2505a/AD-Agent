cmake_minimum_required(VERSION 3.20)

project(ADAgent LANGUAGES C CXX)

# ---------------------------------
# C++ Standard
# ---------------------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# ---------------------------------
# Compile Commands
# ---------------------------------
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Copy compile_commands.json to root
add_custom_target(copy_compile_commands ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
)


# ---------------------------------
# Output Directories
# ---------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_SOURCE_DIR}/Binaries/$<CONFIG>
)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_SOURCE_DIR}/Binaries/$<CONFIG>
)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${CMAKE_SOURCE_DIR}/Binaries/$<CONFIG>
)


# ---------------------------------
# Projects
# ---------------------------------
add_subdirectory(Core)
add_subdirectory(Agent)
