#include "public/perfmon.h"
#include <atomic>
#include <boost/test/unit_test.hpp>
#include <thread>

namespace {

std::atomic<int> global_gcd_1_result(0);
std::atomic<int> global_gcd_2_result(0);
std::atomic<int> global_gcd_4_result(0);

int N = 2 * 1024;

int gcd(int a, int b) {
  for (;;) {
    if (a == 0) {
      return b;
    }
    b %= a;
    if (b == 0) {
      return a;
    }
    a %= b;
  }
}

void gcd_1_thread() {
  for (int i = 0; i < 4; ++i) {
    PERFMON_SCOPE("gcd_1_thread");
    int result = 0;
    for (int i = 0; i < N; ++i) {
      for (int j = 0; j < N; ++j) {
        PERFMON_SCOPE("congestion_point");
        result += gcd(i, j);
      }
    }
    global_gcd_1_result += result;
  }
}

void gcd_2_thread() {
  for (int i = 0; i < 2; ++i) {
    PERFMON_SCOPE("gcd_2_thread");
    int result = 0;
    for (int i = 0; i < N; ++i) {
      for (int j = 0; j < N; ++j) {
        PERFMON_SCOPE("congestion_point");
        result += gcd(i, j);
      }
    }
    global_gcd_2_result += result;
  }
}

void gcd_4_thread() {
  PERFMON_SCOPE("gcd_4_thread");
  int result = 0;
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      PERFMON_SCOPE("congestion_point");
      result += gcd(i, j);
    }
  }
  global_gcd_4_result += result;
}

}  // namespace

BOOST_AUTO_TEST_CASE(test_performance) {
  {  // 1 thread
    std::thread t1(gcd_1_thread);
    t1.join();
  }

  {  // 2 threads
    std::thread t1(gcd_2_thread);
    std::thread t2(gcd_2_thread);
    t1.join();
    t2.join();
  }
  {  // 4 threads
    std::thread t1(gcd_4_thread);
    std::thread t2(gcd_4_thread);
    std::thread t3(gcd_4_thread);
    std::thread t4(gcd_4_thread);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
  }

  BOOST_REQUIRE_EQUAL(global_gcd_1_result, global_gcd_2_result);
  BOOST_REQUIRE_EQUAL(global_gcd_1_result, global_gcd_4_result);

  const auto counters = PERFMON_COUNTERS();
  BOOST_REQUIRE_EQUAL(counters["gcd_1_thread"].Calls(),
                      counters["gcd_2_thread"].Calls());
  BOOST_REQUIRE_EQUAL(counters["gcd_1_thread"].Calls(),
                      counters["gcd_4_thread"].Calls());

  BOOST_REQUIRE(counters["gcd_1_thread"].Ticks() >
                0.89 * counters["gcd_2_thread"].Ticks());
  BOOST_REQUIRE(counters["gcd_2_thread"].Ticks() >
                0.89 * counters["gcd_4_thread"].Ticks());
}
