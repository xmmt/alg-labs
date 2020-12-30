#pragma once

#include "allocator_interface.hpp"
#include "fixed_size_allocator.hpp"
#include "coalesce_allocator.hpp"

#include <cassert>
#include <memory>
#include <iostream>
#include <string>
#include <cctype>
#include <array>

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
        fsa16[0].init();
        fsa32[0].init();
        fsa64[0].init();
        fsa128[0].init();
        fsa256[0].init();
        fsa512[0].init();
        ca[0].init();
        destroyed = false;
    }
    bool is_init() final {
        return !destroyed;
    }
    void destroy() final {
        auto destroy = [](auto& allocator) {
            for (int i = 0; i < allocator.size(); ++i) {
                if (allocator[i].is_init()) {
                    allocator[i].destroy();
                }
            }
        };
        destroy(fsa16);
        destroy(fsa32);
        destroy(fsa64);
        destroy(fsa128);
        destroy(fsa256);
        destroy(fsa512);
        destroy(ca);
        //#define DESTROY(ma)                        \
//    for (int i = 0; i < sizeof(ma); ++i) { \
//        if (ma[i].is_init()) {             \
//            ma[i].destroy();               \
//        }                                  \
//    }
        //
        // DESTROY(fsa16)
        // DESTROY(fsa32)
        // DESTROY(fsa64)
        // DESTROY(fsa128)
        // DESTROY(fsa256)
        // DESTROY(fsa512)
        // DESTROY(ca)
        destroyed = true;
    }
    template <typename Allocator>
    constexpr void* allocm(Allocator& allocator, size_t const size) {
        for (int i = 0; i < allocator.size(); ++i) {
            if (!allocator[i].is_init()) {
                allocator[i].init();
            }
            auto* p = allocator[i].alloc(size);
            if (p != nullptr) {
                return p;
            }
        }
        return nullptr;
    }
    void* alloc(size_t size) final {
        size = (size + 7) & ~size_t{ 7 };

        //auto alloc = [](auto& allocator, size_t const size) -> void* {
        //    for (int i = 0; i < allocator.size(); ++i) {
        //        if (!allocator[i].is_init()) {
        //            allocator[i].init();
        //        }
        //        auto* p = allocator[i].alloc(size);
        //        if (p != nullptr) {
        //            return p;
        //        }
        //    }
        //    return nullptr;
        //};
        if (size <= 16) {
            return allocm(fsa16, size);
        } else if (size <= 32) {
            return allocm(fsa32, size);
        } else if (size <= 64) {
            return allocm(fsa64, size);
        } else if (size <= 128) {
            return allocm(fsa128, size);
        } else if (size <= 256) {
            return allocm(fsa256, size);
        } else if (size <= 512) {
            return allocm(fsa512, size);
        } else if (size <= 0xA00000) {
            return allocm(ca, size);
        } else {
            return malloc(size);
        }

        //#define FF(ma)                             \
//    for (int i = 0; i < sizeof(ma); ++i) { \
//        if (!ma[i].is_init()) {            \
//            ma[i].init();                  \
//        }                                  \
//        auto* p = ma[i].alloc(0);          \
//        if (p != nullptr) {                \
//            return p;                      \
//        }                                  \
//    }                                      \
//    return nullptr;
        //
        //        if (size <= 16) {
        //            FF(fsa16)
        //        } else if (size <= 32) {
        //            FF(fsa32)
        //        } else if (size <= 64) {
        //            FF(fsa64)
        //        } else if (size <= 128) {
        //            FF(fsa128)
        //        } else if (size <= 256) {
        //            FF(fsa256)
        //        } else if (size <= 512) {
        //            FF(fsa512)
        //        } else if (size <= 0xA00000) {
        //            FF(ca)
        //        } else {
        //            return malloc(size);
        //        }
    }
    template <typename Allocator>
    constexpr bool freem(Allocator& allocator, void* p) {
        for (int i = 0; i < allocator.size(); ++i) {
            if (allocator[i].is_init()) {
                if (allocator[i].belongs(p)) {
                    allocator[i].free(p);
                    return true;
                }
            }
        }
        return false;
    }
    void free(void* p) final {
        if (p == nullptr) {
            return;
        }
        //auto free = [](auto& allocator, void* p) -> bool {
        //    for (int i = 0; i < allocator.size(); ++i) {
        //        if (allocator[i].is_init()) {
        //            if (allocator[i].belongs(p)) {
        //                allocator[i].free(p);
        //                return true;
        //            }
        //        }
        //    }
        //    return false;
        //};
        if (freem(fsa16, p)) {
            return;
        }
        if (freem(fsa32, p)) {
            return;
        }
        if (freem(fsa64, p)) {
            return;
        }
        if (freem(fsa128, p)) {
            return;
        }
        if (freem(fsa256, p)) {
            return;
        }
        if (freem(fsa512, p)) {
            return;
        }
        if (freem(ca, p)) {
            return;
        }
        ::free(p);
    }
    bool belongs(void* p) final {
        auto belongs = [](auto& allocator, void* p) -> bool {
            for (int i = 0; i < allocator.size(); ++i) {
                if (allocator[i].is_init() && allocator[i].belongs(p)) {
                    return true;
                }
            }
            return false;
        };
        return belongs(fsa16, p)
          || belongs(fsa32, p)
          || belongs(fsa64, p)
          || belongs(fsa128, p)
          || belongs(fsa256, p)
          || belongs(fsa512, p)
          || belongs(ca, p);
    }
#if _DEBUG
    void dumpStat() const final {
    }
    void dumpBlocks() const final {
    }
#endif
private:
    std::array<FixedSizeAllocator<16, 65536 * 32>, 100> fsa16;
    std::array<FixedSizeAllocator<32, 65536 * 16>, 100> fsa32;
    std::array<FixedSizeAllocator<64, 65536 * 8>, 100> fsa64;
    std::array<FixedSizeAllocator<128, 65536 * 4>, 100> fsa128;
    std::array<FixedSizeAllocator<256, 65536 * 2>, 100> fsa256;
    std::array<FixedSizeAllocator<512, 65536>, 100> fsa512;
    std::array<CoalesceAllocator<65536 * 1024>, 1000> ca;
    bool destroyed{ true };
};

} // namespace memory