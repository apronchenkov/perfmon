#include "cpu_frequency.h"
#include "perfmon.h"
#include "ticks.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

#include <sys/time.h>
#include <unistd.h>

namespace {

/* Enforce counter creation at runtime. */
const char SELF_COUNTER_NAME[] = "EstimateCpuFrequency";
const auto& SELF_COUNTER_REF = PERFMON_COUNTER(SELF_COUNTER_NAME);

/** Simple estimation of the cpu frequency. */
double EstimateCpuFrequency(double sleep_seconds)
{
    struct timeval tvstart, tvstop;

    const auto startTick = ReadTickCounter();
    if (0 != gettimeofday(&tvstart, NULL)) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    if (0 != usleep(static_cast<useconds_t>(sleep_seconds * 1e6))) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    const auto stopTick = ReadTickCounter();
    if (0 != gettimeofday(&tvstop, NULL)) {
        return std::numeric_limits<double>::quiet_NaN();
    }

    const double secondsElapsed = (tvstop.tv_sec - tvstart.tv_sec) + (tvstop.tv_usec - tvstart.tv_usec) * 1e-6;
    return TicksElapsed(startTick, stopTick) / secondsElapsed;
}

/** Estimation based on median. */
double EstimateCpuFrequency(double total_sleep_seconds, size_t sample_size)
{
    PERFMON_SCOPE(SELF_COUNTER_NAME);
    const auto sleep_seconds = total_sleep_seconds / sample_size;

    std::vector<double> sample;
    sample.reserve(sample_size);

    for (size_t index = 0; index < sample_size; ++index) {
        const auto value = EstimateCpuFrequency(sleep_seconds);
        if (!std::isnan(value)) {
            sample.push_back(value);
        }
    }

    if (sample.empty() || sample.size() + 3 < sample_size) {
        return std::numeric_limits<double>::quiet_NaN();
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
