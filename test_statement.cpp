#include "perfmon.h"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_statement_undef) {
  const auto counters = PERFMON_COUNTERS();
  BOOST_REQUIRE_EQUAL(counters["statement_undef"].Name(), "");
  BOOST_REQUIRE_EQUAL(counters["statement_undef"].Calls(), 0);
  BOOST_REQUIRE_EQUAL(counters["statement_undef"].Ticks(), 0);
}

BOOST_AUTO_TEST_CASE(test_statement_def) {
  PERFMON_STATEMENT("statement_def_A") { /* nop */
    ;
  };
  PERFMON_STATEMENT("statement_def_B") { /* nop */
    ;
  };
  PERFMON_STATEMENT("statement_def_B") { /* nop */
    ;
  };

  size_t statement_def_A_found = 0;
  size_t statement_def_B_found = 0;
  for (const auto &counter : PERFMON_COUNTERS()) {
    if (counter.Name() == "statement_def_A") {
      BOOST_REQUIRE_EQUAL(counter.Calls(), 1);
      ++statement_def_A_found;
    } else if (counter.Name() == "statement_def_B") {
      BOOST_REQUIRE_EQUAL(counter.Calls(), 2);
      ++statement_def_B_found;
    }
  }

  BOOST_REQUIRE_EQUAL(statement_def_A_found, 1);
  BOOST_REQUIRE_EQUAL(statement_def_B_found, 1);
}

BOOST_AUTO_TEST_CASE(test_statement_calls) {
  const int N = 1024;
  PERFMON_STATEMENT("statement_calls_A")
  for (int index = 0; index < N; ++index) {
    PERFMON_STATEMENT("statement_calls_B")
    for (int jndex = 0; jndex < N; ++jndex) {
      PERFMON_STATEMENT("statement_calls_C"){
          /* nop */
      };
    }
  }

  const auto counters = PERFMON_COUNTERS();
  BOOST_REQUIRE_EQUAL(counters["statement_calls_A"].Calls(), 1);
  BOOST_REQUIRE_EQUAL(counters["statement_calls_B"].Calls(), N);
  BOOST_REQUIRE_EQUAL(counters["statement_calls_C"].Calls(), N * N);

  BOOST_REQUIRE(counters["statement_calls_A"].Ticks() >
                counters["statement_calls_B"].Ticks());
  BOOST_REQUIRE(counters["statement_calls_B"].Ticks() >
                counters["statement_calls_C"].Ticks());
  BOOST_REQUIRE(counters["statement_calls_C"].Ticks() > 1);
}
