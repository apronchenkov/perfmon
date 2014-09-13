#ifndef PERFMON_INL_H
#error "You should no use this file directly. Please include the 'perfmon.h'."
#endif // PERFMON_INL_H

#include "cpu_frequency.h"
#include "iterator_range.h"
#include "ticks.h"
#include <forward_list>

namespace perfmon {

inline const std::string& Counter::Name() const
{
    return name_;
}

inline uint64_t Counter::Calls() const
{
    return calls_;
}

inline uint64_t Counter::Ticks() const
{
    return ticks_;
}

inline double Counter::Seconds() const
{
    return ticks_ / internal::EstimateCpuFrequency();
}

inline double Counter::AverageSeconds() const
{
    return Seconds() / calls_;
}

inline Counter::operator bool () const
{
    return calls_ > 0;
}

inline void Counter::Update(uint64_t callTicks)
{
    calls_++;
    ticks_ += callTicks;
}

inline void Counter::Reset()
{
    calls_ = 0;
    ticks_ = 0;
}

inline void Counter::Reset() const
{
    const_cast<Counter*>(this)->Reset();
}

inline Counter::Counter(std::string&& name)
  : name_(std::move(name))
  , calls_(0)
  , ticks_(0)
{ }


namespace internal {

class Monitor {
public:
    explicit Monitor(Counter& counter_ref)
      : counter_ref_(counter_ref)
      , start_tick_(ReadTickCounter())
    { }

    ~Monitor()
    {
        counter_ref_.Update(TicksElapsed(start_tick_, ReadTickCounter()));
    }

    explicit operator bool () const
    {
        return false;
    }

private:
    Counter& counter_ref_;
    const Tick start_tick_;
};

Counter& CounterRef(std::string&& counter_name);

IteratorRange<std::forward_list<Counter>::const_iterator> Counters();

} } // namespace perfmon::internal

#undef PERFMON_COUNTERS
#undef PERFMON_COUNTER
#undef PERFMON_SCOPE
#undef PERFMON_STATEMENT

#define PERFMON_INTERNAL__CONCAT(x, y)    x ## y
#define PERFMON_INTERNAL_CONCAT(x, y)     PERFMON_INTERNAL__CONCAT(x, y)
#define PERFMON_INTERNAL_SCOPE_VAR        PERFMON_INTERNAL_CONCAT(perfmon_scope_monitor_, __COUNTER__)

/** Get list of counters */
#define PERFMON_COUNTERS                  (::perfmon::internal::Counters())

/** Get cached ref of the counter */
#define PERFMON_COUNTER(counter_name)     ([](const char* name) -> ::perfmon::Counter& { static auto& counter_ref = ::perfmon::internal::CounterRef(name); return counter_ref; }(counter_name))

/** Scope */
#define PERFMON_SCOPE(counter_name)       const auto PERFMON_INTERNAL_SCOPE_VAR = ::perfmon::internal::Monitor(PERFMON_COUNTER(counter_name))
/** Statement */
#define PERFMON_STATEMENT(counter_name)   if (PERFMON_SCOPE(counter_name)) { /* Suppress the 'control may reach end of non-void function' warning */ } else

