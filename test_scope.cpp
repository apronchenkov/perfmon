#include "public/perfmon.h"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_scope_undef) {
  const auto counters = PERFMON_COUNTERS();
  BOOST_REQUIRE_EQUAL(counters["scope_undef"].Name(), "");
  BOOST_REQUIRE_EQUAL(counters["scope_undef"].Calls(), 0);
  BOOST_REQUIRE_EQUAL(counters["scope_undef"].Ticks(), 0);
}

BOOST_AUTO_TEST_CASE(test_scope_def) {
  { PERFMON_SCOPE("scope_def_A"); }
  { PERFMON_SCOPE("scope_def_B"); }
  { PERFMON_SCOPE("scope_def_B"); }

  size_t scope_def_A_found = 0;
  size_t scope_def_B_found = 0;
  for (const auto &counter : PERFMON_COUNTERS()) {
    if (counter.Name() == "scope_def_A") {
      BOOST_REQUIRE_EQUAL(counter.Calls(), 1);
      ++scope_def_A_found;
    } else if (counter.Name() == "scope_def_B") {
      BOOST_REQUIRE_EQUAL(counter.Calls(), 2);
      ++scope_def_B_found;
    }
  }

  BOOST_REQUIRE_EQUAL(scope_def_A_found, 1);
  BOOST_REQUIRE_EQUAL(scope_def_B_found, 1);
}

BOOST_AUTO_TEST_CASE(test_scope_call) {
  const int N = 1024;
  {
    PERFMON_SCOPE("scope_call_A");
    for (int index = 0; index < N; ++index) {
      PERFMON_SCOPE("scope_call_B");
      for (int jndex = 0; jndex < N; ++jndex) {
        PERFMON_SCOPE("scope_call_C");
        /* nop */
      }
    }
  }

  const auto &counters = PERFMON_COUNTERS();
  BOOST_REQUIRE_EQUAL(counters["scope_call_A"].Calls(), 1);
  BOOST_REQUIRE_EQUAL(counters["scope_call_B"].Calls(), N);
  BOOST_REQUIRE_EQUAL(counters["scope_call_C"].Calls(), N * N);

  BOOST_REQUIRE(counters["scope_call_A"].Ticks() >
                counters["scope_call_B"].Ticks());
  BOOST_REQUIRE(counters["scope_call_B"].Ticks() >
                counters["scope_call_C"].Ticks());
  BOOST_REQUIRE(counters["scope_call_C"].Ticks() > 1);

  BOOST_REQUIRE_CLOSE(static_cast<double>(counters["scope_call_A"].Ticks()),
                      static_cast<double>(counters["scope_call_B"].Ticks()),
                      1.0);
}
