#pragma once

#include <insertion_sort.hpp>

namespace quick_sort {

namespace impl {

#ifdef TESTING
extern size_t k_insertion_threshold;
#else
// TODO: determine best size for insertion sort
constexpr size_t k_insertion_threshold{ 0 };
#endif

//template <typename T, typename Compare>
//auto select_pivot(T first, T last, Compare comp) {
//    auto swap_if_less = [comp](auto& a, auto& b) {
//        if (comp(a, b)) {
//            std::swap(a, b);
//        }
//    };
//    auto middle = std::next(first, std::distance(first, last) / 2);
//    std::advance(last, -1);
//    swap_if_less(*middle, *first);
//    swap_if_less(*last, *first);
//    swap_if_less(*middle, *last);
//    return last;
//}

template <typename T, typename Compare>
auto select_pivot(T first, T last, Compare comp) {
    std::advance(last, -1);
    auto middle = std::next(first, std::distance(first, last) / 2);
    if (comp(*first, *middle)) {
        if (comp(*middle, *last)) {
            return middle;
        } else if (comp(*first, *last)) {
            return last;
        } else {
            return first;
        }
    } else if (comp(*first, *last)) {
        return first;
    } else if (comp(*middle, *last)) {
        return last;
    } else {
        return middle;
    }
}

/// <summary>
/// Lomuto partition
/// </summary>
/// <typeparam name="T">Value type</typeparam>
/// <typeparam name="Compare">Comparator function type</typeparam>
/// <param name="first">Iterator to begin of range</param>
/// <param name="last">Iterator to end of range</param>
/// <param name="comp">Comparator - function of two arguments that returns bool</param>
/// <returns>Iterator of partition boundary</returns>
//template <typename T, typename Compare>
//auto do_partition(T first, T last, Compare comp) {
//    using std::advance;
//    using std::distance;
//    using std::prev;
//    auto pivot = select_pivot(first, last, comp);
//    if (pivot != prev(last)) {
//        std::swap(*pivot, *prev(last));
//        pivot = prev(last);
//    }
//    auto i = first;
//    for (auto j = first; j != pivot; advance(j, 1)) {
//        // bool format
//        if (comp(*j, *pivot)) {
//            std::swap(*i, *j);
//            advance(i, 1);
//        }
//    }
//    std::swap(*i, *pivot);
//    return i;
//}

/// <summary>
/// Hoare partition
/// </summary>
/// <typeparam name="T">Value type</typeparam>
/// <typeparam name="Compare">Comparator function type</typeparam>
/// <param name="first">Iterator to begin of range</param>
/// <param name="last">Iterator to end of range</param>
/// <param name="comp">Comparator - function of two arguments that returns bool</param>
/// <returns>Iterator of partition boundary</returns>
template <typename T, typename Compare>
auto do_partition(T first, T last, Compare comp) {
    using std::advance;
    using std::distance;
    using std::next;
    using std::prev;
    using std::swap;
    auto pivot = *select_pivot(first, last, comp);
    advance(last, -1);
    while (distance(first, last) >= 0) {
        while (comp(*first, pivot)) {
            advance(first, 1);
        }
        while (comp(pivot, *last)) {
            advance(last, -1);
        }
        if (distance(first, last) <= 0) {
            break;
        }
        swap(*first, *last);
        advance(first, 1);
        advance(last, -1);
    }
    return next(last);
}

template <typename T>
auto do_partition(T first, T last) {
    return do_partition(first, last, std::less<std::iter_value_t<T>>());
}

} // namespace impl

template <typename T, typename Compare>
void sort(T first, T last, Compare comp) {
    using namespace impl;
    using std::distance;
    assert(distance(first, last) >= 0 && "wrong pointers: *last less than *first");
    while (distance(first, last) > 1) {
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
        //T* pivot = select_pivot(first, last, comp);
        auto pivot = do_partition(first, last, comp);
        if (distance(first, pivot) < distance(pivot, last)) {
            quick_sort::sort(first, pivot, comp);
            first = pivot;
        } else {
            quick_sort::sort(pivot, last, comp);
            last = pivot;
        }
    }
    //quick_sort::sort(first, pivot, comp);
    //quick_sort::sort(pivot, last, comp);
}

template <typename T>
void sort(T first, T last) {
    quick_sort::sort(first, last, std::less<std::iter_value_t<T>>());
}

} // namespace quick_sort