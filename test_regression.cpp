#include "public/perfmon.h"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_average_seconds_division_by_zero) {
  PERFMON_SCOPE("xyz");
  auto counter = PERFMON_COUNTERS()["xyz"];
  BOOST_REQUIRE_EQUAL(counter.Calls(), 0);
  BOOST_REQUIRE_EQUAL(counter.AverageSeconds(), 0.0);
}

BOOST_AUTO_TEST_CASE(test_name_shadowing) {
  PERFMON_STATEMENT("abc") { PERFMON_SCOPE("uvw"); }
  BOOST_REQUIRE_EQUAL(PERFMON_COUNTERS()["abc"].Calls(), 1);
  BOOST_REQUIRE_EQUAL(PERFMON_COUNTERS()["uvw"].Calls(), 1);
}
