#include "perfmon.h"
#include <atomic>
#include <cstdio>
#include <iostream>
#include <thread>
#include <vector>

int n = 2;
int N = 4 * 1024;

namespace {

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

std::atomic<int> global_gcd_1_result(0);
std::atomic<int> global_gcd_n_result(0);

void gcd_1_thread() {
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

void gcd_n_thread() {
  PERFMON_SCOPE("gcd_n_thread");
  int result = 0;
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      PERFMON_SCOPE("congestion_point");
      result += gcd(i, j);
    }
  }
  global_gcd_n_result += result;
}

}  // namespace

void Usage() {
  std::cerr << "usage: N n\n\n";
  std::exit(-1);
}

int main(int argc, char **argv) {
  if (argc != 3) {
    Usage();
  }
  if (1 != sscanf(argv[1], "%d", &N) || 1 != sscanf(argv[2], "%d", &n)) {
    Usage();
  }

  // thread 1
  for (int i = 0; i < n; ++i) {
    std::thread(gcd_1_thread).join();
  }

  // thread n
  std::vector<std::thread> threads;
  for (int i = 0; i < n; ++i) {
    threads.emplace_back(gcd_n_thread);
  }
  for (auto &thread : threads) {
    thread.join();
  }

  const auto counters = PERFMON_COUNTERS();
  std::cout << "1 thread: " << counters["gcd_1_thread"].Calls() << ' '
            << counters["gcd_1_thread"].Ticks() << ' '
            << static_cast<double>(counters["gcd_1_thread"].Ticks()) /
                   counters["gcd_1_thread"].Calls()
            << '\n';

  std::cout << n << " thread: " << counters["gcd_n_thread"].Calls() << ' '
            << counters["gcd_n_thread"].Ticks() << ' '
            << static_cast<double>(counters["gcd_n_thread"].Ticks()) /
                   counters["gcd_n_thread"].Calls()
            << '\n';

  return 0;
}
