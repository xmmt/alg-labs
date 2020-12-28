#pragma once

#include <memory>

namespace memory {

class AllocatorInterface {
public:
    virtual ~AllocatorInterface() = 0;
    virtual void init() = 0;
    virtual void destroy() = 0;
    virtual void* alloc(size_t) = 0;
    virtual void free(void*) = 0;
#if _DEBUG
    virtual void dumpStat() const = 0;
    virtual void dumpBlocks() const = 0;
#endif
};

template <size_t BlockSize, size_t Alignment, size_t BlockCount>
class FixedSizeAllocator final : public AllocatorInterface {
public:
    FixedSizeAllocator() {
    }
    FixedSizeAllocator(FixedSizeAllocator const& other) {
    }
    FixedSizeAllocator(FixedSizeAllocator&& other) {
    }
    FixedSizeAllocator& operator=(FixedSizeAllocator const& other) {
        return *this;
    }
    FixedSizeAllocator& operator=(FixedSizeAllocator&& other) {
        return *this;
    }
    ~FixedSizeAllocator() final {
    }

    void init() final {
    }
    void destroy() final {
    }
    void* alloc(size_t size) final {
    }
    void free(void* p) final {
    }
#if _DEBUG
    void dumpStat() const final {
    }
    void dumpBlocks() const final {
    }
#endif
private:
    using block_storage_t = std::aligned_storage_t<BlockSize, Alignment>;
    static constexpr size_t start_tag_value{ 123456789 };
    static constexpr size_t end_tag_value{ 987654321 };
    struct service_info {
    };
    struct block {
        service_info info;
#if _DEBUG
        size_t start_tag{ start_tag_value };
#endif
        block_storage_t storage;
#if _DEBUG
        size_t end_tag{ end_tag_value };
#endif
    };
    using buffer_storage_t = std::aligned_storage_t<BlockCount * sizeof(block), alignof(block)>;
    std::unique_ptr<buffer_storage_t> buffer_{ nullptr };
};

class MemoryAllocator final : public AllocatorInterface {
public:
    MemoryAllocator() {
    }
    MemoryAllocator(MemoryAllocator const& other) {
    }
    MemoryAllocator(MemoryAllocator&& other) {
    }
    MemoryAllocator& operator=(MemoryAllocator const& other) {
        return *this;
    }
    MemoryAllocator& operator=(MemoryAllocator&& other) {
        return *this;
    }
    ~MemoryAllocator() final {
    }
    void init() final {
    }
    void destroy() final {
    }
    void* alloc(size_t size) final {
        return nullptr;
    }
    void free(void* p) final {
    }
#if _DEBUG
    void dumpStat() const final {
    }
    void dumpBlocks() const final {
    }
#endif
};

} // namespace memory