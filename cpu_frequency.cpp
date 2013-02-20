#include "cpu_frequency.h"
#include "perfmon.h"
#include "ticks.h"

#include <algorithm>
#include <chrono>
#include <thread>
#include <vector>

namespace {

/** Simple estimation of the cpu frequency. */
double EstimateCpuFrequency(double sleep_seconds)
{
    typedef std::conditional<std::chrono::high_resolution_clock::is_steady, std::chrono::high_resolution_clock, std::chrono::steady_clock>::type ClockType;

    const auto sleep_duration = std::chrono::duration<double>(sleep_seconds);

    const auto startTick = ReadTickCounter();
    const auto startClock = ClockType::now();
    std::this_thread::sleep_for(sleep_duration);
    const auto stopTick = ReadTickCounter();
    const auto stopClock = ClockType::now();

    const auto secondsElapsed = std::chrono::duration_cast<std::chrono::duration<double>>(stopClock - startClock).count();
    return TicksElapsed(startTick, stopTick) / secondsElapsed;
}

/** Estimation based on median. */
double EstimateCpuFrequency(double total_sleep_seconds, size_t sample_size)
{
    PERFMON_SCOPE("EstimateCpuFrequency");
    const auto sleep_seconds = total_sleep_seconds / sample_size;

    std::vector<double> sample(sample_size);
    for (auto& value : sample) {
        value = EstimateCpuFrequency(sleep_seconds);
    }

    std::sort(sample.begin(), sample.end());
    if (sample.size() % 2 == 0) {
        return (sample[sample.size()/2 - 1] + sample[sample.size()/2]) / 2.0;
    } else {
        return sample[sample.size()/2];
    }
}

} // namespace


double EstimateCpuFrequency()
{
    static const auto result = EstimateCpuFrequency(0.5, 7);
    return result;
}
