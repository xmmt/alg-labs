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

TEST(Insertion_Sort, Positive_1) {
    int a[] = { 4, 3, 2, 1 };
    const int len = sizeof(a) / sizeof(a[0]);
    insertion_sort::sort(a, a + len);
    for (int i = 1; i < len; ++i) {
        ASSERT_TRUE(a[i - 1] < a[i]);
    }
}

TEST(Insertion_Sort, Positive_2) {
    struct elem {
        int x;
        elem(int x)
            : x(x) {
        }
        virtual ~elem() = default;
        auto operator<=>(const elem&) const = default;
    };
    elem a[] = { elem(4), elem(3), elem(2), elem(1) };
    const int len = sizeof(a) / sizeof(a[0]);
    insertion_sort::sort(a, a + len);
    for (int i = 1; i < len; ++i) {
        ASSERT_TRUE(a[i - 1] < a[i]);
    }
}