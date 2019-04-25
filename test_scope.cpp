#include <gtest/gtest.h>

#include "public/perfmon.h"

TEST(Scope, Unknown) {
  const auto counters = PERFMON_COUNTERS();
  EXPECT_EQ("", counters["scope_undef"].Name());
  EXPECT_EQ(0U, counters["scope_undef"].Calls());
  EXPECT_EQ(0U, counters["scope_undef"].Ticks());
}

TEST(Scope, Trivial) {
  { PERFMON_SCOPE("scope_def_A"); }
  { PERFMON_SCOPE("scope_def_B"); }
  { PERFMON_SCOPE("scope_def_B"); }

  size_t scope_def_A_found = 0;
  size_t scope_def_B_found = 0;
  for (const auto &counter : PERFMON_COUNTERS()) {
    if (counter.Name() == "scope_def_A") {
      EXPECT_EQ(1U, counter.Calls());
      ++scope_def_A_found;
    } else if (counter.Name() == "scope_def_B") {
      EXPECT_EQ(2U, counter.Calls());
      ++scope_def_B_found;
    }
  }

  EXPECT_EQ(1U, scope_def_A_found);
  EXPECT_EQ(1U, scope_def_B_found);
}

TEST(Scope, Nesting) {
  const unsigned int N = 1024;
  {
    PERFMON_SCOPE("scope_call_A");
    for (unsigned int index = 0; index < N; ++index) {
      PERFMON_SCOPE("scope_call_B");
      for (unsigned int jndex = 0; jndex < N; ++jndex) {
        PERFMON_SCOPE("scope_call_C");
        /* nop */
      }
    }
  }

  const auto &counters = PERFMON_COUNTERS();
  EXPECT_EQ(1U, counters["scope_call_A"].Calls());
  EXPECT_EQ(N, counters["scope_call_B"].Calls());
  EXPECT_EQ(N * N, counters["scope_call_C"].Calls());

  EXPECT_GT(counters["scope_call_A"].Ticks(), counters["scope_call_B"].Ticks());
  EXPECT_GT(counters["scope_call_B"].Ticks(), counters["scope_call_C"].Ticks());
  EXPECT_GT(counters["scope_call_C"].Ticks(), 1U);

  const auto a_ticks = static_cast<double>(counters["scope_call_A"].Ticks());
  const auto b_ticks = static_cast<double>(counters["scope_call_A"].Ticks());
  EXPECT_NEAR(a_ticks, b_ticks, 0.005 * (a_ticks + b_ticks));
}
