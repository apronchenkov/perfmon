#pragma once

#include "perfmon.h"
#include <mutex>

namespace perfmon {
namespace internal {

/* public api */
Counters GetCounters();
size_t GetCounterIndex(const char* counter_name);

/* private api */
std::unique_lock<std::mutex> GlobalLockGuard();
size_t UnsafeNumberOfCounters();
void UnsafeAccumulate(size_t counter_index, uint_fast64_t calls, uint_fast64_t ticks);
void UnsafeCommit(size_t counter_index, uint_fast64_t calls, uint_fast64_t ticks);

} } // namespace perfmon::internal
