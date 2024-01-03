cmake_minimum_required(VERSION 3.12)

project(libQRGen LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(libQRGen SHARED)
target_sources(libQRGen
    PRIVATE
    src/data.cpp
    src/data.h
    src/ecccalculator.cpp
    src/ecccalculator.h
    src/gf.h
    src/polynomial.cpp
    src/polynomial.h
)

set_target_properties(libQRGen PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(libQRGen icuuc)


##### Unit Testing #####

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

get_target_property(libQRGen_SOURCES libQRGen SOURCES)
add_executable(libQRGenTest
    ${libQRGen_SOURCES}
    test/test_data.cpp
    test/test_ecccalculator.cpp
    test/test_gf.cpp
    test/test_polynomial.cpp
)

target_link_libraries(
    libQRGenTest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(libQRGenTest)
