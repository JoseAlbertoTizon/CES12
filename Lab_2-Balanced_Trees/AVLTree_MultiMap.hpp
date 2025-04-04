#ifndef LABTREE_AVLTREE_MULTIMAP_HPP
#define LABTREE_AVLTREE_MULTIMAP_HPP

#include <vector>
#include <utility>
#include <algorithm>

template <typename T1, typename T2>
class AVLTree_MultiMap {

public:
    class InOrderIterator;

    AVLTree_MultiMap();

    ~AVLTree_MultiMap();

    long size();

    void insert(std::pair<T1, T2>);

    InOrderIterator lower_bound(T1);

    InOrderIterator upper_bound(T1);

private:
    struct Node;

    long _size = 0;

    Node* _root = nullptr;

    // Node related methods
    Node* _insert(Node*, std::pair<T1, T2>);

    static int _get_height(const Node*);  // Returns 0 if nullptr

    static int _balance_factor(const Node*);

    static Node* _right_rotate(Node*);

    static Node* _left_rotate(Node*);

    void _erase(Node*);

};

template <typename T1, typename T2>
struct AVLTree_MultiMap<T1, T2>::Node {
    Node* left;
    Node* right;
    Node* parent;
    std::pair<T1, T2> values;
    int height;
};

template <typename T1, typename T2>
class AVLTree_MultiMap<T1, T2>::InOrderIterator {

public:
    explicit InOrderIterator(AVLTree_MultiMap::Node*);

    std::pair<T1, T2> operator*();

    InOrderIterator operator++();

    bool operator==(const InOrderIterator& other);

    bool operator!=(const InOrderIterator& other);

private:
    AVLTree_MultiMap::Node* node;

};

// Class methods definitions
template <typename T1, typename T2>
AVLTree_MultiMap<T1, T2>::AVLTree_MultiMap() = default;

template<typename T1, typename T2>
AVLTree_MultiMap<T1, T2>::~AVLTree_MultiMap() {
    _erase(_root);
    _root = nullptr;
    _size = 0;
}

template <typename T1, typename T2>
void AVLTree_MultiMap<T1, T2>::_erase(AVLTree_MultiMap::Node* node) {
    if(not node)
        return;
    _erase(node->left);
    _erase(node->right);
    delete(node);
}

template <typename T1, typename T2>
long AVLTree_MultiMap<T1, T2>::size() {
    return _size;
}

template <typename T1, typename T2>
void AVLTree_MultiMap<T1, T2>::insert(std::pair<T1, T2> element) {
    _root = _insert(_root, element);
    _size ++;
}

template <typename T1, typename T2>
typename AVLTree_MultiMap<T1, T2>::Node* AVLTree_MultiMap<T1, T2>::_insert(AVLTree_MultiMap::Node* node, std::pair<T1, T2> element) {
    // First we insert the new element normally
    if(not node)
        return new Node{nullptr, nullptr, nullptr, element, 1};

    if(element.first <= node->values.first) {
        node->left = _insert(node->left, element);
        node->left->parent = node;
    }
    else {
        node->right = _insert(node->right, element);
        node->right->parent = node;
    }

    // Update the height of the current node
    node->height = 1 + std::max(_get_height(node->left), _get_height(node->right));

    // Find uneven nodes along the path of the inserted element and fix them
    if(_balance_factor(node) > 1) {
        if(_get_height(node->left) > _get_height(node->right)) {
            node = _right_rotate(node);
        }
        else {
            node->left = _left_rotate(node->left);
            node = _right_rotate(node);
        }
    }
    if(_balance_factor(node) < -1) {
        if (_get_height(node->left) < _get_height(node->right))
            node = _left_rotate(node);
        else {
            node->right = _right_rotate(node->right);
            node = _left_rotate(node);
        }
    }

    return node;
}

template <typename T1, typename T2>
int AVLTree_MultiMap<T1, T2>::_get_height(const AVLTree_MultiMap::Node* node) {
    if(not node)
        return 0;
    return node->height;
}

template <typename T1, typename T2>
int AVLTree_MultiMap<T1, T2>::_balance_factor(const AVLTree_MultiMap::Node* node) {
    int left_height = (node->left)? node->left->height: 0;
    int right_height = (node->right)? node->right->height: 0;
    return left_height-right_height;
}

template <typename T1, typename T2>
typename AVLTree_MultiMap<T1, T2>::Node* AVLTree_MultiMap<T1, T2>::_right_rotate(AVLTree_MultiMap::Node* node) {
    auto long_branch = node->left;
    auto parent = node->parent;

    // Rotation logic for children pointers
    node->left = long_branch->right;
    long_branch->right = node;

    // Rotation logic for parent pointers
    long_branch->parent = parent;
    node->parent = long_branch;
    if(node->left)
        node->left->parent = node;

    node->height = 1 + std::max(_get_height(node->left), _get_height(node->right));
    long_branch->height = 1 + std::max(_get_height(long_branch->left), _get_height(long_branch->right));

    return long_branch;
}

template <typename T1, typename T2>
typename AVLTree_MultiMap<T1, T2>::Node* AVLTree_MultiMap<T1, T2>::_left_rotate(AVLTree_MultiMap::Node* node) {
    auto long_branch = node->right;
    auto parent = node->parent;

    // Rotation logic for children pointers
    node->right = long_branch->left;
    long_branch->left = node;

    // Rotation logic for parent pointers
    long_branch->parent = parent;
    node->parent = long_branch;
    if(node->right)
        node->right->parent = node;

    node->height = 1 + std::max(_get_height(node->left), _get_height(node->right));
    long_branch->height = 1 + std::max(_get_height(long_branch->left), _get_height(long_branch->right));

    return long_branch;
}

// This returns an iterator to the smallest node greater or equal to first
template <typename T1, typename T2>
typename AVLTree_MultiMap<T1, T2>::InOrderIterator AVLTree_MultiMap<T1, T2>::lower_bound(T1 first) {
    auto node = _root;
    AVLTree_MultiMap::Node* best = nullptr;
    while(node) {
        if(first <= node->values.first) {
            best = node;
            node = node->left;
        }
        else node = node->right;
    }
    return AVLTree_MultiMap::InOrderIterator(best);
}

// This returns an iterator to next node after last
template <typename T1, typename T2>
typename AVLTree_MultiMap<T1, T2>::InOrderIterator AVLTree_MultiMap<T1, T2>::upper_bound(T1 last) {
    auto node = _root;
    AVLTree_MultiMap::Node* best = nullptr;
    while(node) {
        if(last < node->values.first) {
            best = node;
            node = node->left;
        }
        else node = node->right;
    }
    return AVLTree_MultiMap::InOrderIterator(best);
}

template <typename T1, typename T2>
AVLTree_MultiMap<T1, T2>::InOrderIterator::InOrderIterator(AVLTree_MultiMap::Node* node) {
    this->node = node;
}

template <typename T1, typename T2>
std::pair<T1, T2> AVLTree_MultiMap<T1, T2>::InOrderIterator::operator*() {
    return node->values;
}

template <typename T1, typename T2>
typename AVLTree_MultiMap<T1, T2>::InOrderIterator AVLTree_MultiMap<T1, T2>::InOrderIterator::operator++() {
    // Logic for inorder next operation considering we are at node and want next element
    // (1) If node has a right child, then the next element will be the leftmost child of his right child (or the right child itself)
    // (2) If node has no right child, we go up until the node you came from is his parent's left child. If we get to nullptr before that, then there's no next
    // (3) If he is his parent's left child, the parent is the next element.
    // (4) Restart loop

    // If the iterator does not point to a valid node, just return itself
    if(not node)
        return *this;

    // (1)
    auto current = node;
    if(current->right) {
        current = current->right;
        while(current->left) {
            current = current->left;
        }
        node = current;
        return *this;
    }

    // (2) and (3)
    while(current->parent) {
        if(current == current->parent->left){
            node = current->parent;
            return *this;
        }
        current = current->parent;
    }

    // If there is no next element
    node = nullptr;
    return *this;
}

template <typename T1, typename T2>
bool AVLTree_MultiMap<T1, T2>::InOrderIterator::operator==(const AVLTree_MultiMap<T1, T2>::InOrderIterator &other) {
    return node == other.node;
}

template <typename T1, typename T2>
bool AVLTree_MultiMap<T1, T2>::InOrderIterator::operator!=(const AVLTree_MultiMap<T1, T2>::InOrderIterator &other) {
    return node != other.node;
}

#endif
