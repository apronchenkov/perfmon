#ifndef PERFMON_INL_H
#error "You should no use this file directly. Please include the 'perfmon.h'."
#endif  // PERFMON_INL_H

#include "cpu_frequency.h"
#include "thread_specific_counters.h"
#include "ticks.h"
#include <vector>

namespace perfmon {

inline Counter::Counter(const std::string *name_ptr, uint64_t calls,
                        uint64_t ticks)
    : name_ptr_(name_ptr), calls_(calls), ticks_(ticks) {}

inline const std::string &Counter::Name() const { return *name_ptr_; }

inline uint64_t Counter::Calls() const { return calls_; }

inline uint64_t Counter::Ticks() const { return ticks_; }

inline double Counter::Seconds() const {
  return Ticks() / internal::EstimateCpuFrequency();
}

inline double Counter::AverageSeconds() const {
  return Calls() ? Seconds() / Calls() : 0.0;
}

inline Counter Counters::operator[](const char *name) const {
  for (auto it = begin(); it != end(); ++it) {
    if (it->Name() == name) {
      return *it;
    }
  }
  static const std::string NONE;
  return Counter(&NONE, 0, 0);
}

namespace internal {
class Monitor {
 public:
  explicit Monitor(size_t counter_index)
      : counter_index_(counter_index), start_tick_(ReadTickCounter()) {}

  ~Monitor() {
    UpdateTssCounter(counter_index_,
                     TicksElapsed(start_tick_, ReadTickCounter()));
  }

  explicit operator bool() const { return false; }

 private:
  const size_t counter_index_;
  const Tick start_tick_;
};

size_t GetCounterIndex(const char *counter_name);

Counters GetCounters();

}  // namespace internal
}  // namespace perfmon

#undef PERFMON_COUNTERS
#undef PERFMON_SCOPE
#undef PERFMON_STATEMENT

#define PERFMON_COUNTERS() (::perfmon::internal::GetCounters())

#define PERFMON_INTERNAL__CONCAT(x, y) x##y
#define PERFMON_INTERNAL_CONCAT(x, y) PERFMON_INTERNAL__CONCAT(x, y)
#define PERFMON_INTERNAL_SCOPE_VAR \
  PERFMON_INTERNAL_CONCAT(perfmon_scope_monitor_, __COUNTER__)
#define PERFMON_INTERNAL_INDEX(counter_name)                           \
  ([] {                                                                \
    constexpr const char *name = (counter_name);                       \
    static size_t result = ::perfmon::internal::GetCounterIndex(name); \
    return result;                                                     \
  }())

#define PERFMON_SCOPE(counter_name)       \
  const auto PERFMON_INTERNAL_SCOPE_VAR = \
      ::perfmon::internal::Monitor(PERFMON_INTERNAL_INDEX(counter_name))

#define PERFMON_STATEMENT(counter_name)                                       \
  if (PERFMON_SCOPE(                                                          \
          counter_name)) { /* Suppress the 'control may reach end of non-void \
                              function' warning */                            \
  } else
