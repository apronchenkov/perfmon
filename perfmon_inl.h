#ifndef PERFMON_INL_H
#error "You should no use this file directly. Please include the 'perfmon.h'."
#endif // PERFMON_INL_H

#include <list>

namespace perfmon {
namespace internal {

class Monitor {
public:
    explicit Monitor(::perfmon::Counter& counter_ref)
      : counter_ref_(counter_ref)
      , start_tick_(ReadTickCounter())
    { }

    ~Monitor()
    {
        counter_ref_.ticks += TicksElapsed(start_tick_, ReadTickCounter());
        ++counter_ref_.calls;
    }

    explicit operator bool () const
    {
        return false;
    }

private:
    ::perfmon::Counter& counter_ref_;
    const Tick start_tick_;
};

::perfmon::Counters Counters();

::perfmon::Counter& CounterRef(const char* counter_name);

} } // namespace perfmon::internal

#undef PERFMON_COUNTERS
#undef PERFMON_COUNTER
#undef PERFMON_SCOPE
#undef PERFMON_STATEMENT
#undef PERFMON_FUNCTION_SCOPE

/** Get list of counters */
#define PERFMON_COUNTERS                  (::perfmon::internal::Counters())

/** Get cached ref of the counter */
#define PERFMON_COUNTER(counter_name)     ([](const char* name)->::perfmon::Counter& { static auto& counter_ref = ::perfmon::internal::CounterRef(name); return counter_ref; }(counter_name))

/** Function scope */
#define PERFMON_FUNCTION_SCOPE            PERFMON_SCOPE(__FUNCTION__)


#ifdef NDEBUG
#define PERFMON_SCOPE(counter_name)       do { } while (false)
#define PERFMON_STATEMENT(counter_name)   if (false) { throw; } else
#else // NDEBUG
/** Scope */
#define PERFMON_SCOPE(counter_name)       const auto PERFMON_INTERNAL_SCOPE_VAR = ::perfmon::internal::Monitor(PERFMON_COUNTER(counter_name))
/** Statement */
#define PERFMON_STATEMENT(counter_name)   if (PERFMON_SCOPE(counter_name)) { /* Suppress the 'control may reach end of non-void function' warning */ } else
#endif // NDEBUG

#define PERFMON_INTERNAL__CONCAT(x, y)    x ## y
#define PERFMON_INTERNAL_CONCAT(x, y)     PERFMON_INTERNAL__CONCAT(x, y)
#define PERFMON_INTERNAL_SCOPE_VAR        PERFMON_INTERNAL_CONCAT(perfmon_scope_monitor_, __COUNTER__)


