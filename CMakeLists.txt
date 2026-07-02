cmake_minimum_required(VERSION 3.25)

project(brodo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT WIN32)
    message(FATAL_ERROR "Brodo platform window support is currently Windows-only.")
endif()

add_library(brodo_platform
    src/platform/window_win32.cpp
)

target_include_directories(brodo_platform
    PUBLIC
        include
)

target_compile_definitions(brodo_platform
    PUBLIC
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        UNICODE
        _UNICODE
)

add_executable(brodo_window_sample
    samples/window_sample.cpp
)

target_link_libraries(brodo_window_sample
    PRIVATE
        brodo_platform
)

include(CTest)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
