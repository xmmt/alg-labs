#pragma once
#include <cassert>
#include <type_traits>
#include <cstring>
#include <iterator>

namespace insertion_sort {

template <typename T>
void sort(T* first, T* last) {
    sort(first, last, [](T const& a, T const& b) { return a < b; });
}

template <typename T, typename Compare>
void sort(T* first, T* last, Compare comp) {
    using std::advance;
    using std::distance;
    using std::next;
    assert(distance(first, last) >= 0 && "wrong pointers: *last less than *first");
    // TODO: static check of Compare type
    if (distance(first, last) < 2) {
        // array of one element or wrong pointers - nothing to do, leave it
        return;
    }
    for (auto* it = next(first, 1); it != last; advance(it, 1)) {
        if constexpr (std::is_trivially_copyable_v<T>) {
            auto current = *it;
            auto* jt = it;
            while (jt != first && comp(current, *(next(jt, -1)))) {
                advance(jt, -1);
            }
            std::memmove(next(jt, 1), jt, distance(jt, it) * sizeof(T));
            *jt = current;
        } else {
            for (auto* jt = it; jt != first && comp(*jt, *(next(jt, -1))); advance(jt, -1)) {
                std::swap(*jt, *(next(jt, -1)));
            }
        }
    }
}

} // namespace insertion_sort