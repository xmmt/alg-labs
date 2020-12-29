#pragma once

#include <string>
#include <cstdarg>
#include <random>

namespace helpers {

template <typename T>
struct trivially_copyable_struct {
    T x{};

    trivially_copyable_struct() = default;
    trivially_copyable_struct(trivially_copyable_struct const&) = default;
    trivially_copyable_struct(trivially_copyable_struct&&) = default;
    trivially_copyable_struct& operator=(trivially_copyable_struct const&) = default;
    trivially_copyable_struct& operator=(trivially_copyable_struct&&) = default;
    ~trivially_copyable_struct() = default; // virtual destructor disables is_trivially_copyable trait

    trivially_copyable_struct(T const& n)
        : x(n) {
    }

    auto operator<=>(trivially_copyable_struct const&) const = default;

    [[nodiscard]] T data() const {
        return x;
    }
};

template <typename T>
struct non_trivially_copyable_struct {
    T x{};

    non_trivially_copyable_struct() = default;
    non_trivially_copyable_struct(non_trivially_copyable_struct const&) = default;
    non_trivially_copyable_struct(non_trivially_copyable_struct&&) = default;
    non_trivially_copyable_struct& operator=(non_trivially_copyable_struct const&) = default;
    non_trivially_copyable_struct& operator=(non_trivially_copyable_struct&&) = default;
    virtual ~non_trivially_copyable_struct() = default; // virtual destructor disables is_trivially_copyable trait

    non_trivially_copyable_struct(T const& n)
        : x(n) {
    }

    auto operator<=>(non_trivially_copyable_struct const&) const = default;

    [[nodiscard]] T data() const {
        return x;
    }
};

template <typename T>
concept has_data_getter = requires(T a) {
    a->data();
};

template <typename T>
auto get_data(T const pointer) {
    if constexpr (has_data_getter<T>) {
        return pointer->data();
    } else {
        return *pointer;
    }
}

template <typename T>
std::string serialize_array(T first, T last) {
    if (std::distance(first, last) <= 0) {
        return "[]";
    }
    std::string result;
    result += "[";
    while (std::next(first, 1) != last) {
        result += std::to_string(get_data(first)) + ", ";
        std::advance(first, 1);
    }
    result += std::to_string(get_data(first)) + "]";
    return result;
};

} // namespace helpers

template <typename T, typename Compare>
bool is_sorted(T* first, T* last, Compare comp) {
    auto next = std::next(first);
    while (next != last) {
        if (caom(*next, *first)) {
            return false;
        }
    }
    return true;
}

template <typename T>
bool is_sorted(T* first, T* last) {
    return is_sorted(first, last, std::less());
}

template <class RandomIt>
void random_shuffle(RandomIt first, RandomIt last) {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(first, last, g);
}