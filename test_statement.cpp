#include <gtest/gtest.h>

#include "public/perfmon.h"

TEST(Statement, Unknown) {
  const auto counters = PERFMON_COUNTERS();
  EXPECT_EQ("", counters["statement_undef"].Name());
  EXPECT_EQ(0U, counters["statement_undef"].Calls());
  EXPECT_EQ(0U, counters["statement_undef"].Ticks());
}

TEST(Statement, Trivial) {
  PERFMON_STATEMENT("statement_def_A"){
      /* nop */
  };
  PERFMON_STATEMENT("statement_def_B"){
      /* nop */
  };
  PERFMON_STATEMENT("statement_def_B"){
      /* nop */
  };

  size_t statement_def_A_found = 0;
  size_t statement_def_B_found = 0;
  for (const auto &counter : PERFMON_COUNTERS()) {
    if (counter.Name() == "statement_def_A") {
      EXPECT_EQ(1U, counter.Calls());
      ++statement_def_A_found;
    } else if (counter.Name() == "statement_def_B") {
      EXPECT_EQ(2U, counter.Calls());
      ++statement_def_B_found;
    }
  }

  EXPECT_EQ(1U, statement_def_A_found);
  EXPECT_EQ(1U, statement_def_B_found);
}

TEST(Statement, Nesting) {
  const unsigned int N = 1024;
  PERFMON_STATEMENT("statement_calls_A")
  for (unsigned int index = 0; index < N; ++index) {
    PERFMON_STATEMENT("statement_calls_B")
    for (unsigned int jndex = 0; jndex < N; ++jndex) {
      PERFMON_STATEMENT("statement_calls_C"){
          /* nop */
      };
    }
  }

  const auto counters = PERFMON_COUNTERS();
  EXPECT_EQ(1U, counters["statement_calls_A"].Calls());
  EXPECT_EQ(N, counters["statement_calls_B"].Calls());
  EXPECT_EQ(N * N, counters["statement_calls_C"].Calls());

  EXPECT_GT(counters["statement_calls_A"].Ticks(),
            counters["statement_calls_B"].Ticks());
  EXPECT_GT(counters["statement_calls_B"].Ticks(),
            counters["statement_calls_C"].Ticks());
  EXPECT_GT(counters["statement_calls_C"].Ticks(), 1U);
}
