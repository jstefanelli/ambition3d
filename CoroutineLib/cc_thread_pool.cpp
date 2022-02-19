#include "cc_thread_pool.h"


namespace crlib {

thread_local std::shared_ptr<ThreadPool_Thread> ThreadPool::local_thread = nullptr;

ThreadPool::ThreadPool() : running(true), global_tasks(64) {

}

ThreadPool::~ThreadPool() {
	stop();
}

std::shared_ptr<ThreadPool> ThreadPool::build(size_t thread_count) {
	std::shared_ptr<ThreadPool> self_ptr = std::shared_ptr<ThreadPool>(new ThreadPool());
	self_ptr->self_ptr = self_ptr;

	for (int i = 0; i < thread_count; ++i) {
		self_ptr->threads.push_back(std::shared_ptr<ThreadPool_Thread>(new ThreadPool_Thread(self_ptr)));
	}

	for (auto& t : self_ptr->threads) {
		t->start(t);
	}

	return self_ptr;
}
void ThreadPool::submit(std::coroutine_handle<> h) {
	if (local_thread != nullptr && local_thread->thread_pool.get() == this) {
		local_thread->local_tasks.push(h);
		return;
	}

	global_tasks.push(h);
	std::lock_guard lock(task_added_mutex);
	task_added_variable.notify_all();
}

std::optional<std::coroutine_handle<>> ThreadPool::get_work() {
	std::optional<std::coroutine_handle<>> h = std::nullopt;
	do {
		std::coroutine_handle raw_h;
		if (global_tasks.pop(raw_h)) {
			h = raw_h;
		}
		for (size_t i = 0; i < threads.size() && !h.has_value(); i++) {
			if (threads[i]->local_tasks.pop(raw_h)) {
				h = raw_h;
			}
		}

		if (!h.has_value()) {
			auto lock = std::unique_lock<std::mutex>(task_added_mutex);
			task_added_variable.wait_until(lock, std::chrono::system_clock::now() + std::chrono::milliseconds(500));
		}
		else {
			return h;
		}
	} while (!h.has_value() && running);

	return std::nullopt;
}

void ThreadPool::stop() {
	running = false;

	for (auto& t : threads) {
		t->join();
	}

	threads.clear();
}

}