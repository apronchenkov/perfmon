#include "counters.h"
#include "perfmon/thread_specific_counters.h"
#include <deque>
#include <mutex>

namespace perfmon {
namespace internal {

namespace {

struct GlobalCounter {
    explicit GlobalCounter(const char* name)
      : name(name)
      , commited_calls(0)
      , commited_ticks(0)
    { }

    std::string name;
    uint_fast64_t commited_calls;
    uint_fast64_t commited_ticks;
    uint_fast64_t accumulated_calls;
    uint_fast64_t accumulated_ticks;
};

std::deque<GlobalCounter>& GlobalCounters()
{
    static auto * const result = new std::deque<GlobalCounter>();
    return *result;
}

} // namespace

std::unique_lock<std::mutex> GlobalLockGuard()
{
    static auto * const lock = new std::mutex;
    return std::unique_lock<std::mutex>(*lock);
}

Counters GetCounters()
{
    const auto guard = GlobalLockGuard();
    auto& global_counters = GlobalCounters();
    UnsafeFlushTssCounters();
    Counters result;
    result.reserve(global_counters.size());
    for (const auto& global_counter : global_counters) {
        result.emplace_back(
            &global_counter.name,
            global_counter.commited_calls + global_counter.accumulated_calls,
            global_counter.commited_ticks + global_counter.accumulated_ticks);
    }
    return result;
}

size_t GetCounterIndex(const char* counter_name)
{
    const auto guard = GlobalLockGuard();
    auto& global_counters = GlobalCounters();
    for (size_t i = 0; i < global_counters.size(); ++i) {
        if (global_counters[i].name == counter_name) {
            return i;
        }
    }
    global_counters.emplace_back(counter_name);
    return global_counters.size() - 1;
}

size_t UnsafeNumberOfCounters()
{
    return GlobalCounters().size();
}

void UnsafeResetAccumulators()
{
    auto& global_counters = GlobalCounters();
    for (auto& global_counter : global_counters) {
        global_counter.accumulated_calls = 0;
        global_counter.accumulated_ticks = 0;
    }
}

void UnsafeAccumulate(size_t counter_index, uint_fast64_t calls, uint_fast64_t ticks)
{
    auto& global_counters = GlobalCounters();
    global_counters.at(counter_index).accumulated_calls += calls;
    global_counters.at(counter_index).accumulated_ticks += ticks;
}

void UnsafeCommit(size_t counter_index, uint_fast64_t calls, uint_fast64_t ticks)
{
    auto& global_counters = GlobalCounters();
    global_counters.at(counter_index).commited_calls += calls;
    global_counters.at(counter_index).commited_ticks += ticks;
}

} } // namespace perfmon::internal
