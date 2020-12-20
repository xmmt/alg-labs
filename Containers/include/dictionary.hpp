#pragma once

#include <memory>

namespace containers {

template <typename KeyType, typename ValueType>
class Dictionary final {
public:
    static constexpr std::hash<KeyType> hash{};

public:
    class rbnode;
    using rbnode_ptr_t = rbnode*;
    class rbnode {
    public:
        enum class color : uint8_t {
            black,
            red,
        };

        rbnode(KeyType const& k, ValueType const& v, const color c = color::red, const rbnode_ptr_t p = nullptr, rbnode_ptr_t l = nullptr, rbnode_ptr_t r = nullptr)
            : key{ k }
            , value{ v }
            , color{ c }
            , parent{ p }
            , left{ l }
            , right{ r }
            , hash_{ hash(k) } {
        }

        bool is_leaf() {
            return !(left && right);
        }
        [[nodiscard]] rbnode_ptr_t grandparent() const {
            if (parent) {
                return parent->parent;
            } else {
                return nullptr;
            }
        }

        [[nodiscard]] rbnode_ptr_t uncle() const {
            auto g = grandparent();
            if (g == nullptr) {
                return nullptr;
            }
            if (parent == g->left.get()) {
                return g->right.get();
            }
            return g->left.get();
        }

        [[nodiscard]] rbnode_ptr_t sibling() const {
            if (this == parent->left.get()) {
                return parent->right.get();
            } else {
                return parent->left.get();
            }
        }
        void rotate_left() {
            auto pivot = right.release();
            pivot->parent = parent;
            if (parent) {
                if (parent->left.get() == this) {
                    parent->left.release();
                    parent->left.reset(pivot);
                } else {
                    parent->right.release();
                    parent->right.reset(pivot);
                }
            }
            right = std::move(pivot->left);
            if (right) {
                right->parent = this;
            }
            parent = pivot;
            pivot->left.reset(this);
        }
        void rotate_right() {
            auto pivot = left.release();
            pivot->parent = parent;
            if (parent) {
                if (parent->left.get() == this) {
                    parent->left.release();
                    parent->left.reset(pivot);
                } else {
                    parent->right.release();
                    parent->right.reset(pivot);
                }
            }
            left = std::move(pivot->right);
            if (left) {
                left->parent = this;
            }
            parent = pivot;
            pivot->right.reset(this);
        }
        void insert_case_1() {
            if (!parent) {
                color = color::black;
            } else {
                insert_case_2();
            }
        }
        void insert_case_2() {
            if (parent->color == color::red) {
                insert_case_3();
            }
        }
        void insert_case_3() {
            auto u = uncle();
            if (u && u->color == color::red) {
                parent->color = color::black;
                u->color = color::black;
                auto g = grandparent();
                g->color = color::red;
                g->insert_case_1();
            } else {
                insert_case_4();
            }
        }
        void insert_case_4() {
            auto g = grandparent();
            if (this == parent->right.get() && parent == g->left.get()) {
                parent->rotate_left();
                left->insert_case_5();
                return;
            } else if (this == parent->left.get() && parent == g->right.get()) {
                parent->rotate_right();
                right->insert_case_5();
                return;
            }
            insert_case_5();
        }
        void insert_case_5() {
            auto g = grandparent();
            parent->color = color::black;
            g->color = color::red;
            if (this == parent->left.get() && parent == g->left.get()) {
                g->rotate_right();
            } else { /* this == parent->right.get() && parent == g->right.get() */
                g->rotate_left();
            }
        }
        void replace_node(rbnode_ptr_t child) {
            child->parent = parent;
            if (this == parent->left.get()) {
                parent->left.release();
                parent->left.reset(child);
            } else {
                parent->right.release();
                parent->right.reset(child);
            }
        }

        void delete_one_child() {
            auto child = right->is_leaf() ? left.get() : right.get();
            replace_node(child);
            if (color == color::black) {
                if (child->color == color::red)
                    child->color = color::black;
                else {
                    child->delete_case_1();
                }
            }
            delete this;
        }
        void delete_case_1() {
            if (parent) {
                delete_case_2();
            }
        }
        void delete_case_2() {
            auto s = sibling();
            if (s->color == color::red) {
                parent->color = color::red;
                s->color = color::black;
                if (this == parent->left.get()) {
                    parent->rotate_left();
                } else {
                    parent->rotate_right();
                }
            }
            delete_case_3();
        }
        void delete_case_3() {
            auto s = sibling();
            if (parent->color == color::black && s->color == color::black && s->left->color == color::black && s->right->color == color::black) {
                s->color = color::red;
                parent->delete_case_1();
            } else {
                delete_case_4();
            }
        }
        void delete_case_4() {
            auto s = sibling();
            if (parent->color == color::red && s->color == color::black && s->left->color == color::black && s->right->color == color::black) {
                s->color = color::red;
                parent->color = color::black;
            } else {
                delete_case_5();
            }
        }
        void delete_case_5() {
            auto s = sibling();

            if (s->color == color::black) {
                if (this == parent->left.get() && s->right->color == color::black && s->left->color == color::red) {
                    s->color = color::red;
                    s->left->color = color::black;
                    s->rotate_right();
                } else if (this == parent->right.get() && s->left->color == color::black && s->right->color == color::red) {
                    s->color = color::red;
                    s->right->color = color::black;
                    s->rotate_left();
                }
            }
            delete_case_6();
        }
        void delete_case_6() {
            auto s = sibling();

            s->color = parent->color;
            parent->color = color::black;

            if (this == parent->left.get()) {
                s->right->color = color::black;
                parent->rotate_left();
            } else {
                s->left->color = color::black;
                parent->rotate_right();
            }
        }

        [[nodiscard]] rbnode_ptr_t lookup(KeyType const& k, size_t h) const {
            if (h == hash_ && key == k) {
                return const_cast<rbnode_ptr_t>(this);
            }
            if (h < hash_ && left) {
                return left->lookup(k, h);
            } else if (h >= hash_ && right) {
                return right->lookup(k, h);
            }
            return const_cast<rbnode_ptr_t>(this);
        }

        [[nodiscard]] rbnode_ptr_t lookup(KeyType const& k) const {
            return lookup(k, hash(k));
        }

    public:
        KeyType key;
        ValueType value;
        color color{ color::red };
        rbnode_ptr_t parent{ nullptr };
        std::unique_ptr<rbnode> left{ nullptr }, right{ nullptr };
        size_t hash_{ 0 };
    };

public:
    template <typename IterValueType>
    class Iterator {
        KeyType const& key() const {
            static KeyType key{};
            return key;
        }
        ValueType const& get() const {
            static ValueType value{};
            return value;
        }
        void set(ValueType const& value) {
        }
        void next() {
        }
        void prev() {
        }
        bool hasNext() {
            return true;
        }
        bool hasPrev() {
            return true;
        }
    };

public:
    using size_type = size_t;
    using iterator = Iterator<ValueType>;
    using const_iterator = Iterator<ValueType const>;

    Dictionary() = default;
    Dictionary(Dictionary const&) = default;
    Dictionary(Dictionary&&) = default;
    Dictionary& operator=(Dictionary const&) = default;
    Dictionary& operator=(Dictionary&&) = default;
    ~Dictionary() = default;

    void put(KeyType const& key, ValueType const& value) {
        if (!root) {
            root.reset(new rbnode{ key, value });
            root->insert_case_1();
        } else {
            auto h = hash(key);
            auto s = root->lookup(key);
            if (s->hash_ == h && s->key == key) {
                s->value = value;
            } else {
                auto new_node = new rbnode{ key, value, rbnode::color::red, s };
                if (h < s->hash_) {
                    s->left.reset(new_node);
                } else {
                    s->right.reset(new_node);
                }
                new_node->insert_case_1();
            }
        }
        update_root();
    }

    void remove(KeyType const& key) {
    }

    bool contains(KeyType const& key) {
        return true;
    }

    ValueType const& operator[](KeyType const& key) const {
        static ValueType value{};
        return value;
    }

    ValueType& operator[](KeyType const& key) {
        static ValueType value{};
        return value;
    }

    size_type size() const {
        return 0;
    }

    iterator getIterator() const {
        return {};
    }
    const_iterator getIterator() {
        return {};
    }

public:
    void update_root() {
        if (root->parent) {
            auto p = root->parent;
            while (p->parent) {
                p = p->parent;
            }
            root.release();
            root.reset(p);
        }
    }

public:
    std::unique_ptr<rbnode> root{ nullptr };
};

} // namespace containers
