#pragma once

#include "stdint.hpp"

#include <utility>

template <typename T>
constexpr auto size_align() noexcept -> std::pair<usize, usize> {
	return std::pair(sizeof(T), alignof(T));
}

struct Layout {
	constexpr Layout(usize size, usize align) noexcept
		: size_(size), align_(align) {}

	constexpr auto size() const noexcept -> usize {
		return size_;
	}

	constexpr auto align() const noexcept -> usize {
		return align_;
	}

	template<typename T>
	static constexpr auto array(usize n) noexcept -> Layout {
		auto [layout, offset] = Layout(sizeof(T), alignof(T)).repeat(n);
		return layout.pad_to_align();
	}

	constexpr auto repeat(usize n) const noexcept -> std::pair<Layout, usize> {
		auto padded_size = size() + padding_needed_for(align());
		auto alloc_size = padded_size * n;
		return std::pair(Layout(alloc_size, align()), padded_size);
	}

	constexpr auto padding_needed_for(usize align) const noexcept -> usize {
		return (size() + align - 1) & !(align - 1);
	}

	constexpr auto pad_to_align() const noexcept -> Layout {
		auto new_size = size() + padding_needed_for(align());
		return Layout(new_size, align());
	}
private:
	usize size_;
	usize align_;
};