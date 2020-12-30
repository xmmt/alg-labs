#include "memory_allocator.hpp"

#include <vector>
#include <chrono>

int main() {
    /*
    memory::CoalesceAllocator<512> ca;
    ca.init();
    ca.dumpBuffer();
    ca.dumpInfo();
    auto* m1 = ca.alloc(16);
    {
        auto* p = static_cast<std::byte*>(m1);
        for (int i = 0; i < 16; ++i) {
            *p = static_cast<std::byte>(16);
            p++;
        }
    }
    ca.dumpBuffer();
    ca.dumpInfo();
    auto* m2 = ca.alloc(48);
    {
        auto* p = static_cast<std::byte*>(m2);
        for (int i = 0; i < 48; ++i) {
            *p = static_cast<std::byte>(48);
            p++;
        }
    }
    ca.dumpBuffer();
    ca.dumpInfo();
    auto* m3 = ca.alloc(32);
    {
        auto* p = static_cast<std::byte*>(m3);
        for (int i = 0; i < 32; ++i) {
            *p = static_cast<std::byte>(32);
            p++;
        }
    }
    ca.dumpBuffer();
    ca.dumpInfo();
    auto* m4 = ca.alloc(64);
    {
        auto* p = static_cast<std::byte*>(m4);
        for (int i = 0; i < 64; ++i) {
            *p = static_cast<std::byte>(64);
            p++;
        }
    }
    ca.dumpBuffer();
    ca.dumpInfo();
    auto* m6 = ca.alloc(16);
    {
        auto* p = static_cast<std::byte*>(m6);
        for (int i = 0; i < 17; ++i) {
            *p = static_cast<std::byte>(17);
            p++;
        }
    }
    ca.dumpBuffer();
    ca.dumpInfo();
    ca.free(m2);
    ca.free(m1);
    ca.dumpBuffer();
    ca.dumpInfo();
    auto* m5 = ca.alloc(56);
    {
        auto* p = static_cast<std::byte*>(m5);
        for (int i = 0; i < 56; ++i) {
            *p = static_cast<std::byte>(56);
            p++;
        }
    }
    ca.dumpBuffer();
    ca.dumpInfo();
    ca.free(m5);
    auto* m7 = ca.alloc(32);
    {
        auto* p = static_cast<std::byte*>(m7);
        for (int i = 0; i < 31; ++i) {
            *p = static_cast<std::byte>(31);
            p++;
        }
    }
    auto* m8 = ca.alloc(32);
    {
        auto* p = static_cast<std::byte*>(m8);
        for (int i = 0; i < 33; ++i) {
            *p = static_cast<std::byte>(33);
            p++;
        }
    }
    ca.dumpBuffer();
    ca.dumpInfo();
    ca.free(m3);
    ca.free(m4);
    //ca.free(m5);
    ca.free(m6);
    ca.free(m7);
    ca.free(m8);

    memory::CoalesceAllocator<1024> ca2;
    ca2.init();
    for (int i = 0; i < 500; ++i) {
        {
            ca2.dumpInfo();
            auto* b1 = ca2.alloc(24);
            ca2.dumpInfo();
            auto* b2 = ca2.alloc(72);
            ca2.dumpInfo();
            auto* b3 = ca2.alloc(160);
            ca2.dumpInfo();
            auto* b4 = ca2.alloc(144);
            ca2.dumpInfo();
            auto* b5 = ca2.alloc(16);
            ca2.dumpInfo();
            ca2.free(b3);
            ca2.free(b1);
            ca2.free(b4);
            ca2.free(b2);
            ca2.free(b5);
            ca2.dumpInfo();
        }
        {
            ca2.dumpInfo();
            auto* b1 = ca2.alloc(32);
            ca2.dumpInfo();
            auto* b2 = ca2.alloc(200);
            ca2.dumpInfo();
            auto* b3 = ca2.alloc(64);
            ca2.dumpInfo();
            auto* b4 = ca2.alloc(104);
            ca2.dumpInfo();
            auto* b5 = ca2.alloc(16);
            ca2.dumpInfo();
            ca2.free(b5);
            ca2.free(b4);
            ca2.free(b2);
            ca2.free(b3);
            ca2.free(b1);
            ca2.dumpInfo();
        }
    }

    memory::FixedSizeAllocator<8, 60> fsa;
    fsa.init();
    fsa.dumpInfo();
    fsa.dumpBuffer();
    auto* p1 = new (fsa.alloc(1)) int;
    fsa.dumpInfo();
    auto* p2 = static_cast<double*>(fsa.alloc(1));
    fsa.dumpInfo();
    auto* p3 = fsa.alloc(1);
    fsa.dumpInfo();
    fsa.free(p2);
    fsa.dumpInfo();
    auto* p4 = fsa.alloc(1);
    fsa.dumpInfo();
    auto* p5 = fsa.alloc(1);
    fsa.dumpInfo();
    fsa.free(p1);
    fsa.dumpInfo();
    fsa.free(p5);
    fsa.dumpInfo();
    auto* p6 = fsa.alloc(1);
    fsa.dumpInfo();
    auto* p7 = fsa.alloc(1);
    fsa.dumpInfo();
    auto* p8 = fsa.alloc(1);
    fsa.dumpInfo();
    auto* p9 = fsa.alloc(1);
    fsa.dumpInfo();
    auto* p10 = fsa.alloc(1);
    fsa.dumpInfo();
    auto* p11 = fsa.alloc(1);
    fsa.dumpInfo();
    fsa.free(p6);
    fsa.dumpInfo();
    auto* p12 = fsa.alloc(1);
    fsa.dumpInfo();
    auto* p13 = fsa.alloc(1);
    fsa.dumpInfo();
    fsa.free(p12);
    fsa.dumpInfo();
    auto* p14 = fsa.alloc(1);
    fsa.dumpInfo();
    auto* p15 = fsa.alloc(1);
    fsa.dumpInfo();
    fsa.dumpBuffer();

    //fsa.free(p1);
    //fsa.free(p2);
    fsa.free(p3);
    fsa.free(p4);
    //fsa.free(p5);
    //fsa.free(p6);
    fsa.free(p7);
    fsa.free(p8);
    fsa.free(p9);
    fsa.free(p10);
    fsa.free(p11);
    //fsa.free(p12);
    fsa.free(p13);
    fsa.free(p14);
    fsa.free(p15);

    fsa.destroy();
    ca.destroy();
    ca2.destroy();
    */
    auto start_point = std::chrono::steady_clock::now();
    {
        memory::MemoryAllocator ma;
        ma.init();
        for (int j = 0; j < 2; ++j) {
            std::vector<void*> z;
            for (int i = 0; i < 1000000; ++i) {
                z.push_back(ma.alloc(8));
                z.push_back(ma.alloc(12));
                z.push_back(ma.alloc(16));
                z.push_back(ma.alloc(20));
                z.push_back(ma.alloc(24));
                z.push_back(ma.alloc(32));
                z.push_back(ma.alloc(48));
                z.push_back(ma.alloc(60));
                z.push_back(ma.alloc(64));
                z.push_back(ma.alloc(72));
                z.push_back(ma.alloc(73));
                z.push_back(ma.alloc(8));
                z.push_back(ma.alloc(12));
                z.push_back(ma.alloc(16));
                z.push_back(ma.alloc(20));
                z.push_back(ma.alloc(24));
                z.push_back(ma.alloc(32));
                z.push_back(ma.alloc(48));
                z.push_back(ma.alloc(60));
                z.push_back(ma.alloc(64));
                z.push_back(ma.alloc(72));
                z.push_back(ma.alloc(73));
                z.push_back(ma.alloc(100));
                z.push_back(ma.alloc(120));
                z.push_back(ma.alloc(130));
                z.push_back(ma.alloc(128));
                z.push_back(ma.alloc(200));
                z.push_back(ma.alloc(260));
                z.push_back(ma.alloc(200));
                z.push_back(ma.alloc(270));
                z.push_back(ma.alloc(256));
                z.push_back(ma.alloc(500));
                z.push_back(ma.alloc(512));
                z.push_back(ma.alloc(520));
                z.push_back(ma.alloc(523));
                z.push_back(ma.alloc(600));
                z.push_back(ma.alloc(700));
                //z.push_back(ma.alloc(800));
                //z.push_back(ma.alloc(900));
                //z.push_back(ma.alloc(1000));
                //z.push_back(ma.alloc(100000));
                //z.push_back(ma.alloc(200000));
                //z.push_back(ma.alloc(300300));
                //z.push_back(ma.alloc(2000000));
                //z.push_back(ma.alloc(3000000));
                //z.push_back(ma.alloc(1000000));
                //z.push_back(ma.alloc(400401));
                //z.push_back(ma.alloc(2002000));
                //z.push_back(ma.alloc(500555));
                //z.push_back(ma.alloc(6006006));
                //z.push_back(ma.alloc(777777));
            }
            for (auto* p : z) {
                ma.free(p);
            }
            z.clear();
        }
        ma.destroy();
    }
    std::cout << std::endl
              << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_point).count()
              << std::endl;

    return 0;
}