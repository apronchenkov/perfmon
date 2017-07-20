#include "perfmon/cpu_frequency.h"
#include "perfmon.h"
#include "perfmon/ticks.h"

#include <algorithm>
#include <chrono>
#include <thread>
#include <vector>

namespace perfmon {
namespace internal {

namespace {

/** Simple estimation of the cpu frequency. */
template <class ClockType, class Rep, class Period>
double GetCpuFrequencySampleImpl(
    const std::chrono::duration<Rep, Period> &sleep_duration) {
  const auto startTick = ReadTickCounter();
  const auto startClock = ClockType::now();
  std::this_thread::sleep_for(sleep_duration);
  const auto stopTick = ReadTickCounter();
  const auto stopClock = ClockType::now();

  const auto secondsElapsed =
      std::chrono::duration_cast<std::chrono::duration<double>>(stopClock -
                                                                startClock)
          .count();
  return TicksElapsed(startTick, stopTick) / secondsElapsed;
}

template <class Rep, class Period>
double GetCpuFrequencySample(
    const std::chrono::duration<Rep, Period> &sleep_duration) {
  return GetCpuFrequencySampleImpl<std::chrono::high_resolution_clock>(
      sleep_duration);
}

/** Estimation based on median. */
template <class Rep, class Period>
double EstimateCpuFrequency(
    const std::chrono::duration<Rep, Period> &sleep_duration,
    size_t sample_size) {
  std::vector<double> sample(sample_size);
  for (auto &value : sample) {
    value = GetCpuFrequencySample(sleep_duration);
  }
  std::sort(sample.begin(), sample.end());
  return sample.at(sample.size() / 2);
}

}  // namespace

double EstimateCpuFrequency() {
  static const auto result =
      EstimateCpuFrequency(std::chrono::milliseconds(5), 11);
  return result;
}

}  // namespace internal
}  // namespace perfmon
