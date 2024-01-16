# Copyright 2024 Benjamin Lutz.
#
# This file is part of QRGen. QRGen is free software: you can redistribute it
# and/or modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation, either version 3 of the License,
# or (at your option) any later version.

cmake_minimum_required(VERSION 3.12)

project(libQRGen LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(libQRGen SHARED)
target_sources(libQRGen
    PRIVATE
    include/qrgen.h
    src/data.cpp
    src/data.h
    src/ecccalculator.cpp
    src/ecccalculator.h
    src/gf.h
    src/polynomial.cpp
    src/polynomial.h
    src/qr.cpp
    src/qr.h
    src/qrgen.cpp
    src/symbol.cpp
    src/symbol.h
)

target_include_directories(libQRGen PUBLIC
    include
)
target_link_libraries(libQRGen
    icuuc
)
target_compile_definitions(libQRGen PRIVATE LIBQRGEN_COMPILE)


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
    test/test_qr.cpp
    test/test_symbol.cpp    
)

target_include_directories(libQRGenTest PRIVATE
    include
)
target_link_libraries(libQRGenTest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(libQRGenTest)
