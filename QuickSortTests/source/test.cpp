#include "pch.h"

namespace quick_sort::impl {
size_t k_insertion_threshold{ 15 };
} // namespace quick_sort::impl

TEST(TestCaseName, TestName) {
    EXPECT_EQ(1, 1);
    EXPECT_TRUE(true);
}

TEST(Assertion, LastPointerIsLessThanFirst) {
    int a[] = { 3, 2, 1 };
    ASSERT_DEBUG_DEATH(quick_sort::sort(a + 2, a + 1), "");
}

TEST(Assertion, DISABLED_WrongPivot) {
    int a[] = { 4, 3, 2, 1 };
    ASSERT_DEBUG_DEATH(quick_sort::impl::do_partition(a + 3, a + 2), "");
}

TEST(Quick_Sort, SelectPivot) {
    {
        int a[] = { 1, 2, 3 };
        ASSERT_EQ(quick_sort::impl::select_pivot(a, a + 3, std::less{}), a + 1);
    }
    {
        int a[] = { 1, 3, 2 };
        ASSERT_EQ(quick_sort::impl::select_pivot(a, a + 3, std::less{}), a + 2);
    }
    {
        int a[] = { 2, 1, 3 };
        ASSERT_EQ(quick_sort::impl::select_pivot(a, a + 3, std::less{}), a);
    }
    {
        int a[] = { 2, 3, 1 };
        ASSERT_EQ(quick_sort::impl::select_pivot(a, a + 3, std::less{}), a);
    }
    {
        int a[] = { 3, 1, 2 };
        ASSERT_EQ(quick_sort::impl::select_pivot(a, a + 3, std::less{}), a + 2);
    }
    {
        int a[] = { 3, 2, 1 };
        ASSERT_EQ(quick_sort::impl::select_pivot(a, a + 3, std::less{}), a + 1);
    }
}

TEST(Quick_Sort, Partition) {
    {
        int a[] = { 1 };
        auto pivot = quick_sort::impl::do_partition(a, a + 1);
        ASSERT_EQ(a[0], 1);
        ASSERT_EQ(pivot, a + 1);
    }
    {
        int a[] = { 1, 1 };
        auto pivot = quick_sort::impl::do_partition(a, a + 2);
        ASSERT_EQ(a[0], 1);
        ASSERT_EQ(a[1], 1);
        ASSERT_EQ(pivot, a + 1);
    }
    {
        int a[] = { 1, 1, 1 };
        auto pivot = quick_sort::impl::do_partition(a, a + 3);
        ASSERT_EQ(a[0], 1);
        ASSERT_EQ(a[1], 1);
        ASSERT_EQ(a[2], 1);
        ASSERT_EQ(pivot, a + 2);
    }
    {
        int a[] = { 1, 1, 2 };
        auto pivot = quick_sort::impl::do_partition(a, a + 3);
        ASSERT_EQ(a[0], 1);
        ASSERT_EQ(a[1], 1);
        ASSERT_EQ(a[2], 2);
        ASSERT_EQ(pivot, a + 1);
    }
    {
        int a[] = { 1, 2, 2 };
        auto pivot = quick_sort::impl::do_partition(a, a + 3);
        ASSERT_EQ(a[0], 1);
        ASSERT_EQ(a[1], 2);
        ASSERT_EQ(a[2], 2);
        ASSERT_EQ(pivot, a + 2);
    }
    {
        int a[] = { 1, 2 };
        auto pivot = quick_sort::impl::do_partition(a, a + 2);
        ASSERT_EQ(a[0], 1);
        ASSERT_EQ(a[1], 2);
        ASSERT_EQ(pivot, a + 1);
    }
    {
        int a[] = { 2, 1 };
        auto pivot = quick_sort::impl::do_partition(a, a + 2);
        ASSERT_EQ(a[0], 1);
        ASSERT_EQ(a[1], 2);
        ASSERT_EQ(pivot, a + 1);
    }
    {
        int a[] = { 1, 2, 3 };
        auto pivot = quick_sort::impl::do_partition(a, a + 3);
        ASSERT_EQ(a[0], 1);
        ASSERT_EQ(a[1], 2);
        ASSERT_EQ(a[2], 3);
        std::advance(pivot, -1);
        ASSERT_TRUE(std::distance(a, pivot) >= 0 && std::distance(a, pivot) < 3);
        ASSERT_EQ(*pivot, 2);
    }
    {
        int a[] = { 1, 3, 2 };
        auto pivot = quick_sort::impl::do_partition(a, a + 3);
        ASSERT_EQ(a[0], 1);
        ASSERT_EQ(a[1], 2);
        ASSERT_EQ(a[2], 3);
        std::advance(pivot, -1);
        ASSERT_TRUE(std::distance(a, pivot) >= 0 && std::distance(a, pivot) < 3);
        ASSERT_EQ(*pivot, 2);
    }
    {
        int a[] = { 2, 1, 3 };
        auto pivot = quick_sort::impl::do_partition(a, a + 3);
        ASSERT_EQ(a[0], 1);
        ASSERT_EQ(a[1], 2);
        ASSERT_EQ(a[2], 3);
        std::advance(pivot, -1);
        ASSERT_TRUE(std::distance(a, pivot) >= 0 && std::distance(a, pivot) < 3);
        ASSERT_EQ(*pivot, 1);
    }
    {
        int a[] = { 2, 3, 1 };
        auto pivot = quick_sort::impl::do_partition(a, a + 3);
        ASSERT_EQ(a[0], 1);
        ASSERT_EQ(a[1], 3);
        ASSERT_EQ(a[2], 2);
        std::advance(pivot, -1);
        ASSERT_TRUE(std::distance(a, pivot) >= 0 && std::distance(a, pivot) < 3);
        ASSERT_EQ(*pivot, 1);
    }
    {
        int a[] = { 3, 1, 2 };
        auto pivot = quick_sort::impl::do_partition(a, a + 3);
        ASSERT_EQ(a[0], 2);
        ASSERT_EQ(a[1], 1);
        ASSERT_EQ(a[2], 3);
        std::advance(pivot, -1);
        ASSERT_TRUE(std::distance(a, pivot) >= 0 && std::distance(a, pivot) < 3);
        ASSERT_EQ(*pivot, 1);
    }
    {
        int a[] = { 3, 2, 1 };
        auto pivot = quick_sort::impl::do_partition(a, a + 3);
        ASSERT_EQ(a[0], 1);
        ASSERT_EQ(a[1], 2);
        ASSERT_EQ(a[2], 3);
        std::advance(pivot, -1);
        ASSERT_TRUE(std::distance(a, pivot) >= 0 && std::distance(a, pivot) < 3);
        ASSERT_EQ(*pivot, 2);
    }
    {
        int a[] = { 5, 2, 5, 3, 5, 1, 2, 3, 2, 1, 3, 3, 3, 3, 3 };
        auto pivot = quick_sort::impl::do_partition(a, a + 15);
        ASSERT_EQ(a[0], 1);
        ASSERT_EQ(a[1], 2);
        ASSERT_EQ(a[2], 3);
        std::advance(pivot, -1);
        ASSERT_TRUE(std::distance(a, pivot) >= 0 && std::distance(a, pivot) < 3);
        ASSERT_EQ(*pivot, 2);
    }
}

TEST(Insertion_Sort, TriviallyCopyable) {
    int a[] = { 4, 3, 2, 1 };
    const int len = sizeof(a) / sizeof(a[0]);
    insertion_sort::sort(a, a + len);
    for (size_t i = 1; i < len; ++i) {
        ASSERT_TRUE(a[i - 1] < a[i]);
    }
}

TEST(Insertion_Sort, Non_TriviallyCopyable) {
    helpers::non_trivially_copyable_struct<int> a[]{ 4, 3, 2, 1 };
    const auto len = std::size(a);
    insertion_sort::sort(a, a + len);
    for (size_t i = 1; i < len; ++i) {
        ASSERT_TRUE(a[i - 1] < a[i]);
    }
}

template <typename T>
void permutation_test(void (*sort_func)(T*, T*)) {
    const auto max_len = 8;
    T a[max_len]{};
    T b[max_len]{};
    T c[max_len]{};
    //auto counter = 0;
    for (size_t i = 0; i < max_len; ++i) {
        a[i] = static_cast<int>(i + 1);
    }
    for (size_t size = 1; size <= max_len; ++size) {
        for (size_t i = 0; i < size; ++i) {
            b[i] = a[i];
        }
        do {
            for (size_t i = 0; i < size; ++i) {
                c[i] = b[i];
            }
            sort_func(c, c + size);
            auto correct = true;
            for (size_t i = 1; i < size; ++i) {
                correct &= c[i] > c[i - 1];
            }
            if (!correct) {
                auto s1 = helpers::serialize_array(b, b + size);
                auto s2 = helpers::serialize_array(c, c + size);
                ASSERT_EQ(s1, s2);
            }
            //if (counter == 100) {
            //    auto s1 = helpers::serialize_array(b, b + size);
            //    std::cout << s1 << std::endl;
            //}
            //counter++;
        } while (std::next_permutation(b, b + size));
    }
}

TEST(Insertion_Sort, TriviallyCopyable_Int_Permutations) {
    permutation_test<int>(insertion_sort::sort);
}

TEST(Insertion_Sort, TriviallyCopyable_StructLongLong_Permutations) {
    permutation_test<helpers::trivially_copyable_struct<int64_t>>(insertion_sort::sort);
}

TEST(Insertion_Sort, Non_TriviallyCopyable_StructLongLong_Permutations) {
    permutation_test<helpers::non_trivially_copyable_struct<int64_t>>(insertion_sort::sort);
}

TEST(Insertion_Sort, Vector) {
    std::vector<int64_t> v{ 5, 3, 4, 2, 1 };
    insertion_sort::sort(std::begin(v), std::end(v));
    for (size_t i = 1; i < v.size(); ++i) {
        ASSERT_TRUE(v[i - 1] < v[i]);
    }
}

TEST(Insertion_Sort, ReversedVector) {
    std::vector<int64_t> v{ 1, 3, 4, 2, 5 };
    insertion_sort::sort(std::make_reverse_iterator(std::end(v)), std::make_reverse_iterator(std::begin(v)));
    for (size_t i = 1; i < v.size(); ++i) {
        ASSERT_TRUE(v[i - 1] > v[i]);
    }
}

TEST(Insertion_Sort, List) {
    std::list<int64_t> l{ 5, 3, 4, 2, 1 };
    insertion_sort::sort(std::begin(l), std::end(l));
    int64_t prev = 0;
    for (auto&& i : l) {
        ASSERT_TRUE(prev < i);
        prev = i;
    }
}

//TEST(Insertion_Sort, ForwardList) {
//    std::forward_list<int64_t> l{ 5, 3, 4, 2, 1 };
//    insertion_sort::sort(std::begin(l), std::end(l));
//    int64_t prev = 0;
//    for (auto&& i : l) {
//        ASSERT_TRUE(prev < i);
//        prev = i;
//    }
//}

TEST(Quick_Sort, TriviallyCopyable) {
    for (size_t k = 0; k < 10; ++k) {
        quick_sort::impl::k_insertion_threshold = k;
        int a[] = { 4, 3, 2, 1 };
        const int len = sizeof(a) / sizeof(a[0]);
        quick_sort::sort(a, a + len);
        for (size_t i = 1; i < len; ++i) {
            ASSERT_TRUE(a[i - 1] < a[i]);
        }
    }
}

TEST(Quick_Sort, Non_TriviallyCopyable) {
    for (size_t k = 0; k < 10; ++k) {
        quick_sort::impl::k_insertion_threshold = k;
        helpers::non_trivially_copyable_struct<int> a[]{ 4, 3, 2, 1 };
        const auto len = std::size(a);
        quick_sort::sort(a, a + len);
        for (size_t i = 1; i < len; ++i) {
            ASSERT_TRUE(a[i - 1] < a[i]);
        }
    }
}

TEST(Quick_Sort, TriviallyCopyable_Int_Permutations) {
    for (size_t k = 0; k < 10; ++k) {
        quick_sort::impl::k_insertion_threshold = k;
        permutation_test<int>(quick_sort::sort);
    }
}

TEST(Quick_Sort, TriviallyCopyable_StructLongLong_Permutations) {
    for (size_t k = 0; k < 10; ++k) {
        quick_sort::impl::k_insertion_threshold = k;
        permutation_test<helpers::trivially_copyable_struct<int64_t>>(quick_sort::sort);
    }
}

TEST(Quick_Sort, Non_TriviallyCopyable_StructLongLong_Permutations) {
    for (size_t k = 0; k < 10; ++k) {
        quick_sort::impl::k_insertion_threshold = k;
        permutation_test<helpers::non_trivially_copyable_struct<int64_t>>(quick_sort::sort);
    }
}

TEST(Quick_Sort, Vector) {
    for (size_t k = 0; k < 10; ++k) {
        quick_sort::impl::k_insertion_threshold = k;
        std::vector<int64_t> v{ 5, 3, 4, 2, 1 };
        quick_sort::sort(std::begin(v), std::end(v));
        for (size_t i = 1; i < v.size(); ++i) {
            ASSERT_TRUE(v[i - 1] < v[i]);
        }
    }
}

TEST(Quick_Sort, ReversedVector) {
    for (size_t k = 0; k < 10; ++k) {
        quick_sort::impl::k_insertion_threshold = k;
        std::vector<int64_t> v{ 1, 3, 4, 2, 5 };
        quick_sort::sort(std::make_reverse_iterator(std::end(v)), std::make_reverse_iterator(std::begin(v)));
        for (size_t i = 1; i < v.size(); ++i) {
            ASSERT_TRUE(v[i - 1] > v[i]);
        }
    }
}

TEST(Quick_Sort, DISABLED_List) {
    for (size_t k = 0; k < 10; ++k) {
        quick_sort::impl::k_insertion_threshold = k;
        std::list<int64_t> l{ 5, 3, 4, 2, 1 };
        quick_sort::sort(std::begin(l), std::end(l));
        int64_t prev = 0;
        for (auto&& i : l) {
            ASSERT_TRUE(prev < i);
            prev = i;
        }
    }
}

TEST(Quick_Sort, Similar_1) {
    std::vector<int> v;
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            v.push_back(i);
        }
    }
    random_shuffle(v.begin(), v.end());
    std::cout << helpers::serialize_array(v.begin(), v.end()) << std::endl;
    quick_sort::impl::k_insertion_threshold = 0;
    quick_sort::sort(v.begin(), v.end());
    std::cout << helpers::serialize_array(v.begin(), v.end()) << std::endl;
}

TEST(Quick_Sort, LargeArray) {
    size_t const len = 100'000'000;
    std::vector<int> v;
    v.reserve(len);
    for (int i = 0; i < len / 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            v.emplace_back(i);
        }
    }
    random_shuffle(v.begin(), v.end());
    quick_sort::impl::k_insertion_threshold = 25;
    quick_sort::sort(v.begin(), v.end());
    ASSERT_TRUE(is_sorted(v.begin(), v.end()));
    for (int i = 0; i < 100; ++i) {
        std::cout << v[i] << ' ';
    }
}