#pragma once

/**
 * Performance monitor library
 *
 * Usage:
 *
 *   You could use the PERFMON_SCOPE for any scopes inside a function:
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
 *     for (const auto& counter : PERFMON_COUNTERS()) {
 *        std::cout << counter.Name() << ' ' << counter.Calls() << ' ' << counter.Seconds() << std::endl;
 *     }
 */

#include <atomic>
#include <string>

namespace perfmon {

class Counter {
public:
    /** Name of the counter scope */
    const std::string& Name() const;

    /** Total number of calls */
    uint64_t Calls() const;

    /** Total ticks have spent */
    uint64_t Ticks() const;

    /** Total seconds have spent */
    double Seconds() const;

    /** Average seconds have spent per call */
    double AverageSeconds() const;

    /** Test the counter for any value. */
    explicit operator bool () const;

    /** Update the counter */
    void Update(uint64_t callTicks);

    /** Reset the counter */
    void Reset();
    void Reset() const;

    /** Constructor */
    explicit Counter(std::string&& name);

private:
    std::string name_;
    std::atomic_uint_fast64_t calls_;
    std::atomic_uint_fast64_t ticks_;
};

} // namespace perfmon

/** Get list of the available counters */
#define PERFMON_COUNTERS()

/** Get a counter with a specific name */
#define PERFMON_COUNTER(counter_name)

/** Create a monitor for the following scope */
#define PERFMON_SCOPE(counter_name)

/** Create a monitor for the following statement */
#define PERFMON_STATEMENT(counter_name)

#define PERFMON_INL_H
#include "perfmon/perfmon_inl.h"
#undef PERFMON_INL_H
