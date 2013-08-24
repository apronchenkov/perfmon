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
 *   or the PERFMON_SCOPE for any scopes inside a function:
 *
 *     {
 *         PERFMON_SCOPE("counter_name");
 *         ... // scope body
 *     }
 *
 *   or the PERFMON_STATEMENT for a statement:
 *
 *     PERFMON_STATEMENT("counter_name") {
 *         ... // statement
 *     }
 *
 *   WARNING! Most probably you would not like to use the PERFMON macros in
 *   recursive scopes. You could, if you understand what you want; but values of
 *   tick counter would be a bit crazy for a human being.
 *
 *   You could get current statistics in the following way:
 *
 *     for (const auto& counter : PERFMON_COUNTERS) {
 *        std::cout << counter.Name() << ' ' << counter.Calls() << ' ' << counter.Seconds() << std::endl;
 *     }
 */

#include "cpu_frequency.h"
#include "ticks.h"
#include <atomic>
#include <list>

namespace perfmon {

struct Counter {
    /** Total number of calls */
    std::atomic_uint_fast64_t calls;
    uint64_t Calls() const { return calls; }

    /** Total ticks have spent */
    std::atomic_uint_fast64_t ticks;
    uint64_t Ticks() const { return ticks; }

    /** Name of the counter scope */
    const char* const name;
    const char* Name() const { return name; }

    /** Total seconds have spent */
    double Seconds() const { return static_cast<double>(ticks) / EstimateCpuFrequency(); }

    /** Average seconds have spent per call */
    double AverageSeconds() const { return Seconds() / calls; }

    /** Test the counter for any value. */
    explicit operator bool () const { return calls || ticks; }

    /** Reset the counter */
    void Reset() { while (*this) { calls = 0; ticks = 0; } }
    void Reset() const { const_cast<Counter*>(this)->Reset(); }

    /* Ctor */
    Counter() : calls(0), ticks(0), name(nullptr) { }
};

class Counters {
public:
    typedef std::list<Counter>::const_iterator const_iterator;

    Counters(const_iterator begin, const_iterator end)
      : begin_(begin)
      , end_(end)
    { }

    /** Iterator range interface */
    const_iterator begin() const { return begin_; }
    const_iterator end() const { return end_; }

private:
    const_iterator begin_;
    const_iterator end_;
};

} // namespace perfmon

/** Get list of the available counters */
#define PERFMON_COUNTERS

/** Get a counter with a specific name */
#define PERFMON_COUNTER(counter_name)

/** Create a monitor for the following scope */
#define PERFMON_SCOPE(counter_name)

/** Create a monitor with current function name */
#define PERFMON_FUNCTION_SCOPE

/** Create a monitor for the following statement */
#define PERFMON_STATEMENT(counter_name)

#define PERFMON_INL_H
#include "perfmon_inl.h"
#undef PERFMON_INL_H
