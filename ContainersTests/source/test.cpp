#include "pch.h"

TEST(TestCaseName, TestName) {
    EXPECT_EQ(1, 1);
    EXPECT_TRUE(true);
}

TEST(Dictionary, nullptr) {
    using type = helpers::non_trivially_copyable_with_new<int64_t>;
    using type_ptr = type*;
    containers::Dictionary<type_ptr, int> d;
    type t1, t2, t3;
    type_ptr p1 = &t1, p2 = &t2, p3 = &t3;
    d.put(p1, 1);
    d.put(p2, 2);
    d.put(p3, 3);
    d.put(nullptr, 4);
    d.put(p3, 5);
    d.put(nullptr, 6);
    ASSERT_EQ(d[p1], 1);
    ASSERT_EQ(d[p2], 2);
    ASSERT_EQ(d[p3], 5);
    ASSERT_EQ(d[nullptr], 6);
    int x = 2;
}

TEST(Map, nullptr) {
    using type = helpers::non_trivially_copyable_with_new<int64_t>;
    using type_ptr = type*;
    std::map<type_ptr, int> d;
    type t1, t2, t3;
    type_ptr p1 = &t1, p2 = &t2, p3 = &t3;
    d.insert({ p1, 1 });
    d.insert({ p2, 2 });
    d.insert({ p3, 3 });
    d.insert({ nullptr, 4 });
    d.insert({ p3, 5 });
    d.insert({ nullptr, 6 });
    ASSERT_EQ(d[p1], 1);
    ASSERT_EQ(d[p2], 2);
    ASSERT_EQ(d[p3], 5);
    ASSERT_EQ(d[nullptr], 6);
    int x = 2;
}

TEST(Array, Copy1) {
    struct str {
        int* m_;
        int a_, b_;
        str(int a, int b)
            : a_(a)
            , b_(b) {
            m_ = new int[50];
            for (int i = 0; i < 50; ++i) {
                m_[i] = i;
            }
        }
        str(str const&) = delete;
        str(str&& other)
            : m_{ other.m_ }
            , a_{ other.a_ }
            , b_{ other.b_ } {
            other.m_ = nullptr;
            std::cout << "moved" << std::endl;
        }
        str& operator=(str const&) = delete;
        str& operator=(str&& other) {
            m_ = other.m_;
            other.m_ = nullptr;
            a_ = other.a_;
            b_ = other.b_;
            std::cout << "move assigned" << std::endl;
            return *this;
        }
        ~str() {
            if (m_ != nullptr) {
                delete[] m_;
            }
        }
        bool check() {
            for (int i = 0; i < 50; ++i) {
                if (m_[i] != i) {
                    return false;
                }
            }
            return true;
        }
    };

    containers::Array<str> a;
    for (int i = 0; i < 100; ++i) {
        a.insert(str{ i, i });
    }
    for (int i = 0; i < 100; ++i) {
        ASSERT_TRUE(a[i].check());
    }
}

TEST(Array, Copy2) {
    struct str {
        int m_[50];
        int a_, b_;
        str(int a, int b)
            : a_(a)
            , b_(b) {
            for (int i = 0; i < 50; ++i) {
                m_[i] = i;
            }
        }
        str(str const&) = delete;
        str(str&& other) = default;
        str& operator=(str const&) = delete;
        str& operator=(str&& other) = default;
        ~str() = default;
        bool check() {
            for (int i = 0; i < 50; ++i) {
                if (m_[i] != i) {
                    return false;
                }
            }
            return true;
        }
    };

    containers::Array<str> a;
    for (int i = 0; i < 100; ++i) {
        a.insert(str{ i, i });
    }
    for (int i = 0; i < 100; ++i) {
        ASSERT_TRUE(a[i].check());
    }
}