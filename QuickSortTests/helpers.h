#pragma once

#include <string>

namespace helpers {

template <typename T>
struct TriviallyCopyableStruct {
#if Self
#    error "Ooops"
#endif
#define Self TriviallyCopyableStruct

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

    T getData() const {
        return x;
    }
};

template <typename T>
struct NonTriviallyCopyableStruct {
#if Self
#    error "Ooops"
#endif
#define Self NonTriviallyCopyableStruct

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

    T getData() const {
        return x;
    }
};

template <typename T, typename = void>
struct dataGetter {
    constexpr T operator()(T const* pointer) {
        return *pointer;
    }
};

template <typename T>
struct dataGetter<T, std::void_t<decltype(std::declval<T>().getData())>> {
    constexpr decltype(std::declval<T>().getData()) operator()(T const* pointer) {
        return pointer->getData();
    }
};

template <typename T>
constexpr auto getData(T const* pointer) -> decltype(std::declval<dataGetter<T>>()(pointer)) {
    return dataGetter<T>()(pointer);
}

template <typename T>
std::string serializeArray(T* first, T* last) {
    if (std::distance(first, last) <= 0) {
        return "[]";
    }
    std::string result;
    result += "[";
    while (std::next(first, 1) != last) {
        result += std::to_string(getData(first)) + ", ";
        std::advance(first, 1);
    }
    result += std::to_string(getData(first)) + "]";
    return result;
};

} // namespace helpers