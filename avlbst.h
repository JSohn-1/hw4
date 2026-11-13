#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    int height(AVLNode<Key, Value>* node) const;

    void updateBalance(AVLNode<Key, Value>* node);

    AVLNode<Key, Value>* rotateLeft(AVLNode<Key, Value>* node);

    AVLNode<Key, Value>* rotateRight(AVLNode<Key, Value>* node);

    AVLNode<Key, Value>* rebalance(AVLNode<Key, Value>* node);


};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert(
    const std::pair<const Key, Value>& new_item)
{
    AVLNode<Key, Value>* current =
        static_cast<AVLNode<Key, Value>*>(this->root_);

    AVLNode<Key, Value>* parent = NULL;

    while (current != NULL) {
        parent = current;

        if (new_item.first < current->getKey()) {
            current = current->getLeft();
        }
        else if (new_item.first > current->getKey()) {
            current = current->getRight();
        }
        else {
            current->setValue(new_item.second);
            return;
        }
    }

    AVLNode<Key, Value>* newNode =
        new AVLNode<Key, Value>(
            new_item.first, new_item.second, parent
        );

    if (parent == NULL) {
        this->root_ = newNode;
    }
    else if (new_item.first < parent->getKey()) {
        parent->setLeft(newNode);
    }
    else {
        parent->setRight(newNode);
    }

    AVLNode<Key, Value>* root =
        rebalance(static_cast<AVLNode<Key, Value>*>(this->root_));

    this->root_ = root;

    if (root != NULL) {
        root->setParent(NULL);
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    AVLNode<Key, Value>* current =
        static_cast<AVLNode<Key, Value>*>(
            this->internalFind(key)
        );

    if (current == NULL) {
        return;
    }

    if (current->getLeft() != NULL &&
        current->getRight() != NULL) {

        AVLNode<Key, Value>* pred =
            static_cast<AVLNode<Key, Value>*>(
                this->predecessor(current)
            );

        nodeSwap(current, pred);
    }

    AVLNode<Key, Value>* child = NULL;

    if (current->getLeft() != NULL) {
        child = current->getLeft();
    }
    else {
        child = current->getRight();
    }

    AVLNode<Key, Value>* parent = current->getParent();

    if (parent == NULL) {
        this->root_ = child;
    }
    else if (parent->getLeft() == current) {
        parent->setLeft(child);
    }
    else {
        parent->setRight(child);
    }

    if (child != NULL) {
        child->setParent(parent);
    }

    delete current;

    AVLNode<Key, Value>* root =
        rebalance(static_cast<AVLNode<Key, Value>*>(this->root_));

    this->root_ = root;

    if (root != NULL) {
        root->setParent(NULL);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


template <class Key, class Value>
int AVLTree<Key, Value>::height(AVLNode<Key, Value>* node) const
{
    if (node == NULL) {
        return -1;
    }

    return 1 + std::max(height(node->getLeft()),
                        height(node->getRight()));
}

template <class Key, class Value>
void AVLTree<Key, Value>::updateBalance(AVLNode<Key, Value>* node)
{
    if (node == NULL) {
        return;
    }

    node->setBalance(
        height(node->getRight()) - height(node->getLeft())
    );
}

template <class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateLeft(
    AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* right = node->getRight();
    AVLNode<Key, Value>* parent = node->getParent();
    AVLNode<Key, Value>* middle = right->getLeft();

    if (parent == NULL) {
        this->root_ = right;
    } else if (parent->getLeft() == node) {
        parent->setLeft(right);
    } else {
        parent->setRight(right);
    }

    right->setParent(parent);

    right->setLeft(node);
    node->setParent(right);

    node->setRight(middle);

    if (middle != NULL) {
        middle->setParent(node);
    }

    updateBalance(node);
    updateBalance(right);

    return right;
}

template <class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateRight(
    AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* left = node->getLeft();
    AVLNode<Key, Value>* parent = node->getParent();
    AVLNode<Key, Value>* middle = left->getRight();

    if (parent == NULL) {
        this->root_ = left;
    } else if (parent->getLeft() == node) {
        parent->setLeft(left);
    } else {
        parent->setRight(left);
    }

    left->setParent(parent);

    left->setRight(node);
    node->setParent(left);

    node->setLeft(middle);

    if (middle != NULL) {
        middle->setParent(node);
    }

    updateBalance(node);
    updateBalance(left);

    return left;
}

template <class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rebalance(
    AVLNode<Key, Value>* node)
{
    if (node == NULL) {
        return NULL;
    }

    AVLNode<Key, Value>* left = rebalance(node->getLeft());
    node->setLeft(left);

    if (left != NULL) {
        left->setParent(node);
    }

    AVLNode<Key, Value>* right = rebalance(node->getRight());
    node->setRight(right);

    if (right != NULL) {
        right->setParent(node);
    }

    updateBalance(node);

    int balance = node->getBalance();

    if (balance > 1) {
        if (node->getRight()->getBalance() < 0) {
            rotateRight(node->getRight());
        }

        return rotateLeft(node);
    }

    if (balance < -1) {
        if (node->getLeft()->getBalance() > 0) {
            rotateLeft(node->getLeft());
        }

        return rotateRight(node);
    }

    return node;
}


#endif
