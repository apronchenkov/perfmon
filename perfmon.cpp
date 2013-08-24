#include "perfmon.h"

#include <cstdlib>
#include <cstring>
#include <iterator>
#include <list>
#include <mutex>

namespace perfmon {
namespace internal {

namespace {

std::mutex g_mutex;

std::list< ::perfmon::Counter > g_counters(1);

} // namespace

::perfmon::Counters Counters()
{
    std::lock_guard<std::mutex> lock(g_mutex);
    return ::perfmon::Counters(g_counters.begin(), std::prev(g_counters.end()));
}

::perfmon::Counter& CounterRef(const char* counter_name)
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
