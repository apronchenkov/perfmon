cmake_minimum_required(VERSION 3.1)
include(WriteCompilerDetectionHeader)
project(perfmon)

find_package(Boost REQUIRED COMPONENTS system thread unit_test_framework)
find_package(Threads)

write_compiler_detection_header(
    FILE include/perfmon/config.h
    PREFIX PERFMON_CONFIG_
    COMPILERS AppleClang Clang GNU MSVC
    FEATURES cxx_thread_local)

include_directories(BEFORE SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/include)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")


# perfmon

add_library(perfmon SHARED
    counters.cpp
    counters.h
    cpu_frequency.cpp
    thread_specific_counters.cpp
    include/perfmon.h
    include/perfmon/cpu_frequency.h
    include/perfmon/perfmon_inl.h
    include/perfmon/thread_specific_counters.h
    include/perfmon/ticks.h)

target_link_libraries(perfmon
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT})

set_property(TARGET perfmon PROPERTY CXX_STANDARD 11)


# perfmon_test

add_executable(perfmon_test
    test.cpp
    test_scope.cpp
    test_statement.cpp
    test_performance.cpp)

target_link_libraries(perfmon_test
    perfmon
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT})

set_property(TARGET perfmon_test PROPERTY CXX_STANDARD 11)

enable_testing()
add_test(perfmon_test perfmon_test)


# sample
add_executable(sample sample.cpp)

target_link_libraries(sample perfmon ${CMAKE_THREAD_LIBS_INIT})

set_property(TARGET sample PROPERTY CXX_STANDARD 11)
