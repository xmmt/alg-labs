#pragma once

#include <cassert>
#include <memory>

#if _DEBUG
#    include <iostream>
#endif

namespace containers {

template <typename ValueType>
class Array final {
public:
    template <typename IterValueType>
    class Iterator final {
    public:
        // iterator traits
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using value_type = IterValueType;
        using pointer = IterValueType*;
        using reference = IterValueType&;
        using iterator_category = std::contiguous_iterator_tag;
        //using iterator_category = std::random_access_iterator_tag;

    public:
        Iterator() = delete;
        Iterator(Array& a, difference_type const offset = 0)
            : offset_{ offset }
            , array_{ a } {
        }
        Iterator(Iterator const&) = default;
        Iterator(Iterator&&) = default;
        Iterator& operator=(Iterator const&) = default;
        Iterator& operator=(Iterator&&) = default;
        ~Iterator() = default;

        [[nodiscard]] value_type const& get() const {
            return *pointer_();
        }
        void set(value_type const& value) {
            *pointer_() = value;
        }
        void insert(value_type const& value) {
            array_.insert(offset_, value);
        }
        void remove() {
            array_.remove(offset_);
        }
        void next() {
            ++offset_;
        }
        void prev() {
            --offset_;
        }
        void toIndex(size_type index) {
            offset_ = index;
        }

        [[nodiscard]] bool hasNext() const {
            return offset_ < array_.size_;
        }

        [[nodiscard]] bool hasPrev() const {
            return offset_ > 0;
        }

        /// the methods below are added for compatibility with STL

        Iterator& operator++() {
            offset_ += 1;
            return *this;
        }
        Iterator operator++(int) {
            Iterator result = *this;
            ++(*this);
            return result;
        }
        Iterator& operator--() {
            offset_ -= 1;
            return *this;
        }
        Iterator operator--(int) {
            Iterator result = *this;
            --(*this);
            return result;
        }
        Iterator operator+(difference_type const off) const {
            return { offset_ + off, array_ };
        }
        friend Iterator operator+(difference_type const off, Iterator const& it) {
            return { it.offset_ + off, it.array_ };
        }
        Iterator operator-(difference_type const off) const {
            return { offset_ - off, array_ };
        }
        difference_type operator-(Iterator const& other) const {
            return std::distance(other.pointer_(), pointer_());
        }
        Iterator& operator+=(difference_type const off) {
            offset_ += off;
            return *this;
        }
        Iterator& operator-=(difference_type const off) {
            offset_ -= off;
            return *this;
        }
        auto operator<=>(Iterator const& other) const {
            return pointer_() <=> other.pointer_();
        }
        auto operator==(Iterator const& other) const {
            return pointer_() == other.pointer_();
        }
        reference operator*() const {
            return *pointer_();
        }
        pointer operator->() const {
            return pointer_();
        }
        reference operator[](difference_type const off) const {
            return *pointer_(off);
        }

    private:
        [[nodiscard]] pointer pointer_(const difference_type off = 0) const {
            return static_cast<pointer>(static_cast<void*>(std::next(array_.data_.get(), offset_ + off)));
        }

    private:
        difference_type offset_{ 0 };
        Array& array_;
    };

    using iterator = Iterator<ValueType>;
    using const_iterator = Iterator<ValueType const>;
    using size_type = size_t;
    using pointer = ValueType*;

    using storage_t = std::aligned_storage_t<sizeof(ValueType), alignof(ValueType)>;

    static constexpr pointer to_pointer(storage_t* ptr) {
        return static_cast<pointer>(static_cast<void*>(ptr));
    }

public:
    Array() {
        init();
    };
    explicit Array(size_type const capacity)
        : capacity_{ capacity } {
        init();
    }
    Array(Array const& other)
        : size_{ other.size_ }
        , capacity_{ size_ } {
        init();
        std::copy(
          to_pointer(other.data_.get()),
          to_pointer(std::next(other.data_.get(), other.size_)),
          to_pointer(data_.get()));
    }
    Array(Array&& other) noexcept
        : size_{ other.size_ }
        , capacity_{ size_ } {
        data_.swap(other.data_);
        other.size_ = 0;
        other.capacity_ = 0;
    }
    Array& operator=(Array const& other) {
        if (this != &other)
            [[likely]] {
                size_ = other.size_;
                capacity_ = size_;
                init();
                std::copy(
                  to_pointer(other.data_.get()),
                  to_pointer(std::next(other.data_.get(), other.size_)),
                  to_pointer(data_.get()));
            }
        return *this;
    }
    Array& operator=(Array&& other) noexcept {
        if (this != &other)
            [[likely]] {
                size_ = other.size;
                capacity_ = size_;
                data_.swap(other.data_);
                other.size_ = 0;
                other.capacity_ = 0;
            }
        return *this;
    }
    ~Array() {
        auto* it = to_pointer(data_.get());
        for (size_type idx = 0; idx != size_; std::advance(it, 1), ++idx) {
            it->~ValueType();
        }
    }

    void insert(ValueType const& value) {
        if (size_ >= capacity_)
            [[unlikely]] {
                increase_capacity();
            }
        new (std::next(data_.get(), size_)) ValueType(value);
        ++size_;
    }
    void insert(ValueType&& value) {
        if (size_ >= capacity_)
            [[unlikely]] {
                increase_capacity();
            }
        new (std::next(data_.get(), size_)) ValueType(std::move(value));
        ++size_;
    }
    void insert(size_type index, ValueType const& value) {
        assert(index <= size_ && "Array::insert const failed: out of range");
        if (size_ >= capacity_)
            [[unlikely]] {
                increase_capacity();
            }
        auto* it = std::next(data_.get(), index);
        std::move_backward(
          to_pointer(it),
          to_pointer(std::next(data_.get(), size_)),
          to_pointer(std::next(data_.get(), size_ + 1)));
        new (it) ValueType(value);
        ++size_;
    }
    void insert(size_type index, ValueType&& value) {
        assert(index <= size_ && "Array::insert const failed: out of range");
        if (size_ >= capacity_)
            [[unlikely]] {
                increase_capacity();
            }
        auto* it = std::next(data_.get(), index);
        std::move_backward(
          to_pointer(it),
          to_pointer(std::next(data_.get(), size_)),
          to_pointer(std::next(data_.get(), size_ + 1)));
        new (it) ValueType(std::move(value));
        ++size_;
    }
    void remove(size_type index) {
        assert(index < size_ && "Array::remove failed: out of range");
        auto* it = std::next(data_.get(), index);
        to_pointer(it)->~ValueType();
        std::move(
          to_pointer(std::next(it)),
          to_pointer(std::next(data_.get(), size_)),
          to_pointer(it));
        --size_;
    }
    [[nodiscard]] ValueType const& operator[](size_type index) const {
        assert(index < size_ && "Array::operator[] const failed: out of range");
        return *to_pointer(std::next(data_.get(), index));
    }
    [[nodiscard]] ValueType& operator[](size_type index) {
        assert(index < size_ && "Array::operator[] failed: out of range");
        return *to_pointer(std::next(data_.get(), index));
    }
    [[nodiscard]] size_type size() const {
        return size_;
    }
    [[nodiscard]] const_iterator getIterator() const {
        return begin();
    }
    [[nodiscard]] iterator getIterator() {
        return begin();
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

    [[nodiscard]] const_iterator begin() const {
        return { *this };
    }
    [[nodiscard]] iterator begin() {
        return { *this };
    }
    [[nodiscard]] const_iterator end() const {
        return { *this, static_cast<ptrdiff_t>(size_) };
    }
    [[nodiscard]] iterator end() {
        return { *this, static_cast<ptrdiff_t>(size_) };
    }
    [[nodiscard]] const_iterator cbegin() const {
        return { *this };
    }
    [[nodiscard]] const_iterator cend() const {
        return { *this, static_cast<ptrdiff_t>(size_) };
    }

private:
    void init() {
        data_.reset(new storage_t[capacity_]);
    }
    void increase_capacity() {
        capacity_ = capacity_ * 5 / 3;
        auto* new_data = new storage_t[capacity_];
        std::move(
          to_pointer(data_.get()),
          to_pointer(std::next(data_.get(), size_)),
          to_pointer(new_data));
        data_.reset(new_data);
    }
    struct deleter final {
        void operator()(storage_t* ptr) {
#if _DEBUG
            std::cout << "deleting Array::data_ (" << ptr << ")" << std::endl;
#endif
            delete[] ptr;
        }
    };

public:
    size_type size_{ 0 };
    size_type capacity_{ 4 };
    std::unique_ptr<storage_t[], deleter> data_{ nullptr, deleter{} };
};

} // namespace containers