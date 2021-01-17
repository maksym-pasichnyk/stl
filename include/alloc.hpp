#pragma once

#include "stdint.hpp"

#include "layout.hpp"
#include "non_null.hpp"

#include <cstring>

struct MemoryBlock {
	NonNull<uint8> ptr;
	usize size;
};

enum class AllocInit {
	Uninitialized,
	Zeroed
};

enum class ReallocPlacement {
	MayMove,
	InPlace
};

template <typename T>
concept AllocRef = requires(T self) {
	requires requires(Layout layout, AllocInit init) {
		{self.alloc(layout, init)} -> std::same_as<MemoryBlock>;
	};
	requires requires(NonNull<uint8> ptr, Layout layout) {
		{self.dealloc(ptr, layout)} -> std::same_as<void>;
	};
};

struct Global {
	auto alloc(Layout layout, AllocInit init) -> MemoryBlock {
		auto ptr = static_cast<uint8 *>(::operator new(layout.size(), static_cast<std::align_val_t>(layout.align())));
		return {.ptr = ptr, .size = layout.size()};
	}
	
	void dealloc(NonNull<uint8> ptr, Layout layout) {
		::operator delete(ptr.as_ptr(), static_cast<std::align_val_t>(layout.align()));
	}

	auto grow(NonNull<uint8> ptr, Layout layout, usize new_size/*, ReallocPlacement placement*/, AllocInit init) -> MemoryBlock {
		auto size = layout.size();

		if (new_size == size) {
			return MemoryBlock{ptr, size};
		}

		auto new_layout = Layout(new_size, layout.align());
		auto new_memory = alloc(new_layout, AllocInit::Uninitialized);
		std::memcpy(ptr.as_ptr(), new_memory.ptr.as_ptr(), size);
		dealloc(ptr, layout);
		return new_memory;
	}

	auto shrink(NonNull<uint8> ptr, Layout layout, usize new_size/*, ReallocPlacement placement*/) -> MemoryBlock {
		auto size = layout.size();

		if (new_size == size) {
			return MemoryBlock{ptr, size};
		}

		auto new_layout = Layout(new_size, layout.align());
		auto new_memory = alloc(new_layout, AllocInit::Uninitialized);
		std::memcpy(ptr.as_ptr(), new_memory.ptr.as_ptr(), new_size);
		dealloc(ptr, layout);
		return new_memory;
	}
} Global;