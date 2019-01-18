#ifndef LAB4_AVLTREE_HARD_HPP
#define LAB4_AVLTREE_HARD_HPP

#include <memory>
#include <exception>
#include <set>
#include <vector>

#include "IBSTree.hpp"


template <class K, class V>
class AVLTree : public IBSTree<K, V>{
    struct Node {
        V data;
        K key;
        int height;
        std::shared_ptr<Node> left, right;

        Node(K key, V value): key(key), data(value), height(0), left(nullptr), right(nullptr) {}
    };

    typedef std::shared_ptr<Node> Node_ptr;

    Node_ptr root;
    int size_;


    int height(Node_ptr t) {
        return (t == nullptr ? -1 : t->height);
    }

    Node_ptr rightRotate(Node_ptr node) {
        Node_ptr u = node->left;
        node->left = u->right;
        u->right = node;
        node->height = std::max(height(node->left), height(node->right)) + 1;
        u->height = std::max(height(u->left), node->height) + 1;
        return u;
    }

    Node_ptr leftRotate(Node_ptr node) {
        Node_ptr u = node->right;
        node->right = u->left;
        u->left = node;
        node->height = std::max(height(node->left), height(node->right))+1;
        u->height = std::max(height(node->right), node->height)+1 ;
        return u;
    }

    Node_ptr doubleLeftRotate(Node_ptr node) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    Node_ptr doubleRightRotate(Node_ptr t) {
        t->left = leftRotate(t->left);
        return rightRotate(t);
    }

    Node_ptr findMin(Node_ptr t) {
        if (t == nullptr)
            return nullptr;
        for (; t->left; t = t->left);
        return t;
    }

    Node_ptr findMax(Node_ptr t) {
        if (t == nullptr)
            return nullptr;
        for (; t->right; t = t->right);
        return t;
    }

    Node_ptr remove(K k, Node_ptr t) {
        if(not t) return nullptr;

        if(k < t->key)
            t->left = remove(k, t->left);
        else if(k > t->key)
            t->right = remove(k, t->right);
        else { // Элемент найден
            if (not t->left)       t = t->right;
            else if (not t->right) t = t->left;
            else {
                auto temp = findMin(t->right);
                t->data = temp->data;
                t->key = temp->key;
                t->right = remove(t->key, t->right);
            }

            if (not t) return nullptr;
        }

        t->height = std::max(height(t->left), height(t->right)) + 1;

        if(height(t->left) - height(t->right) == 2) { // поддерево не сбалансировано и левое > правого
            if(height(t->left->left) - height(t->left->right) == 1)
                return leftRotate(t);
            else
                return doubleLeftRotate(t);
        } else if(height(t->right) - height(t->left) == 2) { // поддерево не сбалансировано и правое > левого
            if(height(t->right->right) - height(t->right->left) == 1)
                return rightRotate(t);
            else
                return doubleRightRotate(t);
        }
        return t;
    }

    Node_ptr insert(K key, V value, Node_ptr t) {
        if(not t) {
            t = std::make_shared<Node>(key, value);
        } else if(key < t->key) {
            t->left = insert(key, value, t->left);
            if (height(t->left) - height(t->right) == 2) {
                if(key < t->left->key)
                    t = rightRotate(t);
                else
                    t = doubleRightRotate(t);
            }
        } else if(key > t->key) {
            t->right = insert(key, value, t->right);
            if(height(t->right) - height(t->left) == 2) {
                if(key > t->right->data)
                    t = leftRotate(t);
                else
                    t = doubleLeftRotate(t);
            }
        }

        t->height = std::max(height(t->left), height(t->right)) + 1;
        return t;
    }

    Node_ptr find(K key) {
        Node_ptr res = root;
        while (res and res->key != key)
            res = key < res->key ? res->left : res->right;
        return res;
    }

public:
    AVLTree(): root(nullptr), size_(0) {}

    void remove(K key) override {
        root = remove(key, root);
        size_--;
    }

    void put(K key, V value) override {
        root = insert(key, value, root);
        size_++;
    }

    V getOrDefault(K key, V defaultValue) override {
        auto tmp = find(key);
        return tmp ? tmp->data : defaultValue;
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

        visit(root);
    }

    void foreach_prefix(function<void(K,V)> consume) override {
        function<void(Node_ptr)> visit = [&](Node_ptr root) {
            if (not root) return;

            consume(root->key, root->data);
            visit(root->left);
            visit(root->right);
        };

        visit(root);
    }

    void foreach_postfix(function<void(K,V)> consume) override {
        function<void(Node_ptr)> visit = [&](Node_ptr root) {
            if (not root) return;

            visit(root->left);
            visit(root->right);
            consume(root->key, root->data);
        };

        visit(root);
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
};

#endif //LAB4_AVLTREE_HARD_HPP
