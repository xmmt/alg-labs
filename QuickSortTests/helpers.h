#pragma once

#include <string>
#include <cstdarg>
#include <random>

namespace helpers {

template <typename T>
struct trivially_copyable_struct {
#if Self
#    error "Ooops"
#endif
#define Self trivially_copyable_struct

    T x{};

    Self() = default;
    Self(const Self&) = default;
    Self(Self&&) = default;
    Self& operator=(const Self&) = default;
    Self& operator=(Self&&) = default;
    ~Self() = default; // virtual destructor disables is_trivially_copyable trait

    Self(T const& n)
        : x(n) {
    }

    auto operator<=>(Self const&) const = default;

#undef Self

    [[nodiscard]] T data() const {
        return x;
    }
};

template <typename T>
struct non_trivially_copyable_struct {
#if Self
#    error "Ooops"
#endif
#define Self non_trivially_copyable_struct

    T x{};

    Self() = default;
    Self(const Self&) = default;
    Self(Self&&) = default;
    Self& operator=(const Self&) = default;
    Self& operator=(Self&&) = default;
    virtual ~Self() = default; // virtual destructor disables is_trivially_copyable trait

    Self(T const& n)
        : x(n) {
    }

    auto operator<=>(Self const&) const = default;

#undef Self

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

template <class RandomIt, class URBG>
void shuffle(RandomIt first, RandomIt last, URBG&& g) {
    typedef typename std::iterator_traits<RandomIt>::difference_type diff_t;
    typedef std::uniform_int_distribution<diff_t> distr_t;
    typedef typename distr_t::param_type param_t;

    distr_t D;
    diff_t n = last - first;
    for (diff_t i = n - 1; i > 0; --i) {
        using std::swap;
        swap(first[i], first[D(g, param_t(0, i))]);
    }
}

template <class RandomIt>
void random_shuffle(RandomIt first, RandomIt last) {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(first, last, g);
}