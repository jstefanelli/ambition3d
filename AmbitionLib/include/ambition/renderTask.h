#pragma once

#include <crlib/cc_task.h>
#include <crlib/cc_task_scheduler.h>

namespace ambition {

	template<typename T>
	struct RenderTask_t : public crlib::Task_t<T> {
		typedef crlib::Task_lock_t<T> LockType;
		typedef crlib::TaskAwaiter_t<T> AwaiterType;

        explicit RenderTask_t(std::shared_ptr<LockType> lock) : crlib::Task_t<T>(lock) {

        }

		static RenderTask_t FromResult(T result) {
			auto lock = std::make_shared<crlib::Task_lock_t<T>>();
			lock->set_result(result);
			lock->complete();

			return RenderTask_t(lock);
		}
	};

	struct RenderTaskScheduler : public crlib::BaseTaskScheduler {
	protected:
		static std::shared_ptr<RenderTaskScheduler> instance;

	public:
		crlib::Concurrent_Queue_t<std::coroutine_handle<>> task_queue;

		RenderTaskScheduler() : task_queue(128) {

		}

		virtual void OnTaskSubmitted(std::coroutine_handle<> task) override {
			task_queue.push(task);
		}

		static std::shared_ptr<RenderTaskScheduler>& Instance() {
			if (instance == nullptr) {
				instance = std::make_shared<RenderTaskScheduler>();
			}

			return instance;
		}

		virtual std::string ToString() override {
			return "RenderTaskScheduler";
		}
	};
};


template<typename T, typename ... Args>
struct std::coroutine_traits<ambition::RenderTask_t<T>, Args...> {
	struct promise_type : public crlib::BasePromise<ambition::RenderTask_t<T>, typename std::coroutine_traits<ambition::RenderTask_t<T>, Args...>::promise_type> {
		promise_type() {
			this->scheduler = ambition::RenderTaskScheduler::Instance();
		}

		void return_value(T value) {
			this->lock->set_result(value);
		}
	};
};