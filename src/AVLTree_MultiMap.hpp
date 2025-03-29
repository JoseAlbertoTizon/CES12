#ifndef LABTREE_AVLTREE_MULTIMAP_HPP
#define LABTREE_AVLTREE_MULTIMAP_HPP

#include <vector>
#include <utility>
#include <algorithm>

template <typename T1, typename T2>
class AVLTree_MultiMap {
private:
    struct Node;

    class InOrderIterator;

public:
    AVLTree_MultiMap();

    ~AVLTree_MultiMap();

    long size();

    void insert(std::pair<T1, T2>);

    InOrderIterator lower_bound(T1);

    InOrderIterator upper_bound(T1);

private:
    long _size = 0;

    Node* _root = nullptr;

    // Node related methods
    Node* insert(Node*, std::pair<T1, T2>);

    static int get_height(const Node*);  // Returns 0 if nullptr

    static int balance_factor(const Node*);

    static Node* right_rotate(Node*);

    static Node* left_rotate(Node*);

    void erase(Node*);

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

    void operator++();

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
    erase(_root);
    _root = nullptr;
    _size = 0;
}

template <typename T1, typename T2>
long AVLTree_MultiMap<T1, T2>::size() {
    return _size;
}

template <typename T1, typename T2>
void AVLTree_MultiMap<T1, T2>::erase(AVLTree_MultiMap::Node* node) {
    if(not node)
        return;
    erase(node->left);
    erase(node->right);
    delete(node);
}

template <typename T1, typename T2>
void AVLTree_MultiMap<T1, T2>::insert(std::pair<T1, T2> element) {
    _root = insert(_root, element);
    _size ++;
}

template <typename T1, typename T2>
typename AVLTree_MultiMap<T1, T2>::Node* AVLTree_MultiMap<T1, T2>::insert(AVLTree_MultiMap::Node* node, std::pair<T1, T2> element) {
    // First we insert the new element normally
    if(not node)
        return new Node{nullptr, nullptr, nullptr, element, 1};

    if(element.first <= node->values.first) {
        node->left = insert(node->left, element);
        node->left->parent = node;
    }
    else {
        node->right = insert(node->right, element);
        node->right->parent = node;
    }

    // Update the height of the current node
    node->height = 1 + std::max(get_height(node->left), get_height(node->right));

    // Find uneven nodes along the path of the inserted element and fix them
    if(balance_factor(node) > 1) {
        if (element.first <= node->left->values.first)
            node = right_rotate(node);
        else {
            node->left = left_rotate(node->left);
            node = right_rotate(node);
        }
    }
    if(balance_factor(node) < -1) {
        if (element.first > node->right->values.first)
            node = left_rotate(node);
        else {
            node->right = right_rotate(node->right);
            node = left_rotate(node);
        }
    }

    return node;
}

template <typename T1, typename T2>
int AVLTree_MultiMap<T1, T2>::get_height(const AVLTree_MultiMap::Node* node) {
    if(not node)
        return 0;
    return node->height;
}

template <typename T1, typename T2>
int AVLTree_MultiMap<T1, T2>::balance_factor(const AVLTree_MultiMap::Node* node) {
    int left_height = (node->left)? node->left->height: 0;
    int right_height = (node->right)? node->right->height: 0;
    return left_height-right_height;
}

template <typename T1, typename T2>
typename AVLTree_MultiMap<T1, T2>::Node* AVLTree_MultiMap<T1, T2>::right_rotate(AVLTree_MultiMap::Node* node) {
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

    node->height = 1 + std::max(get_height(node->left), get_height(node->right));
    long_branch->height = 1 + std::max(get_height(long_branch->left), get_height(long_branch->right));

    return long_branch;
}

template <typename T1, typename T2>
typename AVLTree_MultiMap<T1, T2>::Node* AVLTree_MultiMap<T1, T2>::left_rotate(AVLTree_MultiMap::Node* node) {
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

    node->height = 1 + std::max(get_height(node->left), get_height(node->right));
    long_branch->height = 1 + std::max(get_height(long_branch->left), get_height(long_branch->right));

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
void AVLTree_MultiMap<T1, T2>::InOrderIterator::operator++() {
    // Logic for inorder next operation considering we are at node and want next element
    // (1) If node has a right child, then the next element will be the leftmost child of his right child (or the right child itself)
    // (2) If node has no right child, we go up until the node you came from is his parent's left child. If we get to nullptr before that, then there's no next
    // (3) If he is his parent's left child, the parent is the next element.
    // (4) Restart loop

    // It should not be possible for node to be null, this if is here just for redundancy
    if(not node)
        return;

    // (1)
    auto current = node;
    if(current->right) {
        current = current->right;
        while(current->left) {
            current = current->left;
        }
        node = current;
        return;
    }

    // (2) and (3)
    while(current->parent) {
        if(current == current->parent->left){
            node = current->parent;
            return;
        }
        current = current->parent;
    }

    // If there is no next element
    node = nullptr;
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
