#pragma once
#include <insertion_sort.hpp>

namespace quick_sort {

namespace impl {

// TODO: determine best size for insertion sort
constexpr size_t k_insertion_threshold = 15;

template <typename T>
struct default_comparator {
    inline bool operator()(T const& a, T const& b) {
        return a < b;
    }
};

template <typename T, typename Compare>
T* select_pivot(T* first, T* last, Compare comp) {
    // TODO: choose better pivot
    return first;
}

template <typename T, typename Compare>
T* do_partition(T* first, T* last, T* pivot, Compare comp) {
    using std::advance;
    using std::distance;
    assert(distance(first, pivot) >= 0 && distance(pivot, last) > 0 && "Wrong pointers: *pivot must be between *first and *last");
    while (distance(first, last) > 0) {
        while (comp(*pivot, *first)) {
            advance(first, 1);
        }
        while (comp(*last, *pivot)) {
            advance(last, -1);
        }
        if (distance(first, last) > 0) {
            std::swap(*first, *last);
        }
    }
    return last;
}

template <typename T>
T* do_partition(T* first, T* last, T* pivot) {
    return do_partition(first, last, pivot, default_comparator<T>());
}

} // namespace impl

template <typename T, typename Compare>
void sort(T* first, T* last, Compare comp) {
    using std::distance;
    assert(distance(first, last) >= 0 && "wrong pointers: *last less than *first");
    // TODO: static check of Compare type
    if (distance(first, last) < 2) {
        // array of one element or wrong pointers - nothing to do, leave it
        return;
    }
    if (static_cast<size_t>(distance(first, last)) < impl::k_insertion_threshold) {
        insertion_sort::sort(first, last, comp);
        return;
    }
    T* pivot = impl::select_pivot(first, last, comp);
    pivot = impl::do_partition(first, last, pivot, comp);
}

template <typename T>
void sort(T* first, T* last) {
    sort(first, last, impl::default_comparator<T>());
}

} // namespace quick_sort