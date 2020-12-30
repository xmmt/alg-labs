#pragma once

#include "allocator_interface.hpp"

#include <Windows.h>

#include <cassert>
#include <memory>
#include <iostream>
#include <string>
#include <cctype>

namespace memory {

template <size_t BufferSize>
class CoalesceAllocator final : public AllocatorInterface {
public:
    CoalesceAllocator() = default;
    CoalesceAllocator(CoalesceAllocator const&) = delete;
    CoalesceAllocator(CoalesceAllocator&&) = default;
    CoalesceAllocator& operator=(CoalesceAllocator const&) = delete;
    CoalesceAllocator& operator=(CoalesceAllocator&&) = default;
    ~CoalesceAllocator() final {
        assert(destroyed_);
    }

    void init() final {
        //buffer_ = std::make_unique<buffer_storage_t>();
        buffer_.reset(static_cast<buffer_storage_t*>(VirtualAlloc(nullptr, sizeof(buffer_storage_t), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE)));
        assert(buffer_.get() != nullptr);
        start_bound_pointer_ = static_cast<void*>(buffer_.get());
        end_bound_pointer_ = static_cast<void*>(std::next(buffer_.get()));
        auto* first_block_header_ptr = new (start_bound_pointer_) header;
        auto* first_block_footer_ptr = new (std::prev(static_cast<footer*>(end_bound_pointer_))) footer;
        first_block_header_ptr->set_footer(first_block_footer_ptr);
        first_block_footer_ptr->set_header(first_block_header_ptr);
        first_block_header_ptr->set_occupied(false);
        first_block_header_ptr->next_free_block = nullptr;
        first_block_header_ptr->prev_free_block = nullptr;
        first_free_block_ptr_ = first_block_header_ptr;
        free_memory_ = get_memory_size(first_block_header_ptr);
        occupied_memory_ = get_block_size(first_block_header_ptr) - free_memory_;
        destroyed_ = false;
    }

    bool is_init() final {
        return !destroyed_;
    }

    void destroy() final {
        assert(allocated_memory_ == 0);
        buffer_.reset(nullptr);
        start_bound_pointer_ = nullptr;
        end_bound_pointer_ = nullptr;
        first_free_block_ptr_ = nullptr;
        free_memory_ = 0;
        occupied_memory_ = 0;
        allocated_memory_ = 0;
        destroyed_ = true;
    }

    void* alloc(size_t size) final {
        size = (size + 7) & ~size_t{ 7 };
        auto* ptr = first_free_block_ptr_;
        while (ptr != nullptr && get_memory_size(ptr) < size) {
            ptr = ptr->next_free_block;
        }
        if (ptr == nullptr) {
            return nullptr;
        }
        assert(check_free(ptr));
        try_split(ptr, size);
        ptr->set_occupied(true);
        if (ptr->prev_free_block == nullptr) {
            first_free_block_ptr_ = ptr->next_free_block;
        } else {
            ptr->prev_free_block->next_free_block = ptr->next_free_block;
        }
        if (ptr->next_free_block != nullptr) {
            ptr->next_free_block->prev_free_block = ptr->prev_free_block;
        }
        auto memory_size = get_memory_size(ptr);
        occupied_memory_ += memory_size;
        free_memory_ -= memory_size;
        allocated_memory_ += memory_size;
        return &ptr->prev_free_block;
    }

    void free(void* p) final {
        assert(p != nullptr);
        auto* header_ptr = static_cast<header*>(static_cast<void*>(std::prev(static_cast<std::byte*>(p), offsetof(header, prev_free_block))));
        assert(check_occupied(header_ptr));
        header_ptr->set_occupied(false);
        auto memory_size = get_memory_size(header_ptr);
        occupied_memory_ -= memory_size;
        free_memory_ += memory_size;
        allocated_memory_ -= memory_size;
        header_ptr->next_free_block = first_free_block_ptr_;
        header_ptr->prev_free_block = nullptr;
        first_free_block_ptr_ = header_ptr;
        try_merge(header_ptr);
    }

    bool belongs(void* p) final {
        auto* header_ptr = static_cast<header*>(static_cast<void*>(std::prev(static_cast<std::byte*>(p), offsetof(header, prev_free_block))));
        return check_boundary(header_ptr) && check_occupied(header_ptr);
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
        std::cout << "BufferSize: " << BufferSize << std::endl;
        std::cout << "buffer size: " << sizeof(buffer_storage_t) << std::endl;
        std::cout << "buffer_: " << buffer_.get() << std::endl;
        std::cout << "start_bound_pointer_: " << start_bound_pointer_ << std::endl;
        std::cout << "end_bound_pointer_: " << end_bound_pointer_ << std::endl;
        std::cout << "first_free_block_ptr_: " << first_free_block_ptr_ << std::endl;
        std::cout << "occupied_memory_: " << occupied_memory_ << std::endl;
        std::cout << "free_memory_: " << free_memory_ << std::endl;
        std::cout << "allocated_memry_: " << allocated_memory_ << std::endl;
        std::cout << "overhead_: " << occupied_memory_ - allocated_memory_ << std::endl;
        std::cout << std::endl;
    }

private:
    // block structure
    static constexpr size_t block_alignment{ 8 };
#if _DEBUG
    static constexpr uint64_t start_tag_value{ 0x73'74'61'72'74'74'61'67ull }; // "starttag"
    static constexpr uint64_t end_tag_value{ 0x65'6e'64'5f'5f'74'61'67ull };   // "end__tag"
#endif
    struct footer;
    struct header {
        footer* footer_ptr{ nullptr };
#if _DEBUG
        footer* footer_raw_ptr{ nullptr };
        bool is_occupied{ false };
#endif

        footer* get_footer() const {
            auto* raw_ptr = reinterpret_cast<footer*>(reinterpret_cast<uintptr_t>(footer_ptr) & ~static_cast<uintptr_t>(block_alignment - 1));
            assert(raw_ptr == footer_raw_ptr);
            return raw_ptr;
        }
        void set_footer(footer* ptr) {
            auto const o = get_occupied();
            footer_ptr = ptr;
            set_occupied(o);
#if _DEBUG
            footer_raw_ptr = ptr;
#endif
        }
        [[nodiscard]] bool get_occupied() const {
            //assert(footer_ptr != nullptr);
            if (footer_ptr == nullptr) {
                return false;
            }
            bool const o = reinterpret_cast<uintptr_t>(footer_ptr) & static_cast<uintptr_t>(block_alignment - 1);
            assert(o == is_occupied);
            return o;
        }
        void set_occupied(bool const occupied) {
            //assert(footer_ptr != nullptr);
            if (footer_ptr == nullptr) {
                return;
            }
            if (occupied) {
                footer_ptr = reinterpret_cast<footer*>(reinterpret_cast<uintptr_t>(footer_ptr) | uintptr_t{ 1 });
            } else {
                footer_ptr = reinterpret_cast<footer*>(reinterpret_cast<uintptr_t>(footer_ptr) & ~uintptr_t{ 1 });
            }
#if _DEBUG
            is_occupied = occupied;
#endif
            assert(get_occupied() == occupied);
        }
#if _DEBUG
        uint64_t start_tag{ start_tag_value };
#endif
        header* prev_free_block{ nullptr };
        header* next_free_block{ nullptr };
    };
    struct footer {
#if _DEBUG
        uint64_t end_tag{ end_tag_value };
#endif
        header* header_ptr{ nullptr };

    public:
        header* get_header() const {
            return header_ptr;
        }
        void set_header(header* ptr) {
            header_ptr = ptr;
        }
    };

    // buffer storage
    struct deleter {
        void operator()(void* p) const {
            VirtualFree(p, 0, MEM_RELEASE);
        }
    };
    using buffer_storage_t = std::aligned_storage_t<BufferSize, block_alignment>;
    std::unique_ptr<buffer_storage_t, deleter> buffer_{ nullptr };
    void* start_bound_pointer_{ nullptr };
    void* end_bound_pointer_{ nullptr };
    header* first_free_block_ptr_{ nullptr };
    size_t free_memory_{ 0 };
    size_t occupied_memory_{ 0 };
    size_t allocated_memory_{ 0 };
    bool destroyed_{ true };

private:
    void try_split(header* header_ptr, size_t size) {
        assert(header_ptr != nullptr);
        auto* footer_ptr = header_ptr->get_footer();
        assert(footer_ptr != nullptr);
        assert(header_ptr == footer_ptr->get_header());
        assert(get_memory_size(header_ptr) >= size);
        auto diff = get_memory_size(header_ptr) - size;
        if (diff < sizeof(header) + sizeof(footer)) {
            return;
        }
        auto* new_footer_ptr = new (std::next(static_cast<std::byte*>(static_cast<void*>(&header_ptr->prev_free_block)), size)) footer;
        auto* new_header_ptr = new (std::next(new_footer_ptr)) header;
        header_ptr->set_footer(new_footer_ptr);
        footer_ptr->set_header(new_header_ptr);
        new_header_ptr->set_footer(footer_ptr);
        new_footer_ptr->set_header(header_ptr);
        new_header_ptr->next_free_block = header_ptr->next_free_block;
        new_header_ptr->prev_free_block = header_ptr;
        header_ptr->next_free_block = new_header_ptr;
        assert(header_ptr == header_ptr->get_footer()->get_header());
        assert(new_header_ptr == new_header_ptr->get_footer()->get_header());
        assert(get_block_size(header_ptr) >= sizeof(header) + sizeof(footer));
        assert(get_block_size(new_header_ptr) >= sizeof(header) + sizeof(footer));
        free_memory_ -= get_service_info_size();
        occupied_memory_ += get_service_info_size();
    }

    void try_merge_left(header* header_ptr) {
        assert(header_ptr != nullptr);
        if (static_cast<void*>(header_ptr) == start_bound_pointer_) {
            return;
        }
        auto* left_footer = std::prev(static_cast<footer*>(static_cast<void*>(header_ptr)));
        assert(left_footer->get_header() != nullptr);
        auto* left_header = left_footer->get_header();
        assert(left_header != nullptr);
        assert(left_header->get_footer() == left_footer);
        assert(check_boundary(left_header));
        assert(check_boundary(left_footer));
        if (left_header->get_occupied()) {
            return;
        }

        if (left_header->prev_free_block == nullptr) {
            first_free_block_ptr_ = left_header->next_free_block;
        } else {
            left_header->prev_free_block->next_free_block = left_header->next_free_block;
        }
        if (left_header->next_free_block != nullptr) {
            left_header->next_free_block->prev_free_block = left_header->prev_free_block;
        }

        if (header_ptr->prev_free_block == nullptr) {
            first_free_block_ptr_ = header_ptr->next_free_block;
        } else {
            header_ptr->prev_free_block->next_free_block = header_ptr->next_free_block;
        }
        if (header_ptr->next_free_block != nullptr) {
            header_ptr->next_free_block->prev_free_block = header_ptr->prev_free_block;
        }

        left_header->set_footer(header_ptr->get_footer());
        left_header->get_footer()->set_header(left_header);

        left_header->next_free_block = first_free_block_ptr_;
        left_header->prev_free_block = nullptr;
        first_free_block_ptr_ = left_header;

        free_memory_ += get_service_info_size();
        occupied_memory_ -= get_service_info_size();
    }

    void try_merge_right(header* left_header) {
        assert(left_header != nullptr);
        auto* left_footer = left_header->get_footer();
        assert(left_footer != nullptr);
        assert(left_header == left_footer->get_header());
        auto* header_ptr = static_cast<header*>(static_cast<void*>(std::next(left_footer)));
        if (std::distance(static_cast<std::byte*>(static_cast<void*>(header_ptr)), static_cast<std::byte*>(end_bound_pointer_)) < sizeof(header) + sizeof(footer)) {
            return;
        }
        auto* footer_ptr = header_ptr->get_footer();
        assert(footer_ptr != nullptr);
        assert(header_ptr == footer_ptr->get_header());
        assert(check_boundary(header_ptr));
        assert(check_boundary(footer_ptr));
        if (header_ptr->get_occupied()) {
            return;
        }

        if (left_header->prev_free_block == nullptr) {
            first_free_block_ptr_ = left_header->next_free_block;
        } else {
            left_header->prev_free_block->next_free_block = left_header->next_free_block;
        }
        if (left_header->next_free_block != nullptr) {
            left_header->next_free_block->prev_free_block = left_header->prev_free_block;
        }

        if (header_ptr->prev_free_block == nullptr) {
            first_free_block_ptr_ = header_ptr->next_free_block;
        } else {
            header_ptr->prev_free_block->next_free_block = header_ptr->next_free_block;
        }
        if (header_ptr->next_free_block != nullptr) {
            header_ptr->next_free_block->prev_free_block = header_ptr->prev_free_block;
        }

        left_header->set_footer(header_ptr->get_footer());
        left_header->get_footer()->set_header(left_header);

        left_header->next_free_block = first_free_block_ptr_;
        left_header->prev_free_block = nullptr;
        first_free_block_ptr_ = left_header;

        free_memory_ += get_service_info_size();
        occupied_memory_ -= get_service_info_size();
    }

    void try_merge(header* header_ptr) {
        try_merge_right(header_ptr);
        try_merge_left(header_ptr);
    }

    static size_t get_memory_size(header* header_ptr) {
        assert(header_ptr != nullptr);
        assert(header_ptr->get_footer() != nullptr);
        assert(header_ptr == header_ptr->get_footer()->get_header());
        auto* pp = &header_ptr->prev_free_block;
        auto* oo = header_ptr->get_footer();
        return std::distance(static_cast<std::byte*>(static_cast<void*>(&header_ptr->prev_free_block)), static_cast<std::byte*>(static_cast<void*>(header_ptr->get_footer())));
    }

    static size_t get_block_size(header* header_ptr) {
        assert(header_ptr != nullptr);
        assert(header_ptr->get_footer() != nullptr);
        assert(header_ptr == header_ptr->get_footer()->get_header());
        return std::distance(static_cast<std::byte*>(static_cast<void*>(header_ptr)), static_cast<std::byte*>(static_cast<void*>(std::next(header_ptr->get_footer()))));
    }

    static constexpr size_t get_service_info_size() {
        return offsetof(header, prev_free_block) + sizeof(footer);
    }

    bool check_boundary(void* ptr) const {
        assert(ptr != nullptr);
        assert(start_bound_pointer_ != nullptr);
        assert(end_bound_pointer_ != nullptr);
        return !(std::distance(static_cast<std::byte*>(start_bound_pointer_), static_cast<std::byte*>(ptr)) < 0
          || std::distance(static_cast<std::byte*>(ptr), static_cast<std::byte*>(end_bound_pointer_)) <= 0);
    }

    bool check_occupied(header* header_ptr) const {
        assert(header_ptr != nullptr);
        auto* footer_ptr = header_ptr->get_footer();
        assert(footer_ptr != nullptr);
        assert(header_ptr == footer_ptr->get_header());
        if (!check_boundary(header_ptr) || !check_boundary(footer_ptr)) {
            return false;
        }
        return header_ptr->get_occupied();
    }

    bool check_occupied(footer* footer_ptr) const {
        assert(footer_ptr != nullptr);
        return check_occupied(footer_ptr->get_header());
    }

#if _DEBUG

    static bool check_corruption(header* header_ptr) {
        assert(header_ptr != nullptr);
        assert(header_ptr->get_footer() != nullptr);
        return header_ptr->start_tag == start_tag_value && header_ptr->get_footer()->end_tag == end_tag_value;
    }

    static bool check_corruption(footer* footer_ptr) {
        assert(footer_ptr != nullptr);
        assert(footer_ptr->get_header() != nullptr);
        return footer_ptr->get_header()->start_tag == start_tag_value && footer_ptr->end_tag == end_tag_value;
    }

    bool check_free(header* header_ptr) const {
        assert(header_ptr != nullptr);
        auto* footer_ptr = header_ptr->get_footer();
        assert(footer_ptr != nullptr);
        assert(header_ptr == footer_ptr->get_header());
        if (!check_boundary(header_ptr) || !check_boundary(footer_ptr)) {
            return false;
        }
        if (!check_corruption(header_ptr)) {
            return false;
        }
        return !check_occupied(header_ptr);
    }

#endif
};

} // namespace memory
