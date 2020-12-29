#include "memory_allocator.hpp"

int main() {
    memory::MemoryAllocator ma;
    memory::FixedSizeAllocator<16, 16> fsa;
    fsa.init();
    fsa.dumpBuffer();
    return 0;
}