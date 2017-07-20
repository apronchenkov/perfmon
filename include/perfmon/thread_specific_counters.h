#pragma once
#include <cstddef>
#include <cstdint>

namespace perfmon {
namespace internal {

struct TssCounter {
  volatile uint_fast64_t calls;
  volatile uint_fast64_t ticks;
};

struct TssCounters {
  size_t size;
  TssCounter *counters;
};

extern thread_local TssCounters global_tss_counters;

void ExpandTssCounters();

inline void UpdateTssCounter(size_t counter_index, uint_fast64_t ticks) {
  if (global_tss_counters.size <= counter_index) {
    ExpandTssCounters();
  }
  auto &counter = global_tss_counters.counters[counter_index];
  counter.calls += 1;
  counter.ticks += ticks;
}

void UnsafeFlushTssCounters();  // Must be protected by the global mutex!

}  // namespace internal
}  // namespace perfmon
