#pragma once

#include <cassert>
#include <memory>

namespace containers {

template <typename ValueType>
class Array {
public:
    template <typename IterValueType>
    class Iterator {
    public:
        // iterator traits
        using difference_type = size_t;
        using value_type = IterValueType;
        using pointer = IterValueType*;
        using reference = IterValueType&;
        using iterator_category = std::random_access_iterator_tag;

    public:
        Iterator() = default;
        explicit Iterator(pointer const p)
            : pointer_(p) {
        }
        Iterator(Iterator const&) = default;
        Iterator(Iterator&&) = default;
        ~Iterator() = default;

        value_type const& get() const {
        }
        void set(value_type const& value) {
        }
        void insert(value_type const& value) {
        }
        void remove() {
        }
        void next() {
        }
        void prev() {
        }
        void toIndex(size_t index) {
        }
        bool hasNext() const {
        }
        bool hasPrev() const {
        }

        /// the methods below are added for compatibility with STL

        Iterator& operator++() {
            ++pointer_;
            return *this;
        }
        Iterator operator++(int) {
            Iterator result = *this;
            ++(*this);
            return result;
        }
        bool operator==(Iterator other) const {
            return pointer_ == other.pointer_;
        }
        bool operator!=(Iterator other) const {
            return !(*this == other);
        }
        value_type operator*() {
            return *pointer_;
        }

    private:
        pointer pointer_{ nullptr };
    };

    using iterator = Iterator<ValueType>;
    using const_iterator = Iterator<ValueType const>;

public:
    Array() = default;
    explicit Array(size_t const capacity)
        : capacity_(capacity) {
    }
    Array(Array const&) = default;
    Array(Array&&) = default;
    Array& operator=(Array const&) = default;
    Array& operator=(Array&&) = default;
    ~Array() = default;

    void insert(ValueType const& value) {
    }
    void insert(size_t index, ValueType const& value) {
    }
    void remove(size_t index) {
    }
    ValueType const& operator[](size_t index) const {
    }
    ValueType& operator[](size_t index) {
    }
    size_t size() const {
    }
    const_iterator const getIterator() const {
    }
    iterator getIterator() {
    }

    /// the methods below are added for compatibility with STL

public:
    Array(std::initializer_list<ValueType> values)
        : size_(values.size())
        , capacity_(size_)
        , data_(new ValueType[size_]) {
        assert(data_.get() != nullptr && "");
        std::copy(values.begin(), values.end(), data_.get());
    }
    const_iterator begin() const {
        return { data_.get() };
    }
    iterator begin() {
        return { data_.get() };
    }
    const_iterator end() const {
        return { std::next(data_.get(), size_) };
    }
    iterator end() {
        return { std::next(data_.get(), size_) };
    }
    const_iterator cbegin() const {
        return { data_.get() };
    }
    const_iterator cend() const {
        return { std::next(data_.get(), size_) };
    }


private:
    size_t size_{ 0 };
    size_t capacity_{ 0 };
    std::unique_ptr<int[]> data_{ nullptr };
};

} // namespace containers