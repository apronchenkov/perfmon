#include "perfmon.h"

#include <cstdlib>
#include <list>
#include <mutex>

namespace perfmon {
namespace internal {

namespace {

std::mutex g_mutex;

std::list<::perfmon::Counter> g_counters;

/* Enforce counter creation at runtime. */
const auto g_is_init = []() {
    g_counters.resize(2);
    const_cast<const char*&>(g_counters.front().name) = "EstimateCpuFrequency";
    return true;
}();

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
    g_counters.emplace_back();

    const_cast<const char*&>(result.name) = counter_name;
    return result;
}

} } // namespace perfmon::internal
