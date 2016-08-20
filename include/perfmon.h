/**
 * Simple performance counters for C++11.
 *
 * Usage
 * -----
 *
 * You could use the PERFMON_SCOPE for a scope inside of a function:
 *
 *  {
 *    PERFMON_SCOPE("scope_name");
 *    ... // scope body
 *  }
 *
 * or the PERFMON_STATEMENT for a statement:
 *
 *  PERFMON_STATEMENT("counter_name") {
 *    ... // statement
 * }
 *
 * WARNING! Most probably you would not like to use the PERFMON macros in
 * recursive scopes. You could, if you understand what you want; but values of
 * tick counter would be a bit crazy for a human being.
 *
 *
 * You could get current statistics in the following way:
 *
 *   for (const auto& counter : PERFMON_COUNTERS()) {
 *     std::cout << counter.Name() << ' '
 *               << counter.Calls() << ' '
 *               << counter.Seconds() << std::endl;
 *   }
 *
 * NOTICE! Values of `PERFMON_COUNTERS()` can be imprecise because of
 * non-atomicity of the integer arithmetic. If you need to get a better
 * precision, you can call `PERFMON_COUNTERS()` multiple times and use
 * the median.
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace perfmon {

class Counter {
public:
    /** Constructor */
    Counter(const std::string* name_ptr, uint64_t calls, uint64_t ticks);

    /** Name of the counter scope */
    const std::string& Name() const;

    /** Total number of calls */
    uint64_t Calls() const;

    /** Total ticks have spent */
    uint64_t Ticks() const;

    /** Total seconds have been spent */
    double Seconds() const;

    /** Average seconds have been spent per call */
    double AverageSeconds() const;

private:
    const std::string* name_ptr_;
    uint64_t calls_;
    uint64_t ticks_;
};

class Counters : public std::vector<Counter> {
public:
    Counter operator[](const char* name) const;
};

} // namespace perfmon

/** Get list of the available counters */
#define PERFMON_COUNTERS()

/** Create a monitor for the following scope */
#define PERFMON_SCOPE(counter_name)

/** Create a monitor for the following statement */
#define PERFMON_STATEMENT(counter_name)

#define PERFMON_INL_H
#include "perfmon/perfmon_inl.h"
#undef PERFMON_INL_H
