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
    static TssCountersHolder& GetInstance();

    TssCountersHolder()
    {
        next_ = head;
        prev_ = nullptr;
        if (head) {
            head->prev_ = this;
        }
        head = this;
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
        if (head == this) {
            head = next_;
        }
        const size_t size = std::min(counters_.size(), UnsafeNumberOfCounters());
        for (size_t i = 0; i < size; ++i) {
            UnsafeCommit(i, counters_[i].calls, counters_[i].ticks);
        }
    }

    TssCounters UnsafeExpand()
    {
        counters_.resize((UnsafeNumberOfCounters() / 16 + 1) * 16, TssCounter{0, 0});
        return TssCounters{ counters_.size(), &counters_[0] };
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

    static void UnsafeFlushAll()  // Must be protected by the global mutex!
    {
        UnsafeResetAccumulators();
        auto* item = head;
        while (item) {
            item->UnsafeFlush();
            item = item->next_;
        }
    }

private:
    std::vector<TssCounter> counters_;
    TssCountersHolder* next_;
    TssCountersHolder* prev_;
    static TssCountersHolder* head;
};


TssCountersHolder* TssCountersHolder::head;


#if PERFMON_CONFIG__COMPILER_CXX_THREAD_LOCAL

TssCountersHolder& TssCountersHolder::GetInstance()
{
    static thread_local TssCountersHolder instance;
    return instance;
}

#else // PERFMON_CONFIG__COMPILER_CXX_THREAD_LOCAL

TssCountersHolder& TssCountersHolder::GetInstance()
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


extern RERFMON_THREAD_SPECIFIC TssCounters global_tss_counters = {0, nullptr};


void ExpandTssCounters()
{
    const auto guard = GlobalLockGuard();
    global_tss_counters = TssCountersHolder::GetInstance().UnsafeExpand();
}

void UnsafeFlushTssCounters()
{
    TssCountersHolder::UnsafeFlushAll();
}

} } // namespace perfmon::internal
