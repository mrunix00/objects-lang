cmake_minimum_required(VERSION 3.29)
project(ObjectsScript)

set(CMAKE_CXX_STANDARD 23)
include_directories(include)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

file(GLOB_RECURSE SRC src/*.cpp)
file(GLOB_RECURSE TESTS tests/*.cpp)

add_executable(ObjectsScript main.cpp ${SRC}
        include/runtime.h
        src/runtime.cpp)
add_executable(ObjectsScriptTest ${SRC} ${TESTS}
        tests/runtime_tests.cpp)

target_link_libraries(ObjectsScriptTest GTest::gtest_main GTest::gmock_main)
