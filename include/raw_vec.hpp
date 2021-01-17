#pragma once

#include "stdint.hpp"
#include "unique.hpp"
#include "alloc.hpp"

#include <algorithm>

template<typename T, AllocRef A = struct Global>
struct RawVec {
	static auto with_capacity(usize capacity) -> RawVec {
		return with_capacity_in(capacity, Global);
	}

	static auto with_capacity_in(usize capacity, A& alloc) -> RawVec {
		return allocate_in(capacity, AllocInit::Uninitialized, alloc);
	}

	static auto allocate_in(usize capacity, AllocInit init, A& alloc) -> RawVec {
		auto layout = Layout::array<T>(capacity);
		auto memory = alloc.alloc(layout, init);

		return RawVec(
			Unique<T>::new_unchecked(memory.ptr.template cast<T>().as_ptr()),
			capacity_from_bytes(memory.size),
			alloc
		);
	}

	static auto capacity_from_bytes(usize excess) -> usize {
		return excess / sizeof(T);
	}

	void reserve(usize used_capacity, usize needed_extra_capacity) {

	}

	auto try_reserve(usize used_capacity, usize needed_extra_capacity) -> bool {
		if (needs_to_grow(used_capacity, needed_extra_capacity)) {
			return grow_amortized(used_capacity, needed_extra_capacity, ReallocPlacement::MayMove);
		}
		return true;
	}

	auto grow_amortized(usize used_capacity, usize needed_extra_capacity, ReallocPlacement placement) -> bool {
		auto required_cap = used_capacity + needed_extra_capacity;
		auto new_cap = std::max(cap * 2, required_cap);

		auto min_non_zero_cap = [] {
			if constexpr (sizeof(T) == 1) {
				return 1;
			} else if constexpr (sizeof(T) <= 1024) {
				return 4;
			} else {
				return 8;
			}
		}();
		auto new_layout = Layout::array<T>(std::max(min_non_zero_cap, new_cap));
		auto memory = finish_grow(new_layout, placement, current_memory(), *alloc);
	}

	auto needs_to_grow(usize used_capacity, usize needed_extra_capacity) -> bool {

	}
 
	auto current_memory() -> std::pair<NonNull<uint8>, Layout> {
		return std::pair(NonNull(ptr.template cast<T>().as_ptr()), Layout(sizeof(T), alignof(T)));
	}

	auto capacity() const noexcept -> usize {
		return cap;
	}

	auto as_ptr() const noexcept -> T* {
		return ptr.as_ptr();
	}

private:
	RawVec(Unique<T> ptr, usize cap, A alloc)
		: ptr(ptr)
		, cap(cap)
		, alloc(alloc) {}

	Unique<T> ptr;
	usize cap;
	A alloc;
};
