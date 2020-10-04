#pragma once
#include <cassert>

namespace insertion_sort {

template <typename T>
void sort(T* first, T* last) {
    sort(first, last, [](T const& a, T const& b) { return a < b; });
}

template <typename T, typename Compare>
void sort(T* first, T* last, Compare comp) {
    assert(last >= first && "wrong pointers: *last less than *first");
    // TODO: static check of Compare type
    if (last - first < 2) {
        // array of one element or wrong pointers - nothing to do, leave it
        return;
    }
    // TODO: insertion sort
}

} // namespace insertion_sort