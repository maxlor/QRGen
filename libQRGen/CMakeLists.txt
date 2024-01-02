cmake_minimum_required(VERSION 3.12)

project(libQRGen LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(libQRGen SHARED
    src/gf.h)

set_target_properties(libQRGen PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(libQRGen icuuc)


# Test Related

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(libQRGenTest
    test/test_gf.cpp
)

target_link_libraries(
    libQRGenTest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(libQRGenTest)
