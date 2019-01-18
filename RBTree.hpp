#ifndef LAB4_RBTREE_HARD_HPP
#define LAB4_RBTREE_HARD_HPP

#include "IBSTree.hpp"
#include "memory"


template <class K, class V>
class RBTree : public IBSTree<K, V> {
private:
    struct Node {
        V data;
        K key;
        bool is_black;

        std::shared_ptr<Node> left, right, parent;

        Node(K key, V value, bool is_black): key(key), data(value), left(nullptr), right(nullptr),
                                            parent(nullptr), is_black(is_black) {}
    };

    typedef std::shared_ptr<Node> Node_ptr;

    Node_ptr root_;
    int size_;


    static bool is_black(Node_ptr node) {
        if (node) return node->is_black;
        else return true;
    }

    static Node_ptr findMin(Node_ptr t) {
        if (t == nullptr)
            return nullptr;

        for (; t->left; t = t->left);
        return t;
    }

    static Node_ptr findMax(Node_ptr t) {
        if (t == nullptr)
            return nullptr;

        for (; t->right; t = t->right);
        return t;
    }

    static Node_ptr insert(Node_ptr root, Node_ptr ptr) {
        if (not root) {
            return ptr;
        }

        if (ptr->key < root->key) {
            root->left = insert(root->left, ptr);
            root->left->parent = root;
        } else if (ptr->key > root->key) {
            root->right = insert(root->right, ptr);
            root->right->parent = root;
        } else {
            root->data = ptr->data;
        }

        return root;
    }

    void fixInsert(Node_ptr ptr) {
        Node_ptr parent = nullptr, grandparent = nullptr;

        while (ptr != root_ and not is_black(ptr) and not is_black(ptr->parent)) { // пока есть нарушение красный-красный
            parent = ptr->parent;
            grandparent = parent->parent;

            if (parent == grandparent->left) {
                if (Node_ptr uncle = grandparent->right; not is_black(uncle)) {
                    uncle->is_black = true;
                    parent->is_black = true;
                    grandparent->is_black = false;
                    ptr = grandparent;
                } else {
                    if (ptr == parent->right) {
                        rotateLeft(parent);
                        ptr = parent;
                        parent = ptr->parent;
                    }
                    rotateRight(grandparent);
                    std::swap(parent->is_black, grandparent->is_black);
                    ptr = parent;
                }
            } else {
                if (Node_ptr uncle = grandparent->left; not is_black(uncle)) {
                    uncle->is_black = true;
                    parent->is_black = true;
                    grandparent->is_black = false;
                    ptr = grandparent;
                } else {
                    if (ptr == parent->left) {
                        rotateRight(parent);
                        ptr = parent;
                        parent = ptr->parent;
                    }
                    rotateLeft(grandparent);
                    std::swap(parent->is_black, grandparent->is_black);
                    ptr = parent;
                }
            }
        }

        root_->is_black = true;
    }

    void rotateLeft(Node_ptr ptr) {
        Node_ptr right_child = ptr->right;
        ptr->right = right_child->left;

        if (ptr->right)
            ptr->right->parent = ptr;

        right_child->parent = ptr->parent;

        if (not ptr->parent)
            root_ = right_child;
        else if (ptr == ptr->parent->left)
            ptr->parent->left = right_child;
        else
            ptr->parent->right = right_child;

        right_child->left = ptr;
        ptr->parent = right_child;
    }

    void rotateRight(Node_ptr ptr) {
        Node_ptr left_child = ptr->left;
        ptr->left = left_child->right;

        if (ptr->left)
            ptr->left->parent = ptr;

        left_child->parent = ptr->parent;

        if (not ptr->parent)
            root_ = left_child;
        else if (ptr == ptr->parent->left)
            ptr->parent->left = left_child;
        else
            ptr->parent->right = left_child;

        left_child->right = ptr;
        ptr->parent = left_child;
    }

    static Node_ptr prepareToRemove(Node_ptr root, K key) {
        if (not root) return nullptr;

        if (key < root->key)
            return prepareToRemove(root->left, key);

        if (key > root->key)
            return prepareToRemove(root->right, key);

        if (not root->right) {
            if (root->left) {
                // это чтобы никогда у удаляемого элемента небыло левого сына
                Node *temp = findMax(root->left).get();
                root->key = temp->key;
                root->data = temp->data;
                return prepareToRemove(root->right, temp->key);
            } else return root;
        }

        Node_ptr temp = findMin(root->right);
        root->key = std::move(temp->key);
        root->data = std::move(temp->data);
        return prepareToRemove(root->right, temp->key);
    }

    void remove(Node_ptr node) {
        // node ещё не удалена, но не имеет левого и, возможно, правого потомка

        if (not node) return; // удаляется не существующий элемент. ошибку кидать не будем)

        if (not node->parent) { // нода это корень!
            root_ = node->right;
            root_->is_black = true;
            if (node->right)
                node->right->parent = nullptr;
            return;
        }

        (node->parent->left == node ? node->parent->left : node->parent->right) = node->right; // отец не знает ноду
        if (not node->right)
            return;
        node->right->parent = node->parent; // сын не знает ноду

        if (not is_black(node)){
            // правила не нарушаются. просто уходим
            return;
        }

        // длинна черного пути уменьшелась. так же возможна ошибка красный-красный
        if (not is_black(node->right)) {
            // сделаем сына черным и всё ок
            node->right->is_black = true;
        } else if (not is_black(node->parent)) {
            // сделаем отца черным и всё ок
            node->parent->is_black = true;
        } else {
            if (node->parent->left == node)
                rotateRight(node);
            else
                rotateLeft(node);
        }
    }

public:
    void put(K key, V value) override {
        Node_ptr new_node = std::make_shared<Node>(key, value, false);
        root_ = root_ ? insert(root_, new_node) : new_node;
        fixInsert(new_node);
        size_++;
    }

    void remove(K key) override {
        remove(prepareToRemove(root_, key));
    }

    V getOrDefault(K key, V defaultValue) override {
        auto it = root_;

        // ищем нужную ноду
        while (it and it->key != key) {
            if (key < it->key)
                it = it->left;
            else
                it = it->right;
        }

        if (it)
            return it->data;
        else
            return defaultValue;
    }

    set<K> keys() override {
        set<K> res;

        foreach_infix([&res](K key, V value) {
            res.insert(key);
        });

        return res;
    };

    vector<V> values() override {
        vector<V> res;
        res.reserve(size_);

        foreach_infix([&res](K key, V value) {
            res.emplace_back(value);
        });

        return res;
    }

    int size() override {
        return size_;
    }

    bool empty() override {
        return size_ == 0;
    }

    void foreach_infix(function<void(K,V)> consume) override {
        function<void(Node_ptr)> visit = [&](Node_ptr root) {
            if (not root) return;

            visit(root->left);
            consume(root->key, root->data);
            visit(root->right);
        };

        visit(root_);
    }

    void foreach_prefix(function<void(K,V)> consume) override {
        function<void(Node_ptr)> visit = [&](Node_ptr root) {
            if (not root) return;

            consume(root->key, root->data);
            visit(root->left);
            visit(root->right);
        };

        visit(root_);
    }

    void foreach_postfix(function<void(K,V)> consume) override {
        function<void(Node_ptr)> visit = [&](Node_ptr root) {
            if (not root) return;

            visit(root->left);
            visit(root->right);
            consume(root->key, root->data);
        };

        visit(root_);
    }
};

#endif //LAB4RB_TREE_HARD_HPP
