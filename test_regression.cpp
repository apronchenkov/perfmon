#include <gtest/gtest.h>

#include "public/perfmon.h"

TEST(Regression, AverageSecondsDivisionByZero) {
  PERFMON_SCOPE("xyz");
  auto counter = PERFMON_COUNTERS()["xyz"];
  EXPECT_EQ(0U, counter.Calls());
  EXPECT_EQ(0.0, counter.AverageSeconds());
}

TEST(Regression, NameShadowing) {
  PERFMON_STATEMENT("abc") { PERFMON_SCOPE("uvw"); }
  EXPECT_EQ(1U, PERFMON_COUNTERS()["abc"].Calls());
  EXPECT_EQ(1U, PERFMON_COUNTERS()["uvw"].Calls());
}
