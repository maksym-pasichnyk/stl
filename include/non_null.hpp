#pragma once

#include <cassert>

template <typename T>
struct NonNull {
	constexpr NonNull(decltype(nullptr)) noexcept = delete;
	constexpr NonNull(T* pointer) noexcept : pointer(pointer) {
		assert(pointer != nullptr);
	}

	constexpr auto as_ptr() const noexcept -> T* {
		return pointer;
	}

	template <typename U>
	constexpr auto cast() const noexcept -> NonNull<U> {
		return { static_cast<U*>(static_cast<void*>(as_ptr())) };
	}
	
private:
	T* pointer;
};