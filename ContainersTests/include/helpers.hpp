#pragma once

namespace helpers {

template <typename ValueType>
struct trivially_copyable_with_new {
    ValueType x{};

    trivially_copyable_with_new() = default;
    trivially_copyable_with_new(trivially_copyable_with_new const&) = default;
    trivially_copyable_with_new(trivially_copyable_with_new&&) = default;
    trivially_copyable_with_new& operator=(trivially_copyable_with_new const&) = default;
    trivially_copyable_with_new& operator=(trivially_copyable_with_new&&) = default;
    ~trivially_copyable_with_new() = default;

    trivially_copyable_with_new(ValueType const& n)
        : x(n) {
    }

    [[nodiscard]] ValueType data() const {
        return x;
    }

    void* operator new(std::size_t const size) {
        std::cout << "TC new" << std::endl;
        return ::operator new(size);
    }

    void operator delete(void* ptr) {
        std::cout << "TC delete" << std::endl;
        ::operator delete(ptr);
    }

    void* operator new[](std::size_t const size) {
        std::cout << "TC new[]" << std::endl;
        return ::operator new[](size);
    }

    void operator delete[](void* ptr) {
        std::cout << "TC delete[]" << std::endl;
        ::operator delete[](ptr);
    }
};

template <typename ValueType>
struct non_trivially_copyable_with_new {
    ValueType x{};

    non_trivially_copyable_with_new() = default;
    non_trivially_copyable_with_new(non_trivially_copyable_with_new const&) = default;
    non_trivially_copyable_with_new(non_trivially_copyable_with_new&&) = default;
    non_trivially_copyable_with_new& operator=(non_trivially_copyable_with_new const&) = default;
    non_trivially_copyable_with_new& operator=(non_trivially_copyable_with_new&&) = default;
    virtual ~non_trivially_copyable_with_new() = default;

    non_trivially_copyable_with_new(ValueType const& n)
        : x(n) {
    }

    [[nodiscard]] ValueType data() const {
        return x;
    }

    void* operator new(std::size_t const size) {
        std::cout << "NTC new" << std::endl;
        return ::operator new(size);
    }

    void operator delete(void* ptr) {
        std::cout << "NTC delete" << std::endl;
        ::operator delete(ptr);
    }

    void* operator new[](std::size_t const size) {
        std::cout << "NTC new[]" << std::endl;
        return ::operator new[](size);
    }

    void operator delete[](void* ptr) {
        std::cout << "NTC delete[]" << std::endl;
        ::operator delete[](ptr);
    }
};

} // namespace helpers