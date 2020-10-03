#include "pch.h"

TEST(TestCaseName, TestName) {
    EXPECT_EQ(1, 1);
    EXPECT_TRUE(true);
}

TEST(Assertion, LastPointerIsLessThanFirst) {
    int a[] = { 3, 2, 1 };
    ASSERT_DEBUG_DEATH(QuickSort::sort(a + 2, a + 1), "");
}

TEST(Assertion, WrongPivot)
{
    int a[] = { 4, 3, 2, 1 };
    ASSERT_DEBUG_DEATH(QuickSort::Private::do_partition(a + 2, a + 3, a), "");
}