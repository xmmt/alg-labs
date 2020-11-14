#include "pch.h"

TEST(TestCaseName, TestName) {
    EXPECT_EQ(1, 1);
    EXPECT_TRUE(true);
}

TEST(Assertion, LastPointerIsLessThanFirst) {
    int a[] = { 3, 2, 1 };
    ASSERT_DEBUG_DEATH(quick_sort::sort(a + 2, a + 1), "");
}

TEST(Assertion, WrongPivot) {
    int a[] = { 4, 3, 2, 1 };
    ASSERT_DEBUG_DEATH(quick_sort::impl::do_partition(a + 2, a + 3, a), "");
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
    helpers::NonTriviallyCopyableStruct<int> a[]{ 4, 3, 2, 1 };
    const auto len = std::size(a);
    insertion_sort::sort(a, a + len);
    for (size_t i = 1; i < len; ++i) {
        ASSERT_TRUE(a[i - 1] < a[i]);
    }
}

template <typename T>
void permutationTest() {
    const auto max_len = 10;
    T a[max_len]{};
    T b[max_len]{};
    T c[max_len]{};
    for (size_t i = 0; i < max_len; ++i) {
        a[i] = static_cast<int>(i) + 1;
    }
    for (size_t size = 1; size <= max_len; ++size) {
        for (size_t i = 0; i < size; ++i) {
            b[i] = a[i];
        }
        do {
            for (size_t i = 0; i < size; ++i) {
                c[i] = b[i];
            }
            insertion_sort::sort(c, c + size);
            auto correct = true;
            for (size_t i = 1; i < size; ++i) {
                correct &= c[i] > c[i - 1];
            }
            if (!correct) {
                auto s1 = helpers::serializeArray(b, b + size);
                auto s2 = helpers::serializeArray(c, c + size);
                ASSERT_EQ(s1, s2);
            }
        } while (std::next_permutation(b, b + size));
    }
}

TEST(Insertion_Sort, TriviallyCopyable_Int_Permutations) {
    permutationTest<int>();
}

TEST(Insertion_Sort, TriviallyCopyable_StructLongLong_Permutations) {
    permutationTest<helpers::TriviallyCopyableStruct<int64_t>>();
}

TEST(Insertion_Sort, Non_TriviallyCopyable_StructLongLong_Permutations) {
    permutationTest<helpers::NonTriviallyCopyableStruct<int64_t>>();
}