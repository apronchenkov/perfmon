cmake_minimum_required(VERSION 3.1)
include(WriteCompilerDetectionHeader)
project(perfmon)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)
find_package(Threads)

include_directories(
    BEFORE SYSTEM
    ${CMAKE_CURRENT_SOURCE_DIR}/public)

# perfmon

add_library(perfmon
    counters.cpp
    counters.h
    cpu_frequency.cpp
    thread_specific_counters.cpp
    public/perfmon.h
    public/perfmon/cpu_frequency.h
    public/perfmon/perfmon_inl.h
    public/perfmon/thread_specific_counters.h
    public/perfmon/ticks.h)

set_property(TARGET perfmon PROPERTY CXX_STANDARD 11)

# testing

add_executable(perfmon_test
    test.cpp
    test_scope.cpp
    test_statement.cpp
    test_performance.cpp)

set_property(TARGET perfmon_test PROPERTY CXX_STANDARD 11)

target_link_libraries(perfmon_test
    perfmon
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT})

enable_testing()
add_test(perfmon_test perfmon_test)

# sample

add_executable(sample sample.cpp)

set_property(TARGET sample PROPERTY CXX_STANDARD 11)

target_link_libraries(sample perfmon ${CMAKE_THREAD_LIBS_INIT})

# install

install(
    TARGETS perfmon
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)

install(
    DIRECTORY public DESTINATION include)

