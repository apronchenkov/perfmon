cmake_minimum_required(VERSION 2.8.4)
project(perfmon)

find_package(Boost COMPONENTS unit_test_framework)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wextra -pedantic")

add_library(perfmon SHARED
    cpu_frequency.cpp
    perfmon.cpp
    include/perfmon.h
    include/perfmon/cpu_frequency.h
    include/perfmon/perfmon_inl.h
    include/perfmon/ticks.h)

add_executable(perfmon_test
    test.cpp
    test_scope.cpp
    test_statement.cpp)

target_link_libraries(perfmon_test
    perfmon
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

enable_testing()
add_test(perfmon_test perfmon_test)
