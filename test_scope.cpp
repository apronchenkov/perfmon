#include "perfmon.h"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(test_scope_undef)
{
    BOOST_REQUIRE( !PERFMON_COUNTER("scope_undef") );
    BOOST_REQUIRE_EQUAL( PERFMON_COUNTER("scope_undef").Calls(), 0 );
    BOOST_REQUIRE_EQUAL( PERFMON_COUNTER("scope_undef").Ticks(), 0 );
}


BOOST_AUTO_TEST_CASE(test_scope_def)
{
    { PERFMON_SCOPE("scope_defA"); }
    { PERFMON_SCOPE("scope_defB"); }
    { PERFMON_SCOPE("scope_defC"); }
    BOOST_REQUIRE( PERFMON_COUNTER("scope_defA") );
    BOOST_REQUIRE( PERFMON_COUNTER("scope_defB") );
    BOOST_REQUIRE( PERFMON_COUNTER("scope_defC") );
    BOOST_REQUIRE_EQUAL( PERFMON_COUNTER("scope_defA").Calls(), 1 );
    BOOST_REQUIRE_EQUAL( PERFMON_COUNTER("scope_defB").Calls(), 1 );
    BOOST_REQUIRE_EQUAL( PERFMON_COUNTER("scope_defC").Calls(), 1 );
}


BOOST_AUTO_TEST_CASE(test_scope_calls)
{
    const int N = 1024;
    {
        PERFMON_SCOPE("scope_callsA");
        for (int index = 0; index < N; ++index) {
            PERFMON_SCOPE("scope_callsB");
            for (int jndex = 0; jndex < N; ++jndex) {
                PERFMON_SCOPE("scope_callsC");
                /* nop */
            }
        }
    }

    BOOST_REQUIRE( PERFMON_COUNTER("scope_callsA") );
    BOOST_REQUIRE( PERFMON_COUNTER("scope_callsB") );
    BOOST_REQUIRE( PERFMON_COUNTER("scope_callsC") );

    BOOST_REQUIRE_EQUAL( PERFMON_COUNTER("scope_callsA").Calls(), 1 );
    BOOST_REQUIRE_EQUAL( PERFMON_COUNTER("scope_callsB").Calls(), N );
    BOOST_REQUIRE_EQUAL( PERFMON_COUNTER("scope_callsC").Calls(), N*N );

    BOOST_REQUIRE( PERFMON_COUNTER("scope_callsA").Ticks() > 0 );
    BOOST_REQUIRE( PERFMON_COUNTER("scope_callsB").Ticks() > 0 );
    BOOST_REQUIRE( PERFMON_COUNTER("scope_callsC").Ticks() > 0 );
}
