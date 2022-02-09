#pragma once
#include <optional>
#include <atomic>
#include <memory>
#include <vector>
#include "cc_api.h"

#define CRLIB_USE_NEW_QUEUE

namespace crlib {
#ifndef CRLIB_USE_NEW_QUEUE
template<typename T>
struct Concurrent_Queue_t {
protected:
	struct Queue_Section_t {
		friend Concurrent_Queue_t;
	protected:
		std::vector<std::optional<T>> data;
		size_t size;
		std::atomic_size_t start;
		std::atomic_size_t read_end;
		std::atomic_size_t write_end;
		std::atomic<Queue_Section_t*> next;

	public:
		Queue_Section_t()  = delete;

		explicit Queue_Section_t(size_t size) : size(size), start(0), write_end(0), read_end(0), next(nullptr), data(size) {
			
		}

		void Push(const T value) {
			size_t e = write_end.load();
			size_t n;

			do {
				if (e >= size) {
					Queue_Section_t* next_section;

					do {
						next_section = next.load();
					} while (next_section == nullptr);

					next_section->Push(value);
					return;
				}
				n = e + 1;
			} while (!write_end.compare_exchange_weak(e, n, std::memory_order_release, std::memory_order_relaxed));
			data[e] = value;

			read_end.fetch_add(1);

			if (e == size / 2) {
				next.store(new Queue_Section_t(size));
			}
		}

		std::optional<T> Pull() {
			size_t s = start.load();

			do {
				size_t e = read_end.load();
				if (s >= e) {
					return std::optional<T>();
				}
			} while (!start.compare_exchange_weak(s, s + 1, std::memory_order_release, std::memory_order_relaxed));

			auto d = data[s];

			if (!d.has_value()) {
				throw std::runtime_error("Concurrent_Queue_t dequeued an empty item");
			}

			data[s] = std::nullopt;

			return std::optional<T>(d.value());
		}

		bool Full() const {
			return write_end.load() >= size;
		}

		bool Completed() const {
			return Full() && start.load() >= size;
		}
	};

	std::atomic<Queue_Section_t*> first_section;
	std::atomic<Queue_Section_t*> last_section;
	size_t chunk_size;

public:
	Concurrent_Queue_t(size_t chunk_size = 64U) : chunk_size(chunk_size), first_section(new Queue_Section_t(chunk_size)) {
		last_section.store(first_section.load());
	}

	void Push(const T item) {
		auto l = last_section.load();
		l->Push(item);
		auto next = l->next.load();

		if (l->Full()) {
			do {
				next = l->next.load();
				if (!l->Full() || next == nullptr)
					return;
			} while (!last_section.compare_exchange_weak(l, next, std::memory_order_release, std::memory_order_relaxed));
		}
	}

	std::optional<T> Pull() {
		auto f = first_section.load();
		std::optional<T> val = f->Pull();
		Queue_Section_t* next;

		do {
			next = f->next.load();
			if ((!f->Completed()) || next == nullptr)
				return val;
		} while (!first_section.compare_exchange_weak(f, next, std::memory_order_release, std::memory_order_relaxed));
		
		delete f;

		return val;
	}
};
#else
template<typename T>
struct Concurrent_Queue_t {
protected:
	struct Queue_Item_t;

#ifdef _MSC_VER
	typedef std::shared_ptr<Queue_Item_t> QueueItemPtr;
#define CRLIB_QUEUE_ITEM_AUTO_DELETE
#else
	typedef Queue_Item_t* QueueItemPtr;
#define CRLIB_QUEUE_ITEM_MANUAL_DELETE
#endif
	struct Queue_Item_t {
		std::optional<T> value;
		QueueItemPtr next;

		Queue_Item_t(const T item) {
			value = item;
			next = nullptr;
		}
	};

protected:
	std::atomic<QueueItemPtr> head;
	std::atomic<QueueItemPtr> tail;
public:
	Concurrent_Queue_t() {
		head.store(nullptr);
		tail.store(nullptr);
	}

	void Push(const T item) {
#ifdef CRLIB_QUEUE_ITEM_MANUAL_DELETE
		auto t = new Queue_Item_t(item);
#else
		auto t = QueueItemPtr(new Queue_Item_t(item));
#endif

		QueueItemPtr last;
		do {
			last = tail.load();
		} while (!tail.compare_exchange_strong(last, t));
		
		if (last != nullptr) {
			last->next = t;
		}

		QueueItemPtr first;
		do {
			first = head.load();
		} while (first == nullptr && !head.compare_exchange_strong(first, t));
	}

	std::optional<T> Pull() {
		QueueItemPtr first;
		do {
			first = head.load();
			if (first == nullptr)
				return std::nullopt;
		} while (!head.compare_exchange_strong(first, first->next));
		std::optional<T> val = first->value;
		
		#ifdef CRLIB_QUEUE_ITEM_MANUAL_DELETE
		delete first;
		#endif
		
		return val;
	}
};
#endif
}