#include "perfmon.h"

#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <iostream>

namespace {

std::vector<uint64_t> FibonacciSeries(size_t length)
{
    std::vector<uint64_t> result = {0, 1};
    while (result.size() < length) {
        result.push_back(result.rbegin()[0] + result.rbegin()[1]);
    }
    return result;
}

#define gcd(counter_name, a, b)   gcdImpl<__COUNTER__>(counter_name, a, b)

template <int INSTANCE_ID>
int gcdImpl(const char* counter_name, uint64_t a, uint64_t b)
{
    PERFMON_SCOPE (counter_name);
    for (;;) {
        if (a == 0) {
            return b;
        }
        b %= a;
        if (b == 0) {
            return a;
        }
        a %= b;
    }
}

} // namespace


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
    const auto fibonacci_series = FibonacciSeries(1024);

    for (auto left : fibonacci_series) {
        for (auto right : fibonacci_series) {
            gcd("scope_calls", left, right);
        }
    }
    BOOST_REQUIRE( PERFMON_COUNTER("scope_calls") );
    BOOST_REQUIRE_EQUAL( PERFMON_COUNTER("scope_calls").Calls(), fibonacci_series.size() * fibonacci_series.size() );
    BOOST_REQUIRE( PERFMON_COUNTER("scope_calls").Ticks() > 0 );
}


BOOST_AUTO_TEST_CASE(test_scope_ticks)
{
    const size_t N = 1024;
    const auto fibonacci_series = FibonacciSeries(N);

    for (auto left : fibonacci_series) {
        for (auto right : fibonacci_series) {
            gcd("scope_ticksA", left, right);
        }
    }
    for (auto left : fibonacci_series) {
        for (auto right : fibonacci_series) {
            gcd("scope_ticksB", left, right);
        }
    }

    const double scope_ticksA = PERFMON_COUNTER("scope_ticksA").Ticks();
    const double scope_ticksB = PERFMON_COUNTER("scope_ticksB").Ticks();
    BOOST_REQUIRE_CLOSE( scope_ticksA, scope_ticksB, 2.0 /*%*/ );
}
