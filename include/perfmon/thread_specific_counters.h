#pragma once
#include "config.h"
#include <cstddef>
#include <cstdint>

#if PERFMON_CONFIG__COMPILER_IS_MSVC
#define PERFMON_THREAD_SPECIFIC  __declspec(thread)
#else
#define PERFMON_THREAD_SPECIFIC  __thread
#endif

namespace perfmon {
namespace internal {

struct TssCounter {
    volatile uint_fast64_t calls;
    volatile uint_fast64_t ticks;
};

struct TssCounters {
    size_t size;
    TssCounter* counters;
};

extern PERFMON_THREAD_SPECIFIC TssCounters global_tss_counters;

void ExpandTssCounters();

inline void UpdateTssCounter(size_t counter_index, uint_fast64_t ticks)
{
    if (global_tss_counters.size <= counter_index) {
        ExpandTssCounters();
    }
    auto& counter = global_tss_counters.counters[counter_index];
    counter.calls += 1;
    counter.ticks += ticks;
}

void UnsafeFlushTssCounters(); // Must be protected by the global mutex!

} } // namespace perfmon::internal
