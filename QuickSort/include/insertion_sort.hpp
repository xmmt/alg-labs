#pragma once

#include <cassert>
#include <type_traits>
#include <cstring>
#include <iterator>

namespace insertion_sort {

template <typename T, typename Compare>
void sort(T first, T last, Compare comp) {
    using std::advance;
    using std::distance;
    using std::next;
    using std::prev;
    assert(distance(first, last) >= 0 && "wrong pointers: *last less than *first");
    // TODO: static check of Compare type
    if (distance(first, last) < 2) {
        // array of one element or wrong pointers - nothing to do, leave it
        return;
    }
    for (auto it = next(first); it != last; advance(it, 1)) {
        //if constexpr (std::contiguous_iterator<T> && std::is_trivially_copyable_v<std::iter_value_t<T>>) {
        //    auto current = *it;
        //    auto jt = it;
        //    while (jt != first && comp(current, *(prev(jt)))) {
        //        advance(jt, -1);
        //    }
        //    std::memmove(&*(next(jt)), &*jt, distance(jt, it) * sizeof(std::iter_value_t<T>));
        //    *jt = current;
        //} else {
        //    for (auto jt = it; jt != first && comp(*jt, *(prev(jt))); advance(jt, -1)) {
        //        std::swap(*jt, *(prev(jt)));
        //    }
        //}

        //auto current = *it;
        //auto jt = it;
        //while (jt != first && comp(current, *(prev(jt)))) {
        //    advance(jt, -1);
        //}
        //std::copy_backward(jt, it, next(it));
        //*jt = current;

        //if constexpr (std::contiguous_iterator<T> && std::is_trivially_copyable_v<std::iter_value_t<T>>) {
        for (auto jt = it; jt != first && comp(*jt, *(prev(jt))); advance(jt, -1)) {
            std::swap(*jt, *(prev(jt)));
        }
        //} else {
        //    auto current = *it;
        //    auto jt = it;
        //    while (jt != first && comp(current, *(prev(jt)))) {
        //        advance(jt, -1);
        //    }
        //    std::copy_backward(jt, it, next(it));
        //    *jt = current;
        //}
    }
}

template <typename T>
void sort(T first, T last) {
    insertion_sort::sort(first, last, std::less<std::iter_value_t<T>>());
}

} // namespace insertion_sort