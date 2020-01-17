//
//  Mohammed Mamun
//  CSCI 377
//  Project 2: Red Black Tree
//     and B Tree
//

#include <iostream>
using namespace std;

enum Color {RED, BLACK};

struct RBTNode {
    explicit RBTNode(int key);
    int key;
    int color;
    RBTNode *parent;
    RBTNode *leftChild;
    RBTNode *rightChild;
};

// Node constructor. Assign the key value,
// set the beginning color to red and
// initialize parent/children to null.
RBTNode::RBTNode(int key) {
    this->key = key;
    color = RED;
    leftChild = nullptr;
    rightChild = nullptr;
    parent = nullptr;
}

class RBTree {
public:
    RBTree();
    // The main functions
    RBTNode* RBTInsert(int key);
    RBTNode* RBTSearch(int key);
    RBTNode* RBTDelete(int key);
    int RBTDeleteAll(int key);
    void RBTInOrderPrintTree();
    bool verify();
private:
    RBTNode *root;
    // Helper functions
    void RBTInOrderPrintTree(RBTNode *&node);
    string getColorString(RBTNode *&node);
    int getColor(RBTNode *&node);
    void setColor(RBTNode *&node, int color);
    RBTNode* insertRBT(RBTNode *&root, RBTNode *&node);
    void fixColorsAfterDeletion(RBTNode *p);
    void rotateLeft(RBTNode *&node);
    void rotateRight(RBTNode *&node);
    bool verifyRoot();
    bool verifyChildren(RBTNode* node);
    int verifyBlackHeight(RBTNode *ptr);
};

// Constructor for a new tree
RBTree::RBTree() {
    root = nullptr;
}

// An extra method created to verify work
bool RBTree::verify() {
    bool verified = true;
    // 1. The root of the red-black tree must be black
    if (!verifyRoot()) {
        verified = false;
    }
    // 2. The children of a red node must be black
    if (!verifyChildren(root)) {
        verified = false;
    }
    // 3. For each node with at least one null child, the number of black
    // nodes on the path from the root to the null child is the same
    if (verifyBlackHeight(root) >= 0) {
        verified = false;
    }
    return verified;
}
// Extension of the verify function. Verify the root
// is black
bool RBTree::verifyRoot() {
    return root == NULL || root->color == BLACK;
}
// Extension of the verify function. Verify all red nodes
// have black children.
bool RBTree::verifyChildren(RBTNode *ptr) {
    bool verified = true;
    if (ptr != nullptr) {
        // Find the red node
        if (ptr->color == RED) {
            // If one child isn't black, return false
            if ((ptr->leftChild != nullptr && ptr->leftChild->color != BLACK) ||
                (ptr->rightChild != nullptr && ptr->rightChild->color != BLACK)) {
                verified = false;
            }
        }
        // Recursively check the others. AND with already found value so
        // that a false isn't overwritten.
        verified = verifyChildren(ptr->leftChild) && verified;
        verified = verifyChildren(ptr->rightChild) && verified;
    }
    
    return verified;
}
// Extension of the verify function. Verify that the height of all
// simple paths is the same.
int RBTree::verifyBlackHeight(RBTNode *ptr) {
    if (ptr == NULL)
        return 0;
    // REcursively compute the height for the children
    int leftHeight = verifyBlackHeight(ptr->leftChild);
    int rightHeight = verifyBlackHeight(ptr->rightChild);
    int add = ptr->color == BLACK ? 1 : 0;
    // One or more of current node's children is a root of an invalid tree
    // or they contain different number of black nodes on a path to a null node.
    if (leftHeight == -1 || rightHeight == -1 || leftHeight != rightHeight)
        return -1;
    else
        return leftHeight + add;
}
// Public Red Black Tree function inserts a new node with key into the tree
RBTNode* RBTree::RBTInsert(int key) {
    // Create a new node to be inserted
    RBTNode *node = new RBTNode(key);
    
    // Call the private helper insert function to insert the node.
    // New function because will call itself recursively
    root = insertRBT(root, node);
    
    // Fix the tree coloring after the insertion
    RBTNode *parent = nullptr;
    RBTNode *grandparent = nullptr;
    // We're starting at the bottom with the node that we inserted and working up
    // to the top while the color of the node is red and the color of the parent is red.
    while (node != root && getColor(node) == RED && getColor(node->parent) == RED) {
        parent = node->parent;
        grandparent = parent->parent;
        if (parent == grandparent->leftChild) {
            RBTNode *uncle = grandparent->rightChild;
            // If the parent was a left child and our
            // node's right uncle was RED, set the uncle and the
            // parent to black, set the grandparent to red
            // and go with the grandparent on the next iteration
            if (getColor(uncle) == RED) {
                setColor(uncle, BLACK);
                setColor(parent, BLACK);
                setColor(grandparent, RED);
                node = grandparent;
            } else {
                // If the parent was a left child and our node's
                // uncle was BLACK:
                // If our node is a right child,
                // call the rotate function to rotate left.
                // Then always set the node to the parent to iterate
                // on the parent next, Rotate the grandparent
                // to the right by calling the rotate function,
                // and swap the parent and the grandparent colors.
                if (node == parent->rightChild) {
                    rotateLeft(parent);
                    node = parent;
                    parent = node->parent;
                }
                rotateRight(grandparent);
                swap(parent->color, grandparent->color);
                node = parent;
            }
        } else {
            RBTNode *uncle = grandparent->leftChild;
            // If the parent was a right child and our node's
            // left uncle is red, set the uncle to black,
            // set the parent to black, set the grandparent to red,
            // and make the node the grandparent to iterate through
            // him next.
            if (getColor(uncle) == RED) {
                setColor(uncle, BLACK);
                setColor(parent, BLACK);
                setColor(grandparent, RED);
                node = grandparent;
            } else {
                // If the parent was a right child and our node's left
                // uncle is black, then rotate the parent to the right.
                // Then always rotate the grandparent to the left by calling
                // the rotate function and swap the parent and the grandparent
                // colors. Set the node to the parent to iterate through the
                // parents next.
                if (node == parent->leftChild) {
                    rotateRight(parent);
                    node = parent;
                    parent = node->parent;
                }
                rotateLeft(grandparent);
                swap(parent->color, grandparent->color);
                node = parent;
            }
        }
    }
    // The root should always be black
    setColor(root, BLACK);
    return node;
}
// The insert helper function that will insert the node
// in the correct location. No corrections are done here, but
// are rather done after this function is called.
// This function finds the pointer to the spot where we insert
// the new node. We iterate from the beginning of the tree
// with the initial root and will go down the tree depending on the
// new node's key.
RBTNode* RBTree::insertRBT(RBTNode *&root, RBTNode *&newNode) {
    // If we reached the root, return the ptr, representing
    // the spot where we inserted the new node
    if (root == nullptr)
        return newNode;
    
    // If the new node's key is less than the root's key,
    // call this function again with a new root of the root's
    // left child
    if (newNode->key < root->key) {
        root->leftChild = insertRBT(root->leftChild, newNode);
        root->leftChild->parent = root;
        // If the new node's key is greater than the root's key,
        // call this function again with a new root of the root's
        // right child.
    } else if (newNode->key >= root->key) { // store duplicates too
        root->rightChild = insertRBT(root->rightChild, newNode);
        root->rightChild->parent = root;
    }
    
    return root;
}

// This function will search for the node with the value key
// It returns the pointer to that node.
RBTNode* RBTree::RBTSearch(int key) {
    // Start with a null pointer. If the root is null,
    // we will return a null, meaning no node with given key
    // was found
    RBTNode *foundNode = nullptr;
    if(root != nullptr) {
        RBTNode *currentNode = root;
        // If the root is not null, iterate through the tree, until you find
        // the key, or until you reach the end of the tree.
        while(currentNode != nullptr && foundNode == nullptr) {
            // if the current node we're iterating through has the key
            // that we're looking for, set that pointer
            if(currentNode->key == key) {
                foundNode = currentNode;
            }
            // if we didn't find it yet, then move either to the left or the
            // right child of the current node. Move right if the key is greater
            // than the current node's key, move left otherwise
            if(foundNode == nullptr) {
                if(currentNode->key<key) {
                    currentNode=currentNode->rightChild;
                } else {
                    currentNode=currentNode->leftChild;
                }
            }
        }
    }
    return foundNode;
}

// This function will delete all nodes that have with key value
// and returns a count of how many nodes were deleted
int RBTree::RBTDeleteAll(int key) {
    int count = 0;
    // Call the delete method which removes the first occurance of
    // the key found. If it finds a key, increment the counter.
    while (RBTDelete(key) != nullptr) {
        count++;
    }
    // return the counter
    return count;
}
// The delete function is the most complex.
// This function returns a pointer to the node that was
// deleted. It iterates through the tree to find the node,
// deletes it, and fixes the tree so it connects
// to the deleted nodes children correctly.
// then it will call a function that will clean up the colors
// after the deletion.
RBTNode* RBTree::RBTDelete(int key) {
    RBTNode* deletedNode = nullptr;
    // if this is an empty tree, there's nothing to delete
    if(root != nullptr) {
        RBTNode *currentNode = root;
        // Iterate through the tree to find the node to delete
        while(currentNode != nullptr && deletedNode == nullptr) {
            // the current node has key that we're looking to delte
            // so mark this node as the one to delete
            if(currentNode->key == key) {
                deletedNode = currentNode;
            }
            // if we didn't find a node to delete, go to the next node
            // to check
            if(deletedNode == nullptr) {
                // if the key we're looking for is greater than the
                // current node's key, then go to the right child.
                // If it's less than (equals was already checked above),
                // then go to the left child.
                if(currentNode->key < key) {
                    currentNode=currentNode->rightChild;
                } else {
                    currentNode=currentNode->leftChild;
                }
            }
        }
        // If we deleted successfully, we need to balance out the tree
        if(deletedNode != nullptr) {
            RBTNode *childNode = nullptr;
            
            // current node is also the node that we're deleting
            // if the node has no children, set the child node to
            // the current node
            if(currentNode->leftChild == nullptr || currentNode->rightChild == nullptr) {
                childNode=currentNode;
            } else {
                // if the deleted node has children, change the successor
                if(currentNode->leftChild != nullptr) {
                    childNode = currentNode->leftChild;
                    // when the deleted node has left children,
                    // assign the child as the last right node
                    while(childNode->rightChild != nullptr) {
                        childNode = childNode->rightChild;
                    }
                } else {
                    childNode = currentNode->rightChild;
                    // when the deleted node has right children,
                    // assign the child as the last left node
                    while(childNode->leftChild!=nullptr) {
                        childNode=childNode->leftChild;
                    }
                }
            }
            
            RBTNode *grandchildNode = nullptr;
            if(childNode->leftChild != nullptr) {
                // Keep track of the left child
                grandchildNode = childNode->leftChild;
            } else if(childNode->rightChild != nullptr) {
                // Keep track of the right child
                grandchildNode=childNode->rightChild;
            } else {
                // there are no children
                grandchildNode=nullptr;
            }
            
            if(grandchildNode != nullptr) {
                // Assign the parent to make sure the nodes are connected
                grandchildNode->parent = childNode->parent;
            }
            
            if(childNode->parent == nullptr) {
                // If there is no parent, make the grandchild be the
                // new root
                root = grandchildNode;
            } else {
                if(childNode == childNode->parent->leftChild) {
                    // If this is the left child, assign the granchild
                    // to be the new left child
                    childNode->parent->leftChild = grandchildNode;
                } else {
                    // If this is the right child, assign the granchild
                    // to be the new right child
                    childNode->parent->rightChild = grandchildNode;
                }
            }
            if(childNode != currentNode) {
                // we figured out the new child, so we sould change the color
                // and key to the child's color and key
                currentNode->color = childNode->color;
                currentNode->key = childNode->key;
            }
            if(childNode->color == BLACK && grandchildNode != nullptr) {
                // need to fix the colors after deletion since the child
                // color is black, to make sure all black nodes in a path match
                fixColorsAfterDeletion(grandchildNode);
            }
        }
    }
    return deletedNode;
}
// This is a private helper method to fix the colors of a tree
// after a node has been deleted
void RBTree::fixColorsAfterDeletion(RBTNode *currentNode) {
    RBTNode *siblingNode;
    // Iterate up from the given node down until the root
    while(currentNode != root && currentNode->color == BLACK) {
        if(currentNode->parent->leftChild == currentNode) {
            siblingNode = currentNode->parent->rightChild;
            // If this current node is a left child with black color,
            // and its sibling is red, then change the sibling to black
            // and change the parent to red. Then rotate the parent
            // to the left
            if(siblingNode->color==RED) {
                siblingNode->color = BLACK;
                currentNode->parent->color = RED;
                rotateLeft(currentNode->parent);
                siblingNode = currentNode->parent->rightChild;
            }
            // If this current node is a left child with black color,
            // and its sibling has two black children, make the sibling red
            // and go up a node with to make a current node the parent
            if(siblingNode->rightChild->color == BLACK && siblingNode->leftChild->color == BLACK) {
                siblingNode->color = RED;
                currentNode = currentNode->parent;
            } else {
                // If this current node is a left child with a black color,
                // and its sibling's right child is black, then make the
                // sibling's left child black as well and the sibling itself red.
                // Rotate the sibling to the right.
                if(siblingNode->rightChild->color == BLACK) {
                    siblingNode->leftChild->color = BLACK;
                    siblingNode->color = RED;
                    rotateRight(siblingNode);
                    siblingNode = currentNode->parent->rightChild;
                }
                // Then assign the sibling color to the current parent's color
                // and make that parent black, the siblings right child black,
                // and rotate the parent to the left
                siblingNode->color = currentNode->parent->color;
                currentNode->parent->color = BLACK;
                siblingNode->rightChild->color = BLACK;
                rotateLeft(currentNode->parent);
                currentNode = root;
            }
        } else {
            siblingNode = currentNode->parent->leftChild;
            // If this node is a right child and the sibling is red, then
            // make the sibling black and their parent red.
            // Rotate the parent to the right
            if(siblingNode->color == RED) {
                siblingNode->color = BLACK;
                currentNode->parent->color = RED;
                rotateRight(currentNode->parent);
                siblingNode = currentNode->parent->leftChild;
            }
            // If the sibling has two black children, make the sibling color red and change
            // the current node to the parent.
            if(siblingNode->leftChild->color == BLACK && siblingNode->rightChild->color == BLACK) {
                siblingNode->color = RED;
                currentNode=currentNode->parent;
            } else {
                // If the sibling doesn't have two black children, but the left child is black
                // then make the right child black and the sibling red.
                // Then rotate the sibling to the left
                if(siblingNode->leftChild->color == BLACK) {
                    siblingNode->rightChild->color = BLACK;
                    siblingNode->color = RED;
                    rotateLeft(siblingNode);
                    siblingNode = currentNode->parent->leftChild;
                }
                // make the sibling color match the parent color and set the parent to black.
                // set the sibling's left to black (if it wasn't above)
                // and rotate the parent to the right
                siblingNode->color = currentNode->parent->color;
                currentNode->parent->color = BLACK;
                siblingNode->leftChild->color = BLACK;
                rotateRight(currentNode->parent);
                currentNode = root;
            }
        }
        currentNode->color=BLACK;
        root->color=BLACK;
    }
}
// This is a helper function used by insert and delete functions
// to rotate the given node to the left
void RBTree::rotateLeft(RBTNode *&currentNode) {
    // First, grab the right child and assign him to
    // his left child (current nodes grandchild)
    RBTNode *right_child = currentNode->rightChild;
    currentNode->rightChild = right_child->leftChild;
    
    // assign the parent value of the right child to the
    // current node to make sure it links up nicely
    if (currentNode->rightChild != nullptr) {
        currentNode->rightChild->parent = currentNode;
    }
    
    // the right child's parent should also be assigned to
    // the current node's parent
    right_child->parent = currentNode->parent;
    
    // if the current node's parent is null, then this is a root
    // assign the tree's root variable
    if (currentNode->parent == nullptr) {
        root = right_child;
        // If the current node is a left child, assign right child to
        // the left
    } else if (currentNode == currentNode->parent->leftChild) {
        currentNode->parent->leftChild = right_child;
        // If the current node is a right child, assign right child to
        // the right
    } else {
        currentNode->parent->rightChild = right_child;
    }
    
    right_child->leftChild = currentNode;
    currentNode->parent = right_child;
}
// This is a helper function used by insert and delete functions
// to rotate the given node to the right
void RBTree::rotateRight(RBTNode *&currentNode) {
    // First, grab the left child of the current node and
    // assign him to his right child (current's grandchild)
    RBTNode *left_child = currentNode->leftChild;
    currentNode->leftChild = left_child->rightChild;
    
    // set the current node's left child's parent to itself
    // to make sure the linking is there
    if (currentNode->leftChild != nullptr) {
        currentNode->leftChild->parent = currentNode;
    }
    
    // Assign the left child's parent to the current node's parent
    // to make sure the linking is there
    left_child->parent = currentNode->parent;
    
    // if the current node is a root (has no parent), then
    // mark the tree's root variable as the left child
    if (currentNode->parent == nullptr) {
        root = left_child;
        // If the current node is a left child, then assign his left child
        // as the left child node
    } else if (currentNode == currentNode->parent->leftChild) {
        currentNode->parent->leftChild = left_child;
        // If the current node is a right child, then assign his right child
        // as the left child node
    } else {
        currentNode->parent->rightChild = left_child;
    }
    
    left_child->rightChild = currentNode;
    currentNode->parent = left_child;
}

// This is the main public printing function that will call the helper
// printing function passing the root, the beginning of the tree.
void RBTree::RBTInOrderPrintTree() {
    RBTInOrderPrintTree(root);
    cout << "--------------------" << endl;
}

// This is the helper private function that will print the tree.
// It will print it in order from smallest to largest and includes the
// key and color of the node. It will list the parent and two children
// with their key and color.
void RBTree::RBTInOrderPrintTree(RBTNode *&ptr) {
    if (ptr == nullptr)
        return;
    
    // keep calling the left children so the smallest is printed first
    RBTInOrderPrintTree(ptr->leftChild);
    if (ptr->parent == nullptr) {
        // identify the root of the tree
        cout << "Node (Root): " << endl;
    } else {
        cout << "Node: " << endl;
    }
    cout << "  key = " << ptr->key << endl;
    cout << "  color = " << getColorString(ptr) << endl;
    cout << "  parent: " << endl;
    if (ptr->parent == nullptr) {
        cout << "    None" << endl;
    } else {
        cout << "    key = " << ptr->parent->key << endl;
        cout << "    color = " << getColorString(ptr->parent) << endl;
    }
    cout << "  left child: " << endl;
    if (ptr->leftChild == nullptr) {
        cout << "    NIL" << endl;
    } else {
        cout << "    key = " << ptr->leftChild->key << endl;
        cout << "    color = " << getColorString(ptr->leftChild) << endl;
    }
    cout << "  right child: " << endl;
    if (ptr->rightChild == nullptr) {
        cout << "    NIL" << endl;
    } else {
        cout << "    key = " << ptr->rightChild->key << endl;
        cout << "    color = " << getColorString(ptr->rightChild) << endl << endl;
    }
    // then print the right children to finish this side of the tree
    RBTInOrderPrintTree(ptr->rightChild);
}
// This function returns the string literal of the color of the node.
// the deafult is black, but if the node is not null, it should not
// hit a default case.
string RBTree::getColorString(RBTNode *&node) {
    string color;
    switch(node->color) {
        case 0:
            color = "Red";
            break;
        case 1:
            color = "Black";
            break;
        case 2:
            color = "Double Black";
            break;
        default:
            color = "Black";
            break;
    }
    return color;
}
// This is a helper function that returns the color. To avoid a null pointer, it will
// check the node for null and return black if it is null
int RBTree::getColor(RBTNode *&node) {
    if (node == nullptr)
        return BLACK;
    
    return node->color;
}
// This is a helper function that sets the value of the node that is passed to it.
void RBTree::setColor(RBTNode *&node, int color) {
    if (node == nullptr)
        return;
    
    node->color = color;
}

// A BTree node
struct BTNode {
    explicit BTNode(bool leaf);
    int *keys;
    BTNode **children;
    int numKeys;
    bool isLeaf;
};

class BTree {
    BTNode *root;
public:
    BTree() {
        root = NULL;
    }
    void BTInOrderPrintTree() {
        BTInOrderPrintTree(root);
    }
    BTNode* BTSearch(int key) {
        if (root == NULL) {
            return NULL;
        } else {
            return BTSearch(root, key);
        }
    }
    void BTInsert(int);
    int BTDeleteAll(int);
    BTNode* BTDelete(int);
private:
    // Helper functions that help the main functions
    // complete their job
    BTNode* BTSearch(BTNode*, int);
    void BTInOrderPrintTree(BTNode*);
    void BTInsertNonFull(BTNode*, int);
    void BTSplitChild(BTNode*, int, BTNode*);
    void BTMergeChild(BTNode*, int);
    BTNode* BTDelete(BTNode*, int);
    void BTDeleteFromLeaf(BTNode*, int);
    void BTDeleteFromNonLeaf(BTNode*, int);
};

BTNode::BTNode(bool isLeaf) {
    // Copy the given leaf property
    this->isLeaf = isLeaf;
    
    // Assign the maximum number of possible keys
    // and child pointers
    // and number of keys starts at zero
    keys = new int[2];
    children = new BTNode *[3];
    numKeys = 0;
}

// The main function that inserts the new key into this B-Tree
void BTree::BTInsert(int key) {
    if (root == NULL) {
        // Allocate memory for the root and insert the first key
        root = new BTNode(true);
        root->keys[0] = key;
        root->numKeys = 1;
    } else {
        // If root is full, then tree grows in height
        if (root->numKeys == 3) {
            // Allocate memory for the new root and make the old
            // root as a child of the new root
            BTNode *newRoot = new BTNode(false);
            newRoot->children[0] = root;
            
            // Split the old root and move 1 key to the new root
            BTSplitChild(newRoot, 0, root);
            
            // New root has two children now.  Decide which of the
            // two children is going to have a new key
            int i = 0;
            if (newRoot->keys[0] < key) {
                i++;
            }
            BTInsertNonFull(newRoot->children[i], key);
            
            // Root points to the new root
            root = newRoot;
        } else {
            BTInsertNonFull(root, key);
        }
    }
}

// A helper function to insert a new key in this node that is not full
void BTree::BTInsertNonFull(BTNode* insertedNode, int key) {
    // index for the right most element
    int i = insertedNode->numKeys-1;
    
    if (insertedNode->isLeaf == true) {
        // For leaf nodes, find the location of the new key to be
        // inserted and move the greater than keys one place ahead
        while (i >= 0 && insertedNode->keys[i] > key) {
            insertedNode->keys[i+1] = insertedNode->keys[i];
            i--;
        }
        
        // Insert the new key at found location
        insertedNode->keys[i+1] = key;
        insertedNode->numKeys = insertedNode->numKeys + 1;
    } else {
        // For non-leaf nodes, find the child which is going to
        // have the new key
        while (i >= 0 && insertedNode->keys[i] > key) {
            i--;
        }
        
        if (insertedNode->children[i + 1]->numKeys == 3) {
            // If the child is full, then split it
            BTSplitChild(insertedNode, i + 1, insertedNode->children[i+1]);
            
            // Find the location of the new key to be inserted.
            if (insertedNode->keys[i + 1] < key) {
                i++;
            }
        }
        BTInsertNonFull(insertedNode->children[i + 1], key);
    }
}

// This function will delete all occurences of the passed key
// It will perform in a loop by calling the delete function
// and will keep count of how many nodes were deleted.
int BTree::BTDeleteAll(int key) {
    int count = 0;
    while (BTDelete(key) != NULL) {
        count++;
    }
    return count;
}

// This delete fuction will call a helper function to delete a node
// with given key from the tree and will correct the root when
// needed
BTNode* BTree::BTDelete(int key) {
    BTNode* deletedNode = NULL;
    if (root != NULL) {
        deletedNode = BTDelete(root, key);
        
        // If the root doesn't have any keys after the delete,
        // make its first child the new root (if it has a child)
        if (root->numKeys == 0) {
            BTNode *oldRoot = root;
            if (root->isLeaf) {
                root = NULL;
            } else {
                root = root->children[0];
            }
            delete oldRoot;
        }
    }
    return deletedNode;
}

// A helper function that will remove the key from a sub-tree
// starting with the current node as the root
BTNode* BTree::BTDelete(BTNode* currentNode, int key) {
    BTNode* deletedNode = NULL;
    int index = 0;
    // First, find the index of the key that is greater than or equal to the key given
    while (index < currentNode->numKeys && currentNode->keys[index] < key) {
        ++index;
    }
    
    
    if (index < currentNode->numKeys && currentNode->keys[index] == key) {
        // The key to be removed is present in this node
        // If the node is a leaf node - removeFromLeaf is called
        // Otherwise, removeFromNonLeaf function is called
        if (currentNode->isLeaf) {
            BTDeleteFromLeaf(currentNode, index);
        } else {
            BTDeleteFromNonLeaf(currentNode, index);
        }
        // Assign the deleted node to be returned
        deletedNode = currentNode;
    } else {
        // If this node is a leaf node, then the key is not present in tree
        if (!currentNode->isLeaf) {
            // If this is not a leaf node, then the key to be removed is in the
            // subtree that's rooted with this node. The flag indicates if
            // the key is present with the last child of this node
            bool flag = (index == currentNode->numKeys) ? true : false;
            
            // If the child where the key will go has less than 2 keys, find the child
            if (currentNode->children[index]->numKeys < 2) {
                // If the previous child has more than 1 key, borrow a key from that child
                if (index!=0 && currentNode->children[index-1]->numKeys>=2) {
                    // Borrow from previous
                    BTNode *child=currentNode->children[index];
                    BTNode *sibling=currentNode->children[index-1];
                    
                    // Moving all key in childone step ahead
                    for (int i=child->numKeys-1; i>=0; --i) {
                        child->keys[i+1] = child->keys[i];
                    }
                    
                    // If child is not a leaf, move all its child pointers one step ahead
                    if (!child->isLeaf) {
                        for(int i=child->numKeys; i>=0; --i) {
                            child->children[i+1] = child->children[i];
                        }
                    }
                    child->keys[0] = currentNode->keys[index-1];
                    
                    // Moving sibling's last child as this child's first child
                    if (!currentNode->isLeaf)
                        child->children[0] = sibling->children[sibling->numKeys];
                    
                    // Moving the key from the sibling to the parent
                    // This reduces the number of keys in the sibling
                    currentNode->keys[index-1] = sibling->keys[sibling->numKeys-1];
                    
                    child->numKeys += 1;
                    sibling->numKeys -= 1;
                } else if (index!=currentNode->numKeys && currentNode->children[index+1]->numKeys>=2) {
                    // If the next child has more than 1 key, borrow a key from that child
                    BTNode *child=currentNode->children[index];
                    BTNode *sibling=currentNode->children[index+1];
                    
                    // change the number of keys in child
                    child->keys[(child->numKeys)] = currentNode->keys[index];
                    
                    // Sibling's first child is inserted as the last child
                    if (!(child->isLeaf)) {
                        child->children[(child->numKeys)+1] = sibling->children[0];
                    }
                    
                    //The first key from sibling is inserted into keys
                    currentNode->keys[index] = sibling->keys[0];
                    
                    // Moving all keys in sibling one step behind
                    for (int i=1; i<sibling->numKeys; ++i) {
                        sibling->keys[i-1] = sibling->keys[i];
                    }
                    
                    // Moving the child pointers one step behind
                    if (!sibling->isLeaf)  {
                        for(int i=1; i<=sibling->numKeys; ++i) {
                            sibling->children[i-1] = sibling->children[i];
                        }
                    }
                    
                    // Increasing and decreasing the key count
                    child->numKeys += 1;
                    sibling->numKeys -= 1;
                } else {
                    // Merge child with its sibling
                    // If child is the last child, merge it with with its previous sibling
                    // Otherwise merge it with its next sibling
                    if (index != currentNode->numKeys) {
                        BTMergeChild(currentNode, index);
                    } else {
                        BTMergeChild(currentNode, index-1);
                    }
                }
            }
            // If the last child has been merged, it must have merged with the previous
            // child and so we recurse on that child. Else, we recurse on the
            // last child which now has atleast 2 keys
            if (flag && index > currentNode->numKeys) {
                deletedNode = BTDelete(currentNode->children[index-1], key);
            } else {
                deletedNode = BTDelete(currentNode->children[index], key);
            }
        }
    }
    return deletedNode;
}

// A helper function to remove the key with given index from this node
// which is a leaf node
void BTree::BTDeleteFromLeaf (BTNode* currentNode, int index) {
    // Move all the keys after the indexed position one place backward
    for (int i=index+1; i < currentNode->numKeys; ++i) {
        currentNode->keys[i-1] = currentNode->keys[i];
    }
    
    // Reduce the count of keys
    currentNode->numKeys--;
    
    return;
}

// A helper function to remove the key with given index from this node
// which is a non-leaf node
void BTree::BTDeleteFromNonLeaf(BTNode* currentNode, int index) {
    int k = currentNode->keys[index];
    
    // If the child that precedes the indexed child has atleast 2 keys,
    // find the predecessor node in the subtree and recursively delete predecessor
    if (currentNode->children[index]->numKeys >= 2) {
        // get predecessor of keys[idx]
        BTNode *currentChildren = currentNode->children[index];
        while (!currentChildren->isLeaf) {
            currentChildren = currentChildren->children[currentChildren->numKeys];
        }
        // the last key of the leaf as the predecessor
        int pred = currentChildren->keys[currentChildren->numKeys - 1];
        
        currentNode->keys[index] = pred;
        BTDelete(currentNode->children[index], pred);
    } else if  (currentNode->children[index+1]->numKeys >= 2) {
        // If the child has less than 2 keys, examine the next one.
        // If the next one has at least 2 keys, find the successor of the node
        // in the subtree. Recursively delete the successor.
        // get the successor key;
        // move to the left most node starting from C[idx+1] until we reach a leaf
        BTNode *cur = currentNode->children[index+1];
        while (!cur->isLeaf) {
            cur = cur->children[0];
        }
        
        // the first key of the leaf is the successor
        int succ = cur->keys[0];
        
        currentNode->keys[index] = succ;
        BTDelete(currentNode->children[index+1], succ);
    } else {
        // If both nodes have less than 2 keys, merge the node and all the children
        // so will now have 3 keys. Free the child and recursively delete the indexed child
        BTMergeChild(currentNode, index);
        BTDelete(currentNode->children[index], k);
    }
    return;
}

// A function to merge the child with given index and the
// next child with index + 1. The index + 1 child is then freed
// afte merging
void BTree::BTMergeChild(BTNode* currentNode, int index) {
    BTNode *child = currentNode->children[index];
    BTNode *sibling = currentNode->children[index+1];
    
    // Pulling a key from the current node and inserting it into first
    child->keys[1] = currentNode->keys[index];
    
    // Copying the keys from next child to indexed child at the end
    for (int i=0; i<sibling->numKeys; ++i) {
        child->keys[i+2] = sibling->keys[i];
    }
    
    // Copying the child pointers from the next child to indexed child
    if (!child->isLeaf) {
        for(int i=0; i<=sibling->numKeys; ++i) {
            child->children[i+2] = sibling->children[i];
        }
    }
    
    // Moving all keys after index in the current node one step before -
    // to fill the gap created by moving keys to child
    for (int i=index+1; i<currentNode->numKeys; ++i) {
        currentNode->keys[i-1] = currentNode->keys[i];
    }
    
    // Moving the child pointers after next in the current node one
    // step before
    for (int i=index+2; i<=currentNode->numKeys; ++i) {
        currentNode->children[i-1] = currentNode->children[i];
    }
    
    // Updating the key count of child and the current node
    child->numKeys += sibling->numKeys+1;
    currentNode->numKeys--;
    
    // Freeing the memory occupied by sibling
    delete(sibling);
    return;
}

// A utility function to split the child y of this node
// Note that y must be full when this function is called
void BTree::BTSplitChild(BTNode* insertedNode, int index, BTNode *childNode) {
    // New node with the child
    BTNode *newChildNode = new BTNode(childNode->isLeaf);
    newChildNode->numKeys = 1;
    
    // Copy the last keys of child node to new child node
    for (int i = 0; i < 1; i++) {
        newChildNode->keys[i] = childNode->keys[i+2];
    }
    
    // Copy the last children of child node to new child node
    if (childNode->isLeaf == false) {
        for (int j = 0; j < 2; j++) {
            newChildNode->children[j] = childNode->children[j+2];
        }
    }
    
    // Reduce the number of keys in child node
    childNode->numKeys = 1;
    
    // Since this node is going to have a new child,
    // create space of new child
    for (int i = insertedNode->numKeys; i >= index+1; i--) {
        insertedNode->children[i+1] = insertedNode->children[i];
    }
    
    // Link the new child to this node
    insertedNode->children[index+1] = newChildNode;
    
    // A key of y will move to this node. Find location of
    // new key and move all greater keys one space ahead
    for (int i = insertedNode->numKeys-1; i >= index; i--) {
        insertedNode->keys[i+1] = insertedNode->keys[i];
    }
    
    // Copy the middle key of y to this node
    insertedNode->keys[index] = childNode->keys[1];
    
    // Increment count of keys in this node
    insertedNode->numKeys = insertedNode->numKeys + 1;
}
// Function to print out all of the keys of the tree in order
void BTree::BTInOrderPrintTree(BTNode* currentNode) {
    int i;
    // Print the keys first and children
    for (i = 0; i < currentNode->numKeys; i++) {
        // If this is not leaf, then print the subtree
        // rooted with child first before printing the key
        if (currentNode->isLeaf == false) {
            BTInOrderPrintTree(currentNode->children[i]);
        }
        cout << "Node: " << endl;
        cout << "  this key = " << currentNode->keys[i] << endl;
        if (currentNode->isLeaf) {
            cout << "  leaf node" << endl;
        } else {
            cout << "  non-leaf node" << endl;
        }
        cout << "  number of keys = " << currentNode->numKeys << endl;
        cout << "  keys = ";
        for (int j = 0; j < currentNode->numKeys; j++) {
            cout << currentNode->keys[j];
            if (j < currentNode->numKeys - 1) {
                cout << " , ";
            }
        }
        cout << endl;
    }
    // Print the subtree rooted with last child
    if (currentNode->isLeaf == false) {
        BTInOrderPrintTree(currentNode->children[i]);
    }
}

// Helper function to search for key in the sub-tree that begins
// with the given node
BTNode* BTree::BTSearch(BTNode* searchNode, int key) {
    // Find the first key greater than or equal to k
    int i = 0;
    while (i < searchNode->numKeys && key > searchNode->keys[i]) {
        i++;
    }
    
    // If the found key is equal to k, return this node
    if (searchNode->keys[i] == key) {
        return searchNode;
    }
    
    // If key is not found here and this is a leaf node, return a null
    if (searchNode->isLeaf == true) {
        return NULL;
    }
    
    // Go to the appropriate child
    return BTSearch(searchNode->children[i], key);
}

int main() {
    //********* RED BLACK TREE *************
    RBTree rbTree;
    int keys[] = {4, 2, 3, 3, 3, 22, 9, 21, 11, 45, 15, 21, 15, 18, 1, 0, 15, 12, 15};
    
    // Insert the array of keys into tree
    for (int i = 0; i < 19; i++) {
        rbTree.RBTInsert(keys[i]);
    }
    // Print the current tree
    rbTree.RBTInOrderPrintTree();
    
    // Insert values 0 through 119 into the tree
    // and print it when i is fully divisible by 8
    const int period = 8;
    for (int i = 0; i < 120;  i++) {
        rbTree.RBTInsert(i);
        if (i && i % period == 0) {
            rbTree.RBTInOrderPrintTree();
        }
    }
    
    // go through the tree again and delete every key that is
    // in the tree that is fully divisible by 3.
    // keep adding the count up of how many were deleted,
    // and if the count is fully divisible by 8, print out the whole
    // tree and reset the count to zero.
    for (int i = 0, cnt = 0; i < 120;  i++) {
        if (i % 3 == 0 && rbTree.RBTSearch(i) != nullptr) {
            cnt += rbTree.RBTDeleteAll(i);
        }
        if (cnt && cnt % period == 0) {
            rbTree.RBTInOrderPrintTree();
            cnt = 0;
        }
    }
    
    // print the final tree
    rbTree.RBTInOrderPrintTree();
    
    // a little extra verification that everything worked correctly
    if (rbTree.verify()) {
        cout << "RBT Done" << endl;
    }
    
    
    //********* B TREE *************
    
    BTree bTree;
    // Insert each key from the array into the b-tree
    for (int i = 0; i < 19; i++) {
        bTree.BTInsert(keys[i]);
    }
    // Print the current tree
    bTree.BTInOrderPrintTree();
    
    // Insert more keys with values 0 - 119
    // and print the tree when the key can be fully
    // divided by 8
    for (int i = 0; i < 120;  i++) {
        bTree.BTInsert(i);
        if (i && i % period == 0) {
            bTree.BTInOrderPrintTree();
        }
    }
    
    // Go through each key with value 0 - 119 and
    // delete any key that is fully divisible by 3.
    // Add up the number of keys deleted and when that
    // number if fully divisible by 8, print out the tree
    // and reset the counter
    for (int i = 0, cnt = 0; i<120;  i++) {
        if (i % 3 == 0 && bTree.BTSearch(i) != nullptr) {
            cnt += bTree.BTDeleteAll(i);
        }
        if (cnt && cnt % period == 0) {
            bTree.BTInOrderPrintTree();
            cnt = 0;
        }
    }
    
    // Print the final tree
    bTree.BTInOrderPrintTree();
    return 0;
}
