#pragma once

/**
 * Performance monitor library
 *
 * Usage:
 *
 *   You could use the PERFMON_FUNCTION_SCOPE macro for functions:
 *
 *     void function()
 *     {
 *         PERFMON_FUNCTION_SCOPE;
 *         ... // function body
 *     }
 *
 *   or the PERFMON_SCOPE for any scope
 *
 *     {
 *         PERFMON_SCOPE("scope_name");
 *         ... // scope body
 *     }
 *
 *   WARNING! Most probably you would not like to use the PERFMON macros in
 *   recursive scopes. You could, if you understand what you want; but values of
 *   tick counter would be a bit crazy for a human being.
 *
 *   You could get current statistics in the following way:
 *
 *     for (const auto& counter : PERFMON_COUNTERS) {
 *        std::cout << counter.name << ' ' << counter.calls << ' ' << counter.Seconds() << std::endl;
 *     }
 */

#include "cpu_frequency.h"
#include "ticks.h"
#include <atomic>

namespace perfmon {

struct Counter {
    /** Total number of calls */
    std::atomic_uint_fast64_t calls;

    /** Total ticks have spent */
    std::atomic_uint_fast64_t ticks;

    /** Name of the counter scope */
    const char* const name;

    /** Total seconds have spent */
    double Seconds() const { return 1.0 / EstimateCpuFrequency() * ticks; }

    /** Average seconds have spent per call */
    double AverageSeconds() const { return Seconds() / calls; }

    /* Ctor */
    Counter() : calls(0), ticks(0), name(NULL) { }
};

} // namespace perfmon

/** Get list of the available counters */
#define PERFMON_COUNTERS

/** Get a counter with specific name */
#define PERFMON_COUNTER(counter_name)

/** Create a monitor for current scope */
#define PERFMON_SCOPE(counter_name)

/** Create a monitor with current function name */
#define PERFMON_FUNCTION_SCOPE    PERFMON_SCOPE(__FUNCTION__)

#define PERFMON_INL_H
#include "perfmon_inl.h"
#undef PERFMON_INL_H
