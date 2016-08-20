#include "perfmon/thread_specific_counters.h"
#include "counters.h"
#include <algorithm>
#include <vector>

#if !PERFMON_CONFIG__COMPILER_CXX_THREAD_LOCAL
#include <boost/thread/tss.hpp>
#endif

namespace perfmon {
namespace internal {
namespace {

class TssCountersHolder {
public:
    static TssCounters Expand()
    {
        const auto guard = GlobalLockGuard();
        auto& instance = UnsafeGetInstance();
        instance.counters_.resize((UnsafeNumberOfCounters() / 16 + 1) * 16, TssCounter{0, 0});
        return TssCounters{ instance.counters_.size(), &instance.counters_[0] };
    }

    static void UnsafeFlushAll()  // Must be protected by the global mutex!
    {
        for (auto* it = global_head; it; it = it->next_) {
            it->UnsafeFlush();
        }
    }

    ~TssCountersHolder()
    {
        const auto guard = GlobalLockGuard();
        if (next_) {
            next_->prev_ = prev_;
        }
        if (prev_) {
            prev_->next_ = next_;
        }
        if (global_head == this) {
            global_head = next_;
        }
        const size_t size = std::min(counters_.size(), UnsafeNumberOfCounters());
        for (size_t i = 0; i < size; ++i) {
            UnsafeCommit(i, counters_[i].calls, counters_[i].ticks);
        }
    }

private:
    static TssCountersHolder& UnsafeGetInstance();  // Must be protected by the global mutex!

    TssCountersHolder()  // Must be protected by the global mutex!
    {
        next_ = global_head;
        prev_ = nullptr;
        if (global_head) {
            global_head->prev_ = this;
        }
        global_head = this;
    }

    void UnsafeFlush()  // Must be protected by the global mutex!
    {
        const size_t size = std::min(counters_.size(), UnsafeNumberOfCounters());
        for (size_t i = 0; i < size; ++i) {
            UnsafeAccumulate(
                i,
                counters_[i].calls,
                counters_[i].ticks);
        }
    }

    std::vector<TssCounter> counters_;
    TssCountersHolder* next_;
    TssCountersHolder* prev_;
    static TssCountersHolder* global_head;
};

TssCountersHolder* TssCountersHolder::global_head;


#if PERFMON_CONFIG__COMPILER_CXX_THREAD_LOCAL

TssCountersHolder& TssCountersHolder::UnsafeGetInstance()
{
    static thread_local TssCountersHolder instance;
    return instance;
}

#else // PERFMON_CONFIG__COMPILER_CXX_THREAD_LOCAL

TssCountersHolder& TssCountersHolder::UnsafeGetInstance()
{
    static PERFMON_THREAD_SPECIFIC TssCountersHolder* instance;
    if (!instance) {
        static boost::thread_specific_ptr<TssCountersHolder> tss_ptr;
        tss_ptr.reset(new TssCountersHolder);
        instance = tss_ptr.get();
    }
    return *instance;
}

#endif // PERFMON_CONFIG__COMPILER_CXX_THREAD_LOCAL

} // namespace

PERFMON_THREAD_SPECIFIC TssCounters global_tss_counters;


void ExpandTssCounters()
{
    global_tss_counters = TssCountersHolder::Expand();
}

void UnsafeFlushTssCounters()
{
    TssCountersHolder::UnsafeFlushAll();
}

} } // namespace perfmon::internal
