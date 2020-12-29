#pragma once

#include "allocator_interface.hpp"
#include "fixed_size_allocator.hpp"
#include "coalesce_allocator.hpp"

#include <cassert>
#include <memory>
#include <iostream>
#include <string>
#include <cctype>

namespace memory {

class MemoryAllocator final : public AllocatorInterface {
public:
    MemoryAllocator() = default;
    MemoryAllocator(MemoryAllocator const& other) = delete;
    MemoryAllocator(MemoryAllocator&& other) = default;
    MemoryAllocator& operator=(MemoryAllocator const& other) = delete;
    MemoryAllocator& operator=(MemoryAllocator&& other) = default;
    ~MemoryAllocator() final = default;
    void init() final {
        fsa16.init();
        fsa32.init();
        fsa64.init();
        fsa128.init();
        fsa256.init();
        fsa512.init();
        ca.init();
    }
    void destroy() final {
        fsa16.destroy();
        fsa32.destroy();
        fsa64.destroy();
        fsa128.destroy();
        fsa256.destroy();
        fsa512.destroy();
        ca.destroy();
    }
    void* alloc(size_t size) final {
        size = (size + 7) & ~size_t{ 7 };
        if (size <= 16) {
            return fsa16.alloc(0);
        } else if (size <= 32) {
            return fsa32.alloc(0);
        } else if (size <= 64) {
            return fsa64.alloc(0);
        } else if (size <= 128) {
            return fsa128.alloc(0);
        } else if (size <= 256) {
            return fsa256.alloc(0);
        } else if (size <= 512) {
            return fsa512.alloc(0);
        } else if (size <= 0xA00000) {
            return ca.alloc(size);
        } else {
            return malloc(size);
        }
    }
    void free(void* p) final {
        if (p == nullptr) {
            return;
        }
        if (fsa16.belongs(p)) {
            fsa16.free(p);
        } else if (fsa32.belongs(p)) {
            fsa32.free(p);
        } else if (fsa64.belongs(p)) {
            fsa64.free(p);
        } else if (fsa128.belongs(p)) {
            fsa128.free(p);
        } else if (fsa256.belongs(p)) {
            fsa256.free(p);
        } else if (fsa512.belongs(p)) {
            fsa512.free(p);
        } else if (ca.belongs(p)) {
            ca.free(p);
        } else {
            free(p);
        }
    }
    bool belongs(void* p) final {
        return fsa16.belongs(p)
          || fsa32.belongs(p)
          || fsa64.belongs(p)
          || fsa128.belongs(p)
          || fsa256.belongs(p)
          || fsa512.belongs(p)
          || ca.belongs(p);
    }
#if _DEBUG
    void dumpStat() const final {
    }
    void dumpBlocks() const final {
    }
#endif
private:
    FixedSizeAllocator<16, 10000> fsa16;
    FixedSizeAllocator<32, 10000> fsa32;
    FixedSizeAllocator<64, 10000> fsa64;
    FixedSizeAllocator<128, 10000> fsa128;
    FixedSizeAllocator<256, 10000> fsa256;
    FixedSizeAllocator<512, 10000> fsa512;
    CoalesceAllocator<100'000'000> ca;
};

} // namespace memory