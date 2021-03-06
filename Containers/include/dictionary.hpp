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
    class rbnode final {
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
        //void replace_node(rbnode_ptr_t child) {
        //    child->parent = parent;
        //    if (this == parent->left.get()) {
        //        parent->left.release();
        //        parent->left.reset(child);
        //    } else {
        //        parent->right.release();
        //        parent->right.reset(child);
        //    }
        //}
        //
        //void delete_one_child() {
        //    auto child = right->is_leaf() ? left.get() : right.get();
        //    replace_node(child);
        //    if (color == color::black) {
        //        if (child->color == color::red)
        //            child->color = color::black;
        //        else {
        //            child->delete_case_1();
        //        }
        //    }
        //    delete this;
        //}
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
            if (parent->color == color::black && s->color == color::black && (!s->left || s->left->color == color::black) && (!s->right || s->right->color == color::black)) {
                s->color = color::red;
                parent->delete_case_1();
            } else {
                delete_case_4();
            }
        }
        void delete_case_4() {
            auto s = sibling();
            if (parent->color == color::red && s->color == color::black && (!s->left || s->left->color == color::black) && (!s->right || s->right->color == color::black)) {
                s->color = color::red;
                parent->color = color::black;
            } else {
                delete_case_5();
            }
        }
        void delete_case_5() {
            auto s = sibling();

            if (s->color == color::black) {
                if (this == parent->left.get() && (!s->right || s->right->color == color::black) && (s->left && s->left->color == color::red)) {
                    s->color = color::red;
                    s->left->color = color::black;
                    s->rotate_right();
                } else if (this == parent->right.get() && (!s->left || s->left->color == color::black) && (s->right && s->right->color == color::red)) {
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
                if (s->right) {
                    s->right->color = color::black;
                }
                parent->rotate_left();
            } else {
                if (s->left) {
                    s->left->color = color::black;
                }
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
    class Iterator final {
    public:
        Iterator(rbnode_ptr_t node)
            : node_{ node } {
        }
        Iterator(Iterator const&) = default;
        Iterator(Iterator&&) = default;
        Iterator& operator=(Iterator const&) = default;
        Iterator& operator=(Iterator&&) = default;
        ~Iterator() = default;
        KeyType const& key() const {
            static KeyType key{};
            return key;
        }
        ValueType const& get() const {
            static ValueType value{};
            return value;
        }
        void set(ValueType const& value) {
            node_->value = value;
        }
        void next() {
            auto s = node_;
            while (!s->right && s->parent) {
                s = s->parent;
            }
            if (s->right) {
                node_ = s->right.get();
            }
        }
        void prev() {
            auto s = node_;
            while (!s->left && s->parent) {
                s = s->parent;
            }
            if (s->left) {
                node_ = s->left.get();
            }
        }

        [[nodiscard]] bool hasNext() const {
            auto s = node_;
            while (!s->right && s->parent) {
                s = s->parent;
            }
            if (s->right) {
                return true;
            }
            return false;
        }

        [[nodiscard]] bool hasPrev() const {
            auto s = node_;
            while (!s->left && s->parent) {
                s = s->parent;
            }
            if (s->left) {
                return true;
            }
            return false;
        }

    private:
        rbnode_ptr_t node_{ nullptr };
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
            //auto* p = reinterpret_cast<rbnode*>(allocator_.allocate(1));
            //new (p) rbnode{ key, value };
            //root.reset(p);
            root.reset(new rbnode{ key, value });
            root->insert_case_1();
            ++size_;
        } else {
            auto h = hash(key);
            auto s = root->lookup(key, h);
            if (s->hash_ == h && s->key == key) {
                s->value = value;
            } else {
                //auto* new_node = reinterpret_cast<rbnode*>(allocator_.allocate(1));
                //new (new_node) rbnode{ key, value, rbnode::color::red, s };
                auto new_node = new rbnode{ key, value, rbnode::color::red, s };
                if (h < s->hash_) {
                    s->left.reset(new_node);
                } else {
                    s->right.reset(new_node);
                }
                new_node->insert_case_1();
                ++size_;
            }
        }
        update_root();
    }

    void remove(KeyType const& key) {
        if (!root) {
            return;
        }
        auto h = hash(key);
        auto s = root->lookup(key, h);
        if (s->hash_ != h || s->key != key) {
            return;
        }
        if (s->left && s->right) {
            auto t = s->right->lookup(key, h);
            s->key = std::move(t->key);
            s->value = std::move(t->value);
            s = t;
        }
        if (s->color == rbnode::color::red) {
            if (s->parent) {
                if (s->parent->left.get() == s) {
                    s->parent->left.reset(nullptr);
                } else {
                    s->parent->right.reset(nullptr);
                }
            } else {
                root.reset(nullptr);
            }
        } else {
            auto c = s->left ? s->left.get() : s->right.get();
            if (c && c->color == rbnode::color::red) {
                c->color = rbnode::color::black;
                if (c == s->left.get()) {
                    s->left.release();
                } else if (c == s->right.get()) {
                    s->right.release();
                }
                if (s->parent) {
                    c->parent = s->parent;
                    if (s->parent->left.get() == s) {
                        s->parent->left.reset(c);
                    } else {
                        s->parent->right.reset(c);
                    }
                } else {
                    root.reset(c);
                    c->parent = nullptr;
                }
            } else {
                s->delete_case_1();
                if (s->parent) {
                    if (s->parent->left.get() == s) {
                        s->parent->left.reset(nullptr);
                    } else {
                        s->parent->right.reset(nullptr);
                    }
                } else {
                    root.reset(nullptr);
                }
            }
        }
        update_root();
        --size_;
    }

    bool contains(KeyType const& key) {
        if (!root) {
            return false;
        }
        auto h = hash(key);
        auto s = root->lookup(key, h);
        if (h == s->hash_ && key == s->key) {
            return true;
        }
        return false;
    }

    ValueType const&
    operator[](KeyType const& key) const {
        static ValueType value{};
        if (root) {
            auto h = hash(key);
            auto s = root->lookup(key, h);
            if (h == s->hash_ && key == s->key) {
                return s->value;
            }
        }
        return value;
    }

    ValueType& operator[](KeyType const& key) {
        if (!root) {
            put(key, {});
            return root->value;
        }
        auto h = hash(key);
        auto s = root->lookup(key, h);
        if (h == s->hash_ && key == s->key) {
            return s->value;
        }
        put(key, {});
        s = root->lookup(key, h);
        return s->value;
    }

    size_type size() const {
        return size_;
    }

    iterator getIterator() const {
        return {};
    }
    const_iterator getIterator() {
        return {};
    }

public:
    void update_root() {
        if (root && root->parent) {
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
    size_type size_{ 0 };
    //typename std::map<int, std::aligned_storage_t<sizeof(rbnode) - 4, alignof(rbnode)>>::allocator_type allocator_;
    //typename std::vector<std::aligned_storage_t<sizeof(rbnode), alignof(rbnode)>>::allocator_type allocator_;
};

} // namespace containers
