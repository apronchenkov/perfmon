#pragma once

#include <atomic>
#include <list>
#include <utility>

namespace perfmon {

struct Counter {
    std::atomic_uint_fast64_t calls;
    std::atomic_uint_fast64_t ticks;
};

typedef std::list<Counter>::const_iterator Iterator;

Counter& counter(const char* name);
std::pair<Iterator, Iterator> counters();

} // perfmon


#define PERFMON_COUNTERS                        (::perfmon::counters())
#define PERFMON_COUNTER(name)                   ([] -> ::perfmon::Counter& { static auto& counter = ::perfmon::counter(name); return counter; }())


#define PERFMON_INTERNAL__CONCAT(x, y)          x ## y
#define PERFMON_INTERNAL_CONCAT(x, y)           PERFMON_INTERNAL__CONCAT(x, y)
#define PERFMON_INTERNAL_SCOPE_MONITOR_NAME     PERFMON_INTERNAL_CONCAT(perfmon_scope_monitor_, __COUNTER__)

#define PERFMON_SCOPE(name)                     const ::perfmon::Monitor PERFMON_INTERNAL_SCOPE_MONITOR_NAME (PERFMON_COUNTER(name))
#define PERFMON_FUNCTION_SCOPE                  PERFMON_SCOPE (__FUNCTION__)
