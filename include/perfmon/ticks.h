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

#undef PERFMON_TICKS_OK

#if defined(__GNUC__) || defined(__ICC) || defined(__clang__)
#if defined(__i386__)

typedef uint64_t Tick;

inline Tick ReadTickCounter(void) {
  Tick result;
  __asm__ __volatile__("rdtsc" : "=A"(result));
  return result;
}

inline uint64_t TicksElapsed(Tick startTick, Tick stopTick) {
  return stopTick - startTick;
}

#define PERFMON_TICKS_OK
#elif defined(__x86_64__)

typedef uint64_t Tick;

inline Tick ReadTickCounter(void) {
  uint32_t low, high;
  asm volatile("rdtsc" : "=a"(low), "=d"(high));
  return (static_cast<uint64_t>(high) << 32) | low;
}

inline uint64_t TicksElapsed(Tick startTick, Tick stopTick) {
  return stopTick - startTick;
}

#define PERFMON_TICKS_OK
#endif  // defined(__x86_64__)
#endif  // defined(__GNUC__) || defined(__ICC) || defined(__clang__)
}
}  // namespace perfmon

#ifndef PERFMON_TICKS_OK

#include <chrono>
namespace perfmon {
namespace internal {

using Tick = std::chrono::high_resolution_clock::time_point;

inline Tick ReadTickCounter() {
  return std::chrono::high_resolution_clock::now();
}

inline uint64_t TicksElapsed(Tick startTick, Tick stopTick) {
  return (stopTick - startTick).count();
}

}  // namespace internal
}  // namespace perfmon

#define PERFMON_TICKS_OK
#endif  // PERFMON_TICKS_OK

#undef PERFMON_TICKS_OK
