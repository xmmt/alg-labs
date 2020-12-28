#include "array.hpp"
#include "chunked_list.hpp"
#include "dictionary.hpp"
#include <iostream>
#include <vector>
#include <map>
#include <chrono>
#include <string>
#include <thread>

struct x {
    int d{ 0 };
    double f{ 0.0 };
    x() {
        std::cout << "Default constructed (" << this << ")" << std::endl;
    }
    x(int a, double b)
        : d{ a }
        , f{ b } {
        std::cout << d << ' ' << f << " constructed (" << this << ")" << std::endl;
    }
    x(x const& o) {
        if (this != &o) {
            d = o.d;
            f = o.f;
            std::cout << d << ' ' << f << " copy constructed (" << this << ") from (" << &o << ")" << std::endl;
        } else {
            std::cout << d << ' ' << f << " self-copied (" << this << ")" << std::endl;
        }
    }
    x(x&& o) noexcept {
        if (this != &o) {
            d = o.d;
            f = o.f;
            o.d = 0;
            o.f = 0;
            std::cout << d << ' ' << f << " move constructed (" << this << ") from (" << &o << ")" << std::endl;
        } else {
            std::cout << d << ' ' << f << " self-moved (" << this << ")" << std::endl;
        }
    }
    x& operator=(x const& o) {
        if (this != &o) {
            d = o.d;
            f = o.f;
            std::cout << d << ' ' << f << " copy assigned (" << this << ") from (" << &o << ")" << std::endl;
        } else {
            std::cout << d << ' ' << f << " self-copied (" << this << ")" << std::endl;
        }
        return *this;
    }
    x& operator=(x&& o) noexcept {
        if (this != &o) {
            d = o.d;
            f = o.f;
            o.d = 0;
            o.f = 0;
            std::cout << d << ' ' << f << " move assigned (" << this << ") from (" << &o << ")" << std::endl;
        } else {
            std::cout << d << ' ' << f << " self-moved (" << this << ")" << std::endl;
        }
        return *this;
    }
    ~x() {
        std::cout << d << ' ' << f << " destructed (" << this << ")" << std::endl;
    }
    auto operator<=>(x const&) const = default;
};

struct y {
    int d{ 0 };
    double f{ 0.0 };
    y() {
    }
    y(int a, double b)
        : d{ a }
        , f{ b } {
    }
    y(y const& o) {
        if (this != &o) {
            d = o.d;
            f = o.f;
        }
    }
    y(y&& o) noexcept {
        if (this != &o) {
            d = o.d;
            f = o.f;
            o.d = 0;
            o.f = 0;
        }
    }
    y& operator=(y const& o) {
        if (this != &o) {
            d = o.d;
            f = o.f;
        }
        return *this;
    }
    y& operator=(y&& o) noexcept {
        if (this != &o) {
            d = o.d;
            f = o.f;
            o.d = 0;
            o.f = 0;
        }
        return *this;
    }
    ~y() {
        int a = 3;
    }
    auto operator<=>(y const&) const = default;
};

namespace std {
template <>
struct hash<x> {
    std::size_t operator()(x const& s) const noexcept {
        const auto h1 = std::hash<int>{}(s.d);
        const auto h2 = std::hash<double>{}(s.f);
        //return h1 ^ (h2 << 1);
        //return h1;
        return s.d;
    }
};
} // namespace std

namespace std {
template <>
struct hash<y> {
    std::size_t operator()(y const& s) const noexcept {
        const auto h1 = std::hash<int>{}(s.d);
        const auto h2 = std::hash<double>{}(s.f);
        //return h1 ^ (h2 << 1);
        //return h1;
        return s.d;
    }
};
} // namespace std

int main() {
    {
        containers::Array<int> a(3);
        a.insert(1);
        a.insert(2);
        a.insert(3);
        a.insert(4);
        a.insert(5);
        for (auto const& i : a) {
            std::cout << i << ' ';
        }
        std::cout << std::endl;
        for (auto const& i : a) {
            std::cout << i << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    {
        struct x {
            int d{ 0 };
            x() = default;
            x(int a)
                : d{ a } {
            }
            x(x const& o) {
                d = o.d;
            }
            x(x&& o) {
                d = o.d;
                o.d = 0;
            }
            x& operator=(x const& o) {
                d = o.d;
                return *this;
            }
            x& operator=(x&& o) {
                d = o.d;
                o.d = 0;
                return *this;
            }
        };
        //std::vector<x> v;
        //v.emplace_back(1);
        //v.emplace_back(2);
        //v.emplace_back(3);
        //v.emplace_back(4);
        //v.emplace_back(5);
        //std::vector<x> v2(5);
        //std::copy(std::make_move_iterator(v.begin()), std::make_move_iterator(v.end()), v2.begin());
        //std::cout << std::endl;
        //for (auto& i : v) {
        //    std::cout << i.d << ' ';
        //}
        containers::Array<x> a(3);
        a.insert(1);
        a.insert(2);
        a.insert(3);
        a.insert(4);
        a.insert(5);

        if constexpr (std::random_access_iterator<decltype(a.begin())>) {
            std::cout << "yes" << std::endl;
        } else {
            std::cout << "no" << std::endl;
        }
        if constexpr (std::contiguous_iterator<decltype(a.begin())>) {
            std::cout << "yes" << std::endl;
        } else {
            std::cout << "no" << std::endl;
        }

        std::vector<x> v(5);
        std::copy(std::make_move_iterator(a.begin()), std::make_move_iterator(a.end()), v.begin());
        std::cout << std::endl;
        for (auto& i : a) {
            std::cout << i.d << ' ';
        }
        std::cout << std::endl;
        for (auto& i : v) {
            std::cout << i.d << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    {
        containers::Array<x> a;
        auto out = [&a] {
            bool comma = true;
            std::cout << "a: [";
            for (auto&& i : a) {
                if (comma) {
                    comma = false;
                    std::cout << i.d;
                } else {
                    std::cout << ", " << i.d;
                }
            }
            std::cout << "]" << std::endl;
        };
        out();
        a.insert({ 1, 1.0 });
        out();
        auto it = a.getIterator();
        it.insert({ 2, 2.0 });
        out();
        it.insert({ 3, 3.0 });
        out();
        it.next();
        it.insert({ 4, 4.0 });
        out();
        it.insert({ 5, 5.0 });
        out();
        it.next();
        it.next();
        it.insert({ 6, 6.0 });
        out();
        it.remove();
        out();
        it.remove();
        out();
        it.remove();
        out();
    }
    std::cout << std::endl;
    {
        containers::List<x> l;
        std::cout << sizeof(containers::List<std::aligned_storage_t<1, 1>>::Chunk) << std::endl;
        std::cout << sizeof(containers::List<double>::Chunk) << std::endl;
        std::cout << sizeof(std::unique_ptr<containers::List<double>::Chunk>) << std::endl;
        std::cout << sizeof(containers::Array<int>().data_) << std::endl;
    }
    std::cout << std::endl;
    {
        const int N = 100000;
        containers::Dictionary<y, int> d;
        for (auto i = 0; i < N; ++i) {
            d.put({ i, static_cast<double>(i) }, i);
        }
        for (auto i = 0; i < N; ++i) {
            d.remove({ i, static_cast<double>(i) });
        }
    }
    std::this_thread::sleep_for(std::chrono::seconds(4));
    std::cout << std::endl;
    {
        const int N = 100'000;
        containers::Dictionary<y, int> d;
        auto start = std::chrono::steady_clock::now();
        for (auto i = 0; i < N; ++i) {
            d.put({ i, static_cast<double>(i) }, i);
        }
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() << std::endl;

        start = std::chrono::steady_clock::now();
        for (auto i = 0; i < N; i += 2) {
            d.remove({ i, static_cast<double>(i) });
        }
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() << std::endl;

        std::map<y, int> mp;
        start = std::chrono::steady_clock::now();
        for (auto i = 0; i < N; ++i) {
            mp[{ i, static_cast<double>(i) }] = i;
            //d.put({ i, static_cast<double>(i) }, i);
        }
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() << std::endl;
        std::vector<int> v(N, 0);
        auto* r = d.root.get();
        auto traverse = [&v](auto ptr) {
            auto tr_impl = [&v](auto ptr, int depth, auto& tr_ref) -> int {
                if (ptr) {
                    ++v[ptr->key.d];
                    int a = tr_ref(ptr->left.get(), depth + 1, tr_ref);
                    int b = tr_ref(ptr->right.get(), depth + 1, tr_ref);
                    return a > b ? a : b;
                }
                return depth;
            };
            return tr_impl(ptr, 1, tr_impl);
        };
        int dd = traverse(r);
        for (int i = 0; i < N; ++i) {
            if (i % 2 != 0 && v[i] != 1) {
                int b = 3;
            } else if (i % 2 == 0 && v[i] != 0)
            {
                int b = 3;
            }
        }
        int a = 3;
    }
    std::cout << std::endl;
    {
        const int N = 100'000;
        containers::Dictionary<std::string, std::string> d;
        auto start = std::chrono::steady_clock::now();
        for (auto i = 0; i < N; ++i) {
            auto s1 = std::to_string(i);
            auto s2 = s1 + s1 + s1;
            auto s3 = s2 + s2 + s2;
            auto s4 = s3 + s3 + s3;
            d.put(s4, s4);
        }
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() << std::endl;
        std::map<std::string, std::string> mp;
        start = std::chrono::steady_clock::now();
        for (auto i = 0; i < N; ++i) {
            auto s1 = std::to_string(i);
            auto s2 = s1 + s1 + s1;
            auto s3 = s2 + s2 + s2;
            auto s4 = s3 + s3 + s3;
            mp[s4] = s4;
            //d.put({ i, static_cast<double>(i) }, i);
        }
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() << std::endl;
        //std::vector<int> v(N, 0);
        auto* r = d.root.get();
        auto traverse = [](auto ptr) {
            auto tr_impl = [](auto ptr, int depth, auto& tr_ref) -> int {
                if (ptr) {
                    //++v[ptr->key.d];
                    int a = tr_ref(ptr->left.get(), depth + 1, tr_ref);
                    int b = tr_ref(ptr->right.get(), depth + 1, tr_ref);
                    return a > b ? a : b;
                }
                return depth;
            };
            return tr_impl(ptr, 1, tr_impl);
        };
        int dd = traverse(r);
        //for (int i = 0; i < N; ++i) {
        //    if (v[i] != 1) {
        //        int b = 3;
        //    }
        //}
        int a = 3;
    }
    //std::this_thread::sleep_for(std::chrono::seconds(4));
    
    return 0;
}