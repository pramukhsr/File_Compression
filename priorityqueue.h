// priorityqueue.h
//
// The purpose of this program is to create a binary search tree for a
// priorityqueue. This means that some values in the tree will have the same
// priority so there are three dimension to the bst.
#pragma once

#include <iostream>
#include <sstream>
#include <set>

using namespace std;

template<typename T>
class priorityqueue {
private:
    struct NODE {
        int priority;  // used to build BST
        T value;  // stored data for the p-queue
        bool dup;  // marked true when there are duplicate priorities
        NODE* parent;  // links back to parent
        NODE* link;  // links to linked list of NODES with duplicate priorities
        NODE* left;  // links to left child
        NODE* right;  // links to right child
    };
    NODE* root;  // pointer to root node of the BST
    int size;  // # of elements in the pqueue
    NODE* curr;  // pointer to next item in pqueue (see begin and next)
    
    // This function is used for toString to ouput the values from the
    // tree for inorder traversal.
    void _recursiveFunciton(NODE* node, ostream& output){
        if(node == NULL) {
            return;
        } else {
            _recursiveFunciton(node->left, output);
            output << node->priority << " value: " << node->value << endl;
            _recursiveFunciton(node->link, output);
            _recursiveFunciton(node->right, output);
        }
    }
    // This function is used to traverse the other bst and make copies from
    // the original. The enqueue fucntion is called here to copy the values
    // down and preorder traveral is used here.
    void _recursiveCopy(NODE* node) {
        if(node == NULL) {
            return;
        } else {
            enqueue(node->value, node->priority);
            _recursiveCopy(node->link);
            _recursiveCopy(node->left);
            _recursiveCopy(node->right);
        }
    }
    // This clear function is used to traverse through a tree and clear
    // all of the nodes using postorder traversal.
    void _recursiveClear(NODE* node) {
        if(node == NULL) {
            return;
        } else {
            _recursiveClear(node->left);
            _recursiveClear(node->right);
            _recursiveClear(node->link);
            delete node;
        }
    }
    // This function is the helper function used for the equality operator
    // function. This was primarily taken from Professor Reckinger's slides
    // for AVL trees. It was then modified to account for the extra features
    // of our project like links and values.
    bool _recursiveEqual(NODE* root, NODE* otherRoot) const {
        if (root == NULL && otherRoot == NULL) {
            return true;
        } else if (root == NULL) {
            return false;
        } else if (otherRoot == NULL) {
            return false;
        } else {
        if (root->value == otherRoot->value && root->priority == otherRoot->priority && _recursiveEqual(root->left, otherRoot->left) && _recursiveEqual(root->link, otherRoot->link) && _recursiveEqual(root->right, otherRoot->right)) {
            return true;
        } else {
            return false;
        }
        }
    }

public:
    //
    // function that creates the default values for the private variables
    // O(1) complexity
    priorityqueue() {
        root = NULL;
        size = 0;
        curr = NULL;
    }

    //
    // operator=
    //
    // This function clears the current bst and makes a copy of the other
    // bst tree of the this one.
    // O(n) complexity

    priorityqueue& operator=(const priorityqueue& other) {
        NODE* temp = other.root;
        _recursiveClear(this->root);
        _recursiveCopy(temp);
        this->size = other.size;
        return *this;
    }

    //
    // clear:
    //
    // This function uses the recursive clear function clear the bst and
    // set the root and size to default values
    // O(n) complexity

    void clear() {
        _recursiveClear(root);
        root = NULL;
        size = 0;
    }

    //
    // destructor:
    //
    // This function is called at the end of the program to make ensure
    // all allocated memory is freed, and private variabls are reset
    // O(n) complexity
    ~priorityqueue() {
        if (root != NULL) {
          _recursiveClear(root);
        }
        root = NULL;
        size = 0;
        curr = NULL;
    }

    //
    // enqueue:
    //
    // This function builds the bst by taking a node with a given value
    // and priority and adds the node on the proper spot in the bst taking
    // account for duplicates as well.
    void enqueue(T value, int priority) {
        NODE* prev = NULL;
        NODE* trav = root;
        NODE* temp = new NODE();
        temp->value = value;
        temp->priority = priority;
        temp->left = NULL;
        temp->right = NULL;
        temp->dup = false;
        temp->parent = NULL;
        temp->link = NULL;
        while (trav != NULL) {
            prev = trav;
            if (priority < trav->priority) {
                trav = trav->left;
            } else if (priority > trav->priority) {
                trav = trav->right;
            } else if (priority == trav->priority) {
                temp->dup = true;
                trav = trav->link;
            }
        }
        if (prev == NULL) {
            root = temp;
        } else if (priority == prev->priority) {
            prev->link = temp;
            temp->parent = prev;
        } else if (priority > prev->priority) {
            prev->right = temp;
            temp->parent = prev;
        } else if (priority < prev->priority ) {
            prev->left = temp;
            temp->parent = prev;
        }
        size++;
    }
    //
    // dequeue:
    //
    // This function returns the value of the next node in the bst and
    // deletes that node from the tree.
    T dequeue() {
        T valueOut;
        NODE* trav = root;
        if (trav == NULL) {
            return T();
        }
        while (trav->left != NULL) {
            trav = trav->left;
        }
        valueOut = trav->value;
        if (trav->link != NULL) {
            if (trav == root) {
                root = trav->link;
                trav->link->parent = NULL;
            } else if (trav->parent != NULL) {
                trav->link->parent = trav->parent;
                trav->parent->left = trav->link;
            }
            if (trav->right != NULL) {
                trav->link->right = trav->right;
                trav->right->parent = trav->link;
            }
        } else {
            if (trav == root && trav->right != NULL) {
                root = trav->right;
                trav->right->parent = NULL;
            }
            else if (trav->right != NULL && trav->parent->left == trav) {
                trav->parent->left = trav->right;
                trav->right->parent = trav->parent;
            }
            else if (trav != root && trav->right == NULL) {
                trav->parent->left = NULL;
            }
        }
        delete trav;
        size--;
        if (size == 0) {
            root = NULL;
        }
        return valueOut; // TO DO: update this return
    }
    
    //
    // Size:
    //
    // This function returns the number of nodes in the bst.
    int Size() {
        if (root == NULL) {
            return 0;
        }
        return this->size;
    }

    //
    // begin
    //
    // This function iterates to the left most node in the bst and sets
    // the curr node to that node.
    void begin() {
        NODE* trav = root;
        if (trav == NULL) {
            return;
        }
        while (trav->left != NULL) {
            trav = trav->left;
        }
        curr = trav;
    }    
    //
    // next
    //
    // This function takes the next node in the priority queue at returns
    // the value at the node. You have to make sure you are traversing
    // with inorder here to get the correct priorites in order.
    bool next(T& value, int &priority) {
        if (curr == NULL) {
            return false;
        }
        value = curr->value;
        priority = curr->priority;
        if (curr->link != NULL) {
            curr = curr->link;
            return true;
        } else if (curr->link == NULL) {
            while (curr->parent != NULL && curr->parent->priority ==
            curr->priority) {
                curr = curr->parent;
                if (curr->parent == NULL) {
                   break;
                }
            }
        }
        if (curr->right != NULL) {
            curr = curr->right;
            while (curr->left != NULL) {
                curr = curr->left;
            }
            return true;
        } 
        else if (curr->right == NULL) {
            while (curr->parent != NULL && curr->parent->left != curr) {
                curr = curr->parent;
            }
            if (curr->parent != NULL) {
            curr = curr->parent;
            return true;
            }
        } 
        return false;
    }
    
    //
    // toString:
    //
    // Returns a single string with the priorities and values of the nodes
    // in the bst.
    string toString() {
        stringstream ss;
        if (root == NULL) {
            return "";
        } else {
            _recursiveFunciton(root, ss);
        }
        return ss.str();
    }
    
    //
    // peek:
    //
    // This function peeks the next node value in the bst and returns the
    // value but doesn't return delete the associated node.
    T peek() {
        T valueOut;
        NODE* trav = root;
        if (trav == NULL) {
            return T();
        }
        while (trav->left != NULL) {
            trav = trav->left;
        }
        valueOut = trav->value;
        return valueOut;
    }
    
    //
    // ==operator
    //
    // This function uses the helper equality function and returns true
    // or false depending on the equality of the two bsts.
    bool operator==(const priorityqueue& other) const {
        return _recursiveEqual(this->root, other.root);
    }
    
    //
    // getRoot - Do not edit/change!
    //
    // Used for testing the BST.
    // return the root node for testing.
    //
    void* getRoot() {
        return root;
    }
};
