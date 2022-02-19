#pragma once
#include <optional>
#include <mutex>
#include <memory>
#include <queue>
#include <assert.h>
#include "cc_api.h"

#ifndef CRLIB_USE_LEGACY_QUEUE
#include <boost/lockfree/queue.hpp>
#endif

namespace crlib {
#ifdef CRLIB_USE_LEGACY_QUEUE
template<typename T>
struct Concurrent_Queue_t {
	std::mutex mutex;
	std::queue<T> internal_queue;

	Concurrent_Queue_t(size_t) {

	}

	void push(const T& val) {
		mutex.lock();
		internal_queue.push(val);
		mutex.unlock();
	}

	bool pop(T& val) {
		bool rvl = false;
		mutex.lock();
		if (!internal_queue.empty()) {
			val = internal_queue.front();
			internal_queue.pop();
			rvl = true;
		}
		mutex.unlock();
		return rvl;
	}
};
#else
	template<typename T>
	using Concurrent_Queue_t = boost::lockfree::queue<T>;
#endif
}