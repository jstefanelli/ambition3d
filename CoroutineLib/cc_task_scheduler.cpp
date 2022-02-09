#include "cc_task_scheduler.h"

namespace crlib {

CRLIB_API std::shared_ptr<BaseTaskScheduler> BaseTaskScheduler::default_task_scheduler = nullptr;
thread_local std::shared_ptr<BaseTaskScheduler> BaseTaskScheduler::current_scheduler = nullptr;

CRLIB_API BaseTaskScheduler::BaseTaskScheduler() {

}

CRLIB_API void BaseTaskScheduler::Schedule(std::coroutine_handle<> handle, std::shared_ptr<BaseTaskScheduler> target_scheduler) {
    if (target_scheduler != nullptr) {
        target_scheduler->OnTaskSubmitted(handle);
        return;
    }

    if (current_scheduler != nullptr) {
        current_scheduler->OnTaskSubmitted(handle);
    } else {
        if (default_task_scheduler == nullptr) {
            default_task_scheduler = std::make_shared<ThreadPoolTaskScheduler>();
        }
        default_task_scheduler->OnTaskSubmitted(handle);
    }
}

CRLIB_API ThreadPoolTaskScheduler::ThreadPoolTaskScheduler() : ThreadPoolTaskScheduler(CRLIB_DEFAULT_THREAD_POOL_THREADS) {

}

CRLIB_API ThreadPoolTaskScheduler::ThreadPoolTaskScheduler(size_t thread_amount) {
    thread_pool = ThreadPool::build(thread_amount);
}

CRLIB_API void ThreadPoolTaskScheduler::OnTaskSubmitted(std::coroutine_handle<> handle) {
    thread_pool->submit(handle);
}

CRLIB_API std::string ThreadPoolTaskScheduler::ToString() {
    return "ThreadPoolTaskScheduler";
}

}