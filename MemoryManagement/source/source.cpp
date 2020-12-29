#include "memory_allocator.hpp"

int main() {
    memory::MemoryAllocator ma;
    memory::CoalesceAllocator<256> ca;
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

    //memory::FixedSizeAllocator<8, 6> fsa;
    //fsa.init();
    //fsa.dumpInfo();
    //fsa.dumpBuffer();
    //auto* p1 = new (fsa.alloc(1)) int;
    //fsa.dumpInfo();
    //auto* p2 = static_cast<double*>( fsa.alloc(1));
    //fsa.dumpInfo();
    //auto* p3 = fsa.alloc(1);
    //fsa.dumpInfo();
    //fsa.free(p2);
    //fsa.dumpInfo();
    //auto* p4 = fsa.alloc(1);
    //fsa.dumpInfo();
    //auto* p5 = fsa.alloc(1);
    //fsa.dumpInfo();
    //fsa.free(p1);
    //fsa.dumpInfo();
    //fsa.free(p5);
    //fsa.dumpInfo();
    //auto* p6 = fsa.alloc(1);
    //fsa.dumpInfo();
    //auto* p7 = fsa.alloc(1);
    //fsa.dumpInfo();
    //auto* p8 = fsa.alloc(1);
    //fsa.dumpInfo();
    //auto* p9 = fsa.alloc(1);
    //fsa.dumpInfo();
    //auto* p10 = fsa.alloc(1);
    //fsa.dumpInfo();
    //auto* p11 = fsa.alloc(1);
    //fsa.dumpInfo();
    //fsa.free(p5);
    //fsa.dumpInfo();
    //auto* p12 = fsa.alloc(1);
    //fsa.dumpInfo();
    //auto* p13 = fsa.alloc(1);
    //fsa.dumpInfo();
    //fsa.free(p12);
    //fsa.dumpInfo();
    //auto* p14 = fsa.alloc(1);
    //fsa.dumpInfo();
    //auto* p15 = fsa.alloc(1);
    //fsa.dumpInfo();
    //fsa.dumpBuffer();
    return 0;
}