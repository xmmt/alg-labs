#pragma once

#include "allocator_interface.hpp"

#include <cassert>
#include <memory>
#include <iostream>
#include <string>
#include <cctype>

namespace memory {

template <size_t BlockSize, size_t BlockCount>
class FixedSizeAllocator final : public AllocatorInterface {
public:
    FixedSizeAllocator() = default;
    FixedSizeAllocator(FixedSizeAllocator const&) = delete;
    FixedSizeAllocator(FixedSizeAllocator&&) = default;
    FixedSizeAllocator& operator=(FixedSizeAllocator const&) = delete;
    FixedSizeAllocator& operator=(FixedSizeAllocator&&) = default;
    ~FixedSizeAllocator() final {
    }

    void init() final {
        buffer_ = std::make_unique<buffer_storage_t>();
        start_bound_pointer_ = static_cast<void*>(buffer_.get());
        end_bound_pointer_ = static_cast<void*>(std::next(buffer_.get()));
        auto* first_block_ptr = new (start_bound_pointer_) block;
        *static_cast<block**>(static_cast<void*>(&first_block_ptr->storage)) = nullptr;
        init_blocks_count_ = 1;
        free_block_ptr_ = first_block_ptr;
    }

    void destroy() final {
    }

    void* alloc(size_t size) final {
        size = (size + 7) & ~size_t{ 7 };
        if (free_block_ptr_ == nullptr && init_blocks_count_ < BlockCount) {
            auto* new_block_ptr = std::next(static_cast<block*>(start_bound_pointer_), init_blocks_count_);
            new (new_block_ptr) block;
            *static_cast<block**>(static_cast<void*>(&new_block_ptr->storage)) = free_block_ptr_;
            free_block_ptr_ = new_block_ptr;
            init_blocks_count_ += 1;
        }
        if (free_block_ptr_ == nullptr) {
            return nullptr;
        }
        assert(check_free(free_block_ptr_));
        auto* block_ptr = free_block_ptr_;
        free_block_ptr_ = *static_cast<block**>(static_cast<void*>(&block_ptr->storage));
#if _DEBUG
        block_ptr->occupied = true;
#endif
        occupied_blocks_count_ += 1;
        return &block_ptr->storage;
    }

    void free(void* p) final {
        assert(p != nullptr);
        auto* block_ptr = static_cast<block*>(static_cast<void*>(std::prev(static_cast<std::byte*>(p), offsetof(block, storage))));
        assert(check_occupied(block_ptr));
#if _DEBUG
        block_ptr->occupied = false;
#endif
        occupied_blocks_count_ -= 1;
        *static_cast<block**>(static_cast<void*>(&block_ptr->storage)) = free_block_ptr_;
        free_block_ptr_ = block_ptr;
    }

    bool belongs(void* p) final {
        auto* block_ptr = static_cast<block*>(static_cast<void*>(std::prev(static_cast<std::byte*>(p), offsetof(block, storage))));
#if _DEBUG
        return check_boundary(block_ptr) && check_occupied(block_ptr);
#else
        return check_boundary(block_ptr);
#endif
    }

#if _DEBUG
    void dumpStat() const final {
    }
    void dumpBlocks() const final {
    }
#endif
    void dumpBuffer() const {
        std::cout.setf(std::ios::right, std::ios::adjustfield);
        auto* p = static_cast<uint8_t*>(static_cast<void*>(buffer_.get()));
        for (int i = 0; i < sizeof(buffer_storage_t); ++i, std::advance(p, 1)) {
            std::cout.width(0);
            std::cout << static_cast<void*>(p);
            std::cout.width(7);
            std::cout << "[" + std::to_string(i) + "]";
            std::cout.width(5);
            std::cout << std::hex << static_cast<uint64_t>(*p);
            std::cout.width(5);
            std::cout << std::dec << static_cast<uint64_t>(*p);
            if (std::isgraph(*p)) {
                std::cout.width(5);
                std::cout << *p;
            }
            std::cout << std::endl;
        }
    }
    void dumpInfo() const {
        std::cout << "BlockSize: " << BlockSize << ", block_size: " << block_size << ", BlockCount: " << BlockCount << std::endl;
        std::cout << "buffer size: " << sizeof(buffer_storage_t) << std::endl;
        std::cout << "buffer_: " << buffer_.get() << std::endl;
        std::cout << "start_bound_pointer_: " << start_bound_pointer_ << std::endl;
        std::cout << "end_bound_pointer_: " << end_bound_pointer_ << std::endl;
        std::cout << "free_block_ptr_: " << free_block_ptr_ << std::endl;
        std::cout << "init_blocks_count_: " << init_blocks_count_ << std::endl;
        std::cout << "occupied_blocks_count_: " << occupied_blocks_count_ << std::endl;
        std::cout << std::endl;
    }

private:
    // block structure
    static constexpr size_t block_alignment{ 8 };
    using block_storage_t = std::aligned_storage_t<BlockSize, block_alignment>;
#if _DEBUG
    static constexpr uint64_t start_tag_value{ 0x73'74'61'72'74'74'61'67ull }; // "starttag"
    static constexpr uint64_t end_tag_value{ 0x65'6e'64'5f'5f'74'61'67ull };   // "end__tag"
#endif
    struct block {
#if _DEBUG
        bool occupied{ false };
        uint64_t start_tag{ start_tag_value };
#endif
        block_storage_t storage{};
#if _DEBUG
        uint64_t end_tag{ end_tag_value };
#endif
    };
    static constexpr auto block_size = sizeof(block);

    // buffer storage
    using buffer_storage_t = std::aligned_storage_t<BlockCount * sizeof(block), alignof(block)>;
    std::unique_ptr<buffer_storage_t> buffer_{ nullptr };
    void* start_bound_pointer_{ nullptr };
    void* end_bound_pointer_{ nullptr };
    block* free_block_ptr_{ nullptr };
    size_t init_blocks_count_{ 0 };
    size_t occupied_blocks_count_{ 0 };

private:
    bool check_boundary(block* block_ptr) const {
        assert(block_ptr != nullptr);
        assert(start_bound_pointer_ != nullptr);
        assert(end_bound_pointer_ != nullptr);
        if (std::distance(static_cast<std::byte*>(start_bound_pointer_), static_cast<std::byte*>(static_cast<void*>(block_ptr))) < 0
          || std::distance(static_cast<std::byte*>(static_cast<void*>(block_ptr)), static_cast<std::byte*>(end_bound_pointer_)) <= 0) {
            return false;
        }
        if (std::distance(static_cast<std::byte*>(start_bound_pointer_), static_cast<std::byte*>(static_cast<void*>(block_ptr))) % block_size != 0) {
            return false;
        }
        return true;
    }

#if _DEBUG

    bool check_occupied(block* block_ptr) const {
        assert(block_ptr != nullptr);
        if (!check_boundary(block_ptr)) {
            return false;
        }
        return block_ptr->occupied;
    }

    static bool check_corruption(block* block_ptr) {
        assert(block_ptr != nullptr);
        return block_ptr->start_tag == start_tag_value && block_ptr->end_tag == end_tag_value;
    }

    bool check_free(block* block_ptr) const {
        assert(block_ptr != nullptr);
        if (!check_boundary(block_ptr)) {
            return false;
        }
        if (!check_corruption(block_ptr)) {
            return false;
        }
        return !block_ptr->occupied;
    }
#endif
};

} // namespace memory
