#pragma once
#include <cassert>

namespace QuickSort {
    namespace Private
    {
        const size_t k_insertion_threshold = 15;
    } // namespace Private

    template<typename T>
    void sort(T* first, T* last)
    {
        sort(first, last, [](T const& a, T const& b) { return a < b; });
    }

    template<typename T, typename Compare>
    void sort(T* first, T* last, Compare comp)
    {
        assert(last >= first && "wrong pointers: *last less than *first");
        // TODO: static check of Compare type
        if (last - first < 2)
        {
            // array of one element or wrong pointers - nothing to do, leave it
            return;
        }
        if (static_cast<size_t>(last - first) < Private::k_insertion_threshold)
        {
            // TODO: insertion sort here
        }
        // TODO: quick sort
    }

} // namespace QuickSort