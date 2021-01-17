#pragma once

//todo: replace with Option
#include <optional>
#include <drop.hpp>

template <typename T/*, typename Deleter = Drop<T>*/>
struct Unique {
    Unique(const Unique&) = delete;
    auto operator=(const Unique&) -> Unique& = delete;

//    constexpr Unique(Unique&& other) noexcept : pointer(std::exchange(other.pointer, nullptr)) {}
//    constexpr auto operator=(Unique&& other) noexcept -> Unique& {
//        pointer = std::exchange(other.pointer, nullptr);
//        return *this;
//    }

//    ~Unique() {
//        if (pointer != nullptr) {
//            Deleter::drop(pointer);
//            pointer = nullptr;
//        }
//    }

	static constexpr auto from(T* pointer) noexcept -> std::optional<Unique> {
		return pointer ? std::optional{Unique(pointer)} : std::nullopt;
	}
	
	static constexpr auto new_unchecked(T* pointer) noexcept -> Unique {
		return Unique(pointer);
	}

	constexpr auto as_ptr() const noexcept -> T* {
		return pointer;
	}

	template <typename U>
	constexpr auto cast() const -> Unique<U> {
		return { static_cast<U*>(static_cast<void*>(pointer)) };
	}
private:
	constexpr explicit Unique(T* pointer) noexcept : pointer(pointer) {}
	
	T* pointer;
};