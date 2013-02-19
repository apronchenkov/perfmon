#include "perfmon.h"

#include <cstdlib>
#include <list>
#include <mutex>

namespace perfmon {
namespace internal {

namespace {

std::list<::perfmon::Counter> g_counters(1);

std::mutex g_mutex;

} // namespace

Counters counters()
{
    std::lock_guard<std::mutex> lock(g_mutex);
    return Counters(g_counters.begin(), --g_counters.end());
}

Counter& counter(const char* counter_name)
{
    std::lock_guard<std::mutex> lock(g_mutex);

    for (auto& counter : g_counters) {
        if (counter.name && 0 == ::strcmp(counter.name, counter_name)) {
            return counter;
        }
    }

    Counter& result = g_counters.back();
    g_counters.resize(g_counters.size() + 1);

    result.calls = 0;
    result.ticks = 0;
    result.name = counter_name;
    return result;
}

} } // namespace perfmon::internal
