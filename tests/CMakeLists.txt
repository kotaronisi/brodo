find_package(GTest CONFIG REQUIRED)

add_executable(brodo_platform_tests
    platform/window_config_tests.cpp
)

target_link_libraries(brodo_platform_tests
    PRIVATE
        brodo_platform
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(brodo_platform_tests)
