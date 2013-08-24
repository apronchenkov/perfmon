#include "perfmon.h"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(test_statement_undef)
{
    BOOST_REQUIRE( !PERFMON_COUNTER("statement_undef") );
    BOOST_REQUIRE_EQUAL( PERFMON_COUNTER("statement_undef").Calls(), 0 );
    BOOST_REQUIRE_EQUAL( PERFMON_COUNTER("statement_undef").Ticks(), 0 );
}


BOOST_AUTO_TEST_CASE(test_statement_def)
{
    PERFMON_STATEMENT("statement_defA") { /* nop */; };
    PERFMON_STATEMENT("statement_defB") { /* nop */; };
    PERFMON_STATEMENT("statement_defC") { /* nop */; };
    BOOST_REQUIRE( PERFMON_COUNTER("statement_defA") );
    BOOST_REQUIRE( PERFMON_COUNTER("statement_defB") );
    BOOST_REQUIRE( PERFMON_COUNTER("statement_defC") );
    BOOST_REQUIRE_EQUAL( PERFMON_COUNTER("statement_defA").Calls(), 1 );
    BOOST_REQUIRE_EQUAL( PERFMON_COUNTER("statement_defB").Calls(), 1 );
    BOOST_REQUIRE_EQUAL( PERFMON_COUNTER("statement_defC").Calls(), 1 );
}


BOOST_AUTO_TEST_CASE(test_statement_calls)
{
    const int N = 1024;

    PERFMON_STATEMENT("statement_callsA")
        for (int index = 0; index < N; ++index) {
            PERFMON_STATEMENT("statement_callsB")
            for (int jndex = 0; jndex < N; ++jndex) {
                PERFMON_STATEMENT("statement_callsC") {
                    /* nop */
                };
            }
        }

    BOOST_REQUIRE( PERFMON_COUNTER("statement_callsA") );
    BOOST_REQUIRE( PERFMON_COUNTER("statement_callsB") );
    BOOST_REQUIRE( PERFMON_COUNTER("statement_callsC") );

    BOOST_REQUIRE_EQUAL( PERFMON_COUNTER("statement_callsA").Calls(), 1 );
    BOOST_REQUIRE_EQUAL( PERFMON_COUNTER("statement_callsB").Calls(), N );
    BOOST_REQUIRE_EQUAL( PERFMON_COUNTER("statement_callsC").Calls(), N*N );

    BOOST_REQUIRE( PERFMON_COUNTER("statement_callsA").Ticks() > 0 );
    BOOST_REQUIRE( PERFMON_COUNTER("statement_callsB").Ticks() > 0 );
    BOOST_REQUIRE( PERFMON_COUNTER("statement_callsC").Ticks() > 0 );
}
