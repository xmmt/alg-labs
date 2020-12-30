#pragma once

#include <memory>

namespace containers {

template <typename ValueType>
class List final {
public:
    class Chunk;
    using chunk_ptr_t = Chunk*;

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
        using iterator_category = std::bidirectional_iterator_tag;

    public:
        Iterator() = delete;
        Iterator(List& a, Chunk* chunk, difference_type const chunk_offset = 0)
            : list_{ a }
            , current_chunk_{ chunk }
            , chunk_offset_{ chunk_offset } {
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
            current_chunk_->insert(chunk_offset_, value);
        }
        void remove() {
            current_chunk_->remove(chunk_offset_);
        }
        void next() {
            ++chunk_offset_;
            if (chunk_offset_ == current_chunk_->size_)
                [[unlikely]] {
                    current_chunk_ = current_chunk_->nextChunk();
                    chunk_offset_ = 0;
                }
        }
        void prev() {
            --chunk_offset_;
            if (chunk_offset_ < 0)
                [[unlikely]] {
                    current_chunk_ = current_chunk_->prevChunk();
                    chunk_offset_ = current_chunk_->size_ - 1;
                }
        }

        [[nodiscard]] bool hasNext() const {
            return chunk_offset_ < current_chunk_->size_ || current_chunk_->next_;
        }

        [[nodiscard]] bool hasPrev() const {
            return chunk_offset_ > 0 || current_chunk_->prev_;
        }

        /// the methods below are added for compatibility with STL

        Iterator& operator++() {
            next();
            return *this;
        }
        Iterator operator++(int) {
            Iterator result = *this;
            ++(*this);
            return result;
        }
        Iterator& operator--() {
            prev();
            return *this;
        }
        Iterator operator--(int) {
            Iterator result = *this;
            --(*this);
            return result;
        }
        //Iterator operator+(difference_type const off) const {
        //    return { offset_ + off, array_ };
        //}
        //friend Iterator operator+(difference_type const off, Iterator const& it) {
        //    return { it.offset_ + off, it.array_ };
        //}
        //Iterator operator-(difference_type const off) const {
        //    return { offset_ - off, array_ };
        //}
        //difference_type operator-(Iterator const& other) const {
        //    return std::distance(other.pointer_(), pointer_());
        //}
        //Iterator& operator+=(difference_type const off) {
        //    offset_ += off;
        //    return *this;
        //}
        //Iterator& operator-=(difference_type const off) {
        //    offset_ -= off;
        //    return *this;
        //}
        //auto operator<=>(Iterator const& other) const {
        //    return pointer_() <=> other.pointer_();
        //}
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
            return to_pointer(std::next(current_chunk_->chunk_, chunk_offset_));
        }

    private:
        List& list_;
        Chunk* current_chunk_{ nullptr };
        difference_type chunk_offset_{ 0 };
    };

    using iterator = Iterator<ValueType>;
    using const_iterator = Iterator<ValueType const>;
    using size_type = size_t;
    using pointer = ValueType*;

    using storage_t = std::aligned_storage_t<sizeof(ValueType), alignof(ValueType)>;

    static constexpr pointer to_pointer(storage_t* ptr) {
        return static_cast<pointer>(static_cast<void*>(ptr));
    }

    List() {
    }
    List(List const&) = default;
    List(List&&) = default;
    List& operator=(List const&) = default;
    List& operator=(List&&) = default;
    ~List() = default;

    void insertHead(ValueType const& value) {
        head_chunk_.insert(0, value);
    }
    void insertTail(ValueType const& value) {
        tail_chunk_->insert(tail_chunk_->size_, value);
    }
    void removeHead() {
        head_chunk_.remove(0);
    }
    void removeTail() {
        tail_chunk_->remove(tail_chunk_->size_);
    }

    [[nodiscard]] ValueType const& head() const {
        return *to_pointer(head_chunk_.chunk_);
    }
    [[nodiscard]] ValueType const& tail() const {
        return *to_pointer(std::next(tail_chunk_->chunk_, tail_chunk_->size_ - 1));
    }
    [[nodiscard]] size_type size() const {
        return size_;
    }
    [[nodiscard]] const_iterator getIterator() const {
        return { *this, &head_chunk_, 0 };
    }
    [[nodiscard]] iterator getIterator() {
        return { *this, &head_chunk_, 0 };
    }

    /// the methods below are added for compatibility with STL

public:
    //List(std::initializer_list<ValueType> values)
    //    : size_(values.size())
    //    , capacity_(size_)
    //    , data_(new ValueType[size_]) {
    //    assert(data_.get() != nullptr && "");
    //    std::copy(values.begin(), values.end(), data_.get());
    //}

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
    //void init() {
    //    data_.reset(new storage_t[capacity_]);
    //}
    //void increase_capacity() {
    //    capacity_ = capacity_ * 5 / 3;
    //    auto* new_data = new storage_t[capacity_];
    //    std::move(
    //      to_pointer(data_.get()),
    //      to_pointer(std::next(data_.get(), size_)),
    //      to_pointer(new_data));
    //    data_.reset(new_data);
    //}
    struct deleter final {
        void operator()(storage_t* ptr) {
#if _DEBUG
            std::cout << "deleting Array::data_ (" << ptr << ")" << std::endl;
#endif
            delete[] ptr;
        }
    };

    [[nodiscard]] size_type capacity() const {
        return chunks_count_ * k_chunk_size;
    }

public:
    //static constexpr size_type k_chunk_size{ (64 + sizeof(ValueType) - 1) / sizeof(ValueType) };
    static constexpr size_type k_chunk_size{ 1 };
    size_type chunks_count_{ 1 };
    size_type size_{ 0 };

    class Chunk final {
    public:
        Chunk() = default;
        explicit Chunk(List& list, chunk_ptr_t const prev, chunk_ptr_t const next = nullptr)
            : list_{ list }
            , next_{ next }
            , prev_{ prev } {
        }
        Chunk(Chunk const&) = delete;
        Chunk(Chunk&&) = delete;
        Chunk& operator=(Chunk const&) = delete;
        Chunk& operator=(Chunk&&) = delete;
        ~Chunk() {
            auto* it = chunk_;
            for (size_type i = 0; i < size_; std::advance(it, 1), ++i) {
                to_pointer(it)->~ValueType();
            }
        }

        ValueType const& operator[](size_type idx) const {
            return chunk_[idx];
        }
        ValueType& operator[](size_type idx) {
            return chunk_[idx];
        }

        [[nodiscard]] chunk_ptr_t nextChunk() const {
            return next_;
        }
        [[nodiscard]] chunk_ptr_t prevChunk() const {
            return prev_;
        }
        [[nodiscard]] bool hasNextChunk() const {
            return next_ != nullptr;
        }
        [[nodiscard]] bool hasPrevChunk() const {
            return prev_ != nullptr;
        }

        void remove(size_type idx) {
            to_pointer(std::next(chunk_, idx))->~ValueType();
            std::move(
              to_pointer(std::next(chunk_, idx + 1)),
              to_pointer(std::next(chunk_, size_)),
              to_pointer(std::next(chunk_, idx)));
            --size_;
            if (size_ == 0)
                [[unlikely]] {
                    if (prev_) {
                        prev_->next_ = next_;
                        if (!prev_->next_) {
                            list_.tail_chunk_ = prev_;
                        }
                    }
                }
        }

        void insert(size_type idx, ValueType const& value) {
            if (size_ == k_chunk_size)
                [[unlikely]] {
                    next_ = std::make_unique<Chunk>(list_, this, next_.release());
                    if (!next_->next_) {
                        list_.tail_chunk_ = next_.get();
                    }
                    std::move(
                      to_pointer(std::next(chunk_, idx)),
                      to_pointer(std::next(chunk_, size_)),
                      to_pointer(next_->chunk_));
                    next_->size_ = size_ - idx;
                    size_ -= size_ - idx;
                }
            std::move_backward(
              to_pointer(std::next(chunk_, idx)),
              to_pointer(std::next(chunk_, size_)),
              to_pointer(std::next(chunk_, size_ + 1)));
            new (std::next(chunk_, idx)) ValueType{ value };
            ++size_;
        }

        size_type size_{ 0 };
        storage_t chunk_[k_chunk_size];

        List& list_;
        std::unique_ptr<Chunk> next_{ nullptr };
        chunk_ptr_t prev_{ nullptr };
    };

    Chunk head_chunk_{ *this, nullptr, nullptr };
    chunk_ptr_t tail_chunk_{ &head_chunk_ };
};

} // namespace containers