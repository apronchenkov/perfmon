#include "perfmon.h"
#include <mutex>

namespace perfmon {
namespace internal {

namespace {

std::mutex& GlobalMutex()
{
    static auto& result = *(new std::mutex);
    return result;
}

std::forward_list<Counter>& GlobalCounters()
{
    static auto& result = *(new std::forward_list<Counter>());
    return result;
}

} // namespace

IteratorRange<std::forward_list<Counter>::const_iterator> Counters()
{
    auto& global_mutex = GlobalMutex();
    auto& global_counters = GlobalCounters();
    std::lock_guard<decltype(global_mutex)> lock(global_mutex);
    return { global_counters.begin(), global_counters.end() };
}

Counter& CounterRef(std::string&& counter_name)
{
    auto& global_mutex = GlobalMutex();
    auto& global_counters = GlobalCounters();
    std::lock_guard<decltype(global_mutex)> lock(global_mutex);
    for (auto& counter : global_counters) {
        if (counter.Name() == counter_name) {
            return counter;
        }
    }

    global_counters.emplace_front(std::move(counter_name));
    return global_counters.front();
}

} } // namespace perfmon::internal
