#pragma once
#include <semaphore>
#include <coroutine>
#include <optional>
#include <functional>
#include "cc_api.h"
#include "cc_queue.h"
#include "cc_task_scheduler.h"
#include <type_traits>
#include <concepts>
#include <iostream>

namespace crlib {

struct Base_Task_lock {
	bool completed;
	std::optional<std::exception_ptr> exception;

	Concurrent_Queue_t<std::function<void()>> waiting_coroutines;
	std::binary_semaphore wait_semaphore;

	Base_Task_lock() : completed(false), wait_semaphore(0)  {

	}

	void complete() {
		completed = true;
		wait_semaphore.release();
		std::optional<std::function<void()>> h;
		do {
			h = waiting_coroutines.Pull();
			if (h.has_value()) {
				h.value()();
			}

		} while (h.has_value());
	}
};

struct Task_lock : public Base_Task_lock {

	Task_lock() {

	}

	void wait() {
		if (!completed) {
			wait_semaphore.acquire();
		}

		if (exception.has_value()) {
			std::rethrow_exception(exception.value());
		}
	}
};

template<typename T>
struct Task_lock_t : public Base_Task_lock {
	std::optional<T> returnValue;

	Task_lock_t() : returnValue() {

	}

	T wait() {
		if (!completed) {
			wait_semaphore.acquire();
		}

		if (exception.has_value()) {
			std::rethrow_exception(exception.value());
		}

		if (!returnValue.has_value()) {
			throw std::runtime_error("Task_t<T> ended with no result provided");
		}
		return returnValue.value();
	}

	void set_result(T& result) {
		returnValue = result;
	}
};

template<typename LockType>
struct BaseTaskAwaiter {
	std::shared_ptr<LockType> lock;

	BaseTaskAwaiter(std::shared_ptr<LockType> lock) :
		lock(lock) {

	}

	bool await_ready() {
		//TODO: Check for threadpool thread
		//return lock->completed;
		return false;
	}

	template<typename PromiseType>
	void await_suspend(std::coroutine_handle<PromiseType> h) {
		if (!lock->completed) {
			lock->waiting_coroutines.Push([h]() {
				auto sch = h.promise().scheduler;
				std::cout << "[BaseAwaiter] Scheduling on: " << (sch == nullptr ? "default" : sch->ToString()) << std::endl;
				BaseTaskScheduler::Schedule(h, sch);
				});
		}
		else {
			auto sch = h.promise().scheduler;
			std::cout << "[BaseAwaiter] Scheduling on: " << (sch == nullptr ? "default" : sch->ToString()) << std::endl;
			BaseTaskScheduler::Schedule(h, sch);
		}
	}

};

struct TaskAwaiter : public BaseTaskAwaiter<Task_lock> {
	
	void await_resume() {
		if (this->lock->exception.has_value()) {
			std::rethrow_exception(this->lock->exception.value());
		}
	}
};

template<typename T>
struct TaskAwaiter_t : public BaseTaskAwaiter<Task_lock_t<T>> {
	T await_resume() {
		if (this->lock->exception.has_value()) {
			std::rethrow_exception(this->lock->exception.value());
		}

		if (!this->lock->returnValue.has_value()) {
			throw std::runtime_error("Task_t<T> ended with no result provided");
		}

		return this->lock->returnValue.value();
	}
};

struct AggregateException : public std::runtime_error {
	std::vector<std::exception_ptr> exceptions;

	AggregateException(std::vector<std::exception_ptr> exceptions) : std::runtime_error("Aggregate exceptions from tasks"), exceptions(exceptions) {

	}
};

template<typename T>
struct MultiTaskAwaiter {
	struct MultiTaskAwaiter_ctrl {
		std::vector<std::shared_ptr<typename T::LockType>> task_locks;
		size_t tasks_count;
		std::atomic_size_t completed_tasks;

		MultiTaskAwaiter_ctrl() : tasks_count(0) {

		}
	};

	std::shared_ptr<MultiTaskAwaiter_ctrl> ctrl_block;

	MultiTaskAwaiter(std::vector<std::shared_ptr<typename T::LockType>> locks) :
		ctrl_block(std::shared_ptr<MultiTaskAwaiter_ctrl>(new MultiTaskAwaiter_ctrl())) 
	{
		ctrl_block->tasks_count = locks.size();
		
		for (auto& l : locks) {
			ctrl_block->task_locks.push_back(l);
		}
	}

	bool await_ready() {
		for(auto& t : ctrl_block->task_locks) {
			if (!t->completed) {
				return false;
			}
		}

		return true;
	}

	template<typename PromiseType>
	void increase_and_schedule(std::coroutine_handle<PromiseType> h) {
		size_t old = ctrl_block->completed_tasks.fetch_add(1);

		if (old == ctrl_block->tasks_count - 1) {
			auto sch = h.promise().scheduler;
			std::cout << "[MultiAwaiter] Scheduling on: " << (sch == nullptr ? "default" : sch->ToString()) << std::endl;
			BaseTaskScheduler::Schedule(h, sch);
		}
	}

	template<typename PromiseType>
	void await_suspend(std::coroutine_handle<PromiseType> h) {
		for (auto& t : ctrl_block->task_locks) {
			if (t->completed) {
				increase_and_schedule(h);
			} else {
				t->waiting_coroutines.Push([this, h] () {
					increase_and_schedule(h);
				});
			}
		}
	}

	void await_resume() {
		std::vector<std::exception_ptr> exceptions;

		for (auto& t : ctrl_block->task_locks) {
			if (t->exception.has_value()) {
				exceptions.push_back(t->exception.value());
			}
		}

		if (exceptions.size() == 1) {
			std::rethrow_exception(exceptions[0]);
		} else if(exceptions.size() > 1) {
			throw AggregateException(std::move(exceptions));
		}
	}
};

template<typename PromiseType>
struct TaskAwaitable {
	bool await_ready() {
		return false;
	}

	void await_suspend(std::coroutine_handle<PromiseType> h) {
		BaseTaskScheduler::Schedule(h, h.promise().scheduler);
	}

	void await_resume() {

	}
};

struct Task {
public:
	typedef crlib::Task_lock LockType;
	typedef crlib::TaskAwaiter AwaiterType;
	std::shared_ptr<LockType> lock;

	Task() = delete;
	Task(std::shared_ptr<LockType> lock) 
		: lock(lock) {

	}


	Task(const Task& other) : lock(other.lock) {

	}

	void wait() {
		lock->wait();
	}
};

template<typename T>
struct Task_t {
public:
	typedef crlib::Task_lock_t<T> LockType;
	typedef crlib::TaskAwaiter_t<T> AwaiterType;
	std::shared_ptr<LockType> lock;

	Task_t() = delete;
	Task_t(std::shared_ptr<Task_lock_t<T>> lock)
		: lock(lock) {

	}


	Task_t(const Task_t& other) : lock(other.lock) {

	}

	T wait() {
		return lock->wait();
	}
};

template<typename ... TS>
MultiTaskAwaiter<Task> WhenAll(const TS&... tasks) {
	std::vector<Task> task_vector {{ tasks... }};

	std::vector<std::shared_ptr<Task_lock>> locks;

	for (auto& t : task_vector) {
		locks.push_back(t.lock);
	}

	return MultiTaskAwaiter<Task>(locks);
};

template<typename T>
MultiTaskAwaiter<T> WhenAll(const std::vector<T>& tasks) {
	std::vector<std::shared_ptr<typename T::LockType>> locks;

	for (auto& t : tasks) {
		locks.push_back(t.lock);
	}
	
	return MultiTaskAwaiter<T>(locks);
}

template<typename TaskType, typename PromiseType>
struct BasePromise {
	std::shared_ptr<typename TaskType::LockType> lock;
	std::shared_ptr<crlib::BaseTaskScheduler> scheduler;

	BasePromise() : lock(new typename TaskType::LockType()) {

	}

	crlib::TaskAwaitable<PromiseType> initial_suspend() {
		return {};
	}

	TaskType get_return_object() {
		return TaskType(lock);
	}

	template<typename TType>
	TType::AwaiterType await_transform(TType task) {
		return typename TType::AwaiterType(task.lock);
	}

	template<typename T>
	crlib::MultiTaskAwaiter<T> await_transform(crlib::MultiTaskAwaiter<T> awaiter) {
		return awaiter;
	}

	std::suspend_never final_suspend() noexcept {
		lock->complete();
		return {};
	}

	void unhandled_exception() {
		lock->exception = std::current_exception();
	}
};

}

template<typename ... Args>
struct std::coroutine_traits<crlib::Task, Args...> {
	struct promise_type : public crlib::BasePromise<crlib::Task, typename std::coroutine_traits<crlib::Task, Args...>::promise_type> {
		void return_void() {}
	};
};

template<typename T, typename ... Args>
struct std::coroutine_traits<crlib::Task_t<T>, Args...> {
	struct promise_type : public crlib::BasePromise<crlib::Task_t<T>, typename std::coroutine_traits<crlib::Task_t<T>, Args...>::promise_type> {
		void return_value(T val) {
			this->lock->set_result(val);
		}
	};
};

