# GoogleTest and CMake Patterns

## Existing CMake project

When CMake already exists, prefer the smallest additive change:

- enable testing if not already enabled
- resolve GoogleTest with `find_package(GTest CONFIG REQUIRED)`
- add a dedicated test target for the affected module
- link the module target and `GTest::gtest_main`
- register tests with `gtest_discover_tests()`

## No test harness yet

Bootstrap only what is needed to compile and run tests:

```cmake
enable_testing()
add_subdirectory(tests)
```

```cmake
find_package(GTest CONFIG REQUIRED)

add_executable(brodo_module_tests
    sample_test.cpp
)

target_link_libraries(brodo_module_tests
    PRIVATE
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(brodo_module_tests)
```

Replace `sample_test.cpp` and the linked module target with the real module under test. Keep the bootstrap minimal and avoid adding CI or unrelated targets here.
