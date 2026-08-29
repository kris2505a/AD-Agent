cmake_minimum_required(VERSION 3.20)

project(Core LANGUAGES CXX)


# ---------------------------------
# Source Files
# ---------------------------------
file(GLOB_RECURSE CORE_SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Sources/*.cpp
)


# ---------------------------------
# Library
# ---------------------------------
add_library(Core SHARED
    ${CORE_SOURCES}
)


# ---------------------------------
# DLL Export Definition
# ---------------------------------
target_compile_definitions(Core PRIVATE
    BUILD_DLL
)


# ---------------------------------
# Include Directories
# ---------------------------------
target_include_directories(Core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Includes
)
