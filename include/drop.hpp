#pragma once

template <typename T>
struct Drop {
    static void drop(T*) = delete;
};