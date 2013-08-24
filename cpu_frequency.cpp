#include "cpu_frequency.h"
#include "perfmon.h"
#include "ticks.h"

#include <algorithm>
#include <chrono>
#include <thread>
#include <vector>

namespace {

/** Simple estimation of the cpu frequency. */
template <class Rep, class Period>
double EstimateCpuFrequency(const std::chrono::duration<Rep, Period>& sleep_duration)
{
    typedef std::conditional<std::chrono::high_resolution_clock::is_monotonic,
                             std::chrono::high_resolution_clock,
                             std::chrono::monotonic_clock>::type ClockType;

    const auto startTick = ReadTickCounter();
    const auto startClock = ClockType::now();
    std::this_thread::sleep_for(sleep_duration);
    const auto stopTick = ReadTickCounter();
    const auto stopClock = ClockType::now();

    const auto secondsElapsed = std::chrono::duration_cast<std::chrono::duration<double>>(stopClock - startClock).count();
    return TicksElapsed(startTick, stopTick) / secondsElapsed;
}

/** Estimation based on median. */
template <class Rep, class Period>
double EstimateCpuFrequency(const std::chrono::duration<Rep, Period>& sleep_duration, size_t sample_size)
{
    std::vector<double> sample(sample_size);
    for (auto& value : sample) {
        value = EstimateCpuFrequency(sleep_duration);
    }
    std::sort(sample.begin(), sample.end());
    return sample.at(sample.size() / 2);
}

} // namespace


double EstimateCpuFrequency()
{
    static const auto result = EstimateCpuFrequency(std::chrono::milliseconds(5), 11);
    return result;
}
