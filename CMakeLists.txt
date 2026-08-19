cmake_minimum_required(VERSION 3.21)

project(ServerAgent LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_executable(ServerAgent
    src/Log.hpp
    src/Helper.hpp
    src/DirectoryService.hpp
    src/DirectoryService.cpp
    src/User.hpp
    src/User.cpp
    src/main.cpp

)

target_compile_features(ServerAgent PRIVATE cxx_std_23)

target_compile_definitions(ServerAgent PRIVATE
    UNICODE
    _UNICODE
    NOMINMAX
)

target_link_libraries(ServerAgent PRIVATE
    activeds
    adsiid
    ole32
    oleaut32
)

target_include_directories(ServerAgent PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

if(CMAKE_EXPORT_COMPILE_COMMANDS)
    add_custom_target(copy_compile_commands ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/compile_commands.json"
            "${CMAKE_SOURCE_DIR}/compile_commands.json"
    )
endif()
