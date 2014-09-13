#pragma once

/**
 * Get current value of the tick counter:
 *
 *   Tick ReadTickCounter(void);
 *
 * Evaluate how many ticks elapsed between two moments:
 *
 *   uint64_t TicksElapsed(Tick startTick, Tick stopTick);
 *
 * Frequency of Ticks depends on hardware and software environment.
 * But you may be sure that the frequency is permanent at runtime.
 *
 */

#include <cstdint>

namespace perfmon {
namespace internal {

#undef ticks_h__ok__

#if defined(__GNUC__) || defined(__ICC) || defined(__clang__)
#if defined(__i386__)

typedef uint64_t Tick;

inline Tick ReadTickCounter(void)
{
    Tick result;
    __asm__ __volatile__("rdtsc": "=A" (result));
    return result;
}

inline uint64_t TicksElapsed(Tick startTick, Tick stopTick)
{
    return stopTick - startTick;
}

#define ticks_h__ok__
#endif // defined(__i386__)

#if defined(__x86_64__)

typedef uint64_t Tick;

inline Tick ReadTickCounter(void)
{
    uint32_t low, high;
    asm volatile("rdtsc" : "=a" (low), "=d" (high));
    return (static_cast<uint64_t>(high) << 32) | low;
}

inline uint64_t TicksElapsed(Tick startTick, Tick stopTick)
{
    return stopTick - startTick;
}

#define ticks_h__ok__
#endif // defined(__x86_64__)
#endif // defined(__GNUC__) || defined(__ICC) || defined(__clang__)

#ifndef ticks_h__ok__
#error Your platform is not supported.
#endif // ticks_h__ok__

} } // namespace perfmon::internal