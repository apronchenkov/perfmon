#include "perfmon.h"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(test_statement_undef)
{
    const auto counters = PERFMON_COUNTERS();
    BOOST_REQUIRE_EQUAL (counters["statement_undef"].Calls(), 0);
    BOOST_REQUIRE_EQUAL (counters["statement_undef"].Ticks(), 0);
}


BOOST_AUTO_TEST_CASE(test_statement_def)
{
    PERFMON_STATEMENT ("statement_defA") { /* nop */; };
    PERFMON_STATEMENT ("statement_defB") { /* nop */; };
    PERFMON_STATEMENT ("statement_defC") { /* nop */; };

    const auto counters = PERFMON_COUNTERS();
    BOOST_REQUIRE_EQUAL (counters["statement_defA"].Calls(), 1);
    BOOST_REQUIRE_EQUAL (counters["statement_defB"].Calls(), 1);
    BOOST_REQUIRE_EQUAL (counters["statement_defC"].Calls(), 1);
}


BOOST_AUTO_TEST_CASE(test_statement_calls)
{
    const int N = 1024;

    PERFMON_STATEMENT ("statement_callsA")
        for (int index = 0; index < N; ++index) {
            PERFMON_STATEMENT ("statement_callsB")
            for (int jndex = 0; jndex < N; ++jndex) {
                PERFMON_STATEMENT ("statement_callsC") {
                    /* nop */
                };
            }
        }

    const auto counters = PERFMON_COUNTERS();
    BOOST_REQUIRE_EQUAL (counters["statement_callsA"].Calls(), 1);
    BOOST_REQUIRE_EQUAL (counters["statement_callsB"].Calls(), N);
    BOOST_REQUIRE_EQUAL (counters["statement_callsC"].Calls(), N*N);

    BOOST_REQUIRE (counters["statement_callsA"].Ticks() > 0);
    BOOST_REQUIRE (counters["statement_callsB"].Ticks() > 0);
    BOOST_REQUIRE (counters["statement_callsC"].Ticks() > 0);
}
