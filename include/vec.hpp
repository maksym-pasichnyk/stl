#pragma once

#include "raw_vec.hpp"

#include <memory>

template<typename T, AllocRef A = struct Global>
struct Vec {
	static auto with_capacity(usize capacity) -> Vec {
        return Vec(RawVec<T, A>::with_capacity(capacity), 0);
    }

	template<typename... Args>
	void emplace(Args&&... args) {
		if (len == buf.capacity()) {
			reserve(1);
		}
		std::construct_at(as_ptr() + len, std::forward<Args>(args)...);
		len += 1;
	}

	void reserve(usize additional) {
		buf.reserve(len, additional);
	}

	T* as_ptr() {
		return buf.ptr();
	}
private:
	Vec(RawVec<T, A> buf, usize len) : buf(buf), len(len) {}

	RawVec<T, A> buf;
	usize len;
};
