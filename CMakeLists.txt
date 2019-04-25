cmake_minimum_required(VERSION 3.1)
project(perfmon LANGUAGES CXX)

enable_testing()

find_package(GTest)
find_package(Threads)

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

target_compile_features(perfmon PUBLIC cxx_std_11)

# testing
if(GTEST_FOUND)
  add_executable(perfmon_test
                 test_scope.cpp
                 test_statement.cpp
                 test_performance.cpp
                 test_regression.cpp)

  target_link_libraries(perfmon_test perfmon GTest::GTest GTest::Main)

  gtest_discover_tests(perfmon_test perfmon_test)
endif(GTEST_FOUND)

# sample

add_executable(sample sample.cpp)

target_link_libraries(sample perfmon Threads::Threads)

# install

install(TARGETS perfmon ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)

install(DIRECTORY public/ DESTINATION include)
