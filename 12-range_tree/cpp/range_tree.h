#include <cstdint>
#include <limits>

// A node of the tree
class Node {
  public:
    int64_t key;
    int64_t value;
    Node* left;
    Node* right;
    Node* parent;

    // Constructor
    Node(int64_t key, int64_t value, Node* parent=nullptr, Node* left=nullptr, Node* right=nullptr) {
        this->key = key;
        this->value = value;
        this->parent = parent;
        this->left = left;
        this->right = right;
        if (left) left->parent = this;
        if (right) right->parent = this;
    }

};

// Splay tree
class Tree {
  public:
    // Pointer to root of the tree; nullptr if the tree is empty.
    Node* root;

    Tree(Node* root=nullptr) {
        this->root = root;
    }

    // Rotate the given `node` up. Perform a single rotation of the edge
    // between the node and its parent, choosing left or right rotation
    // appropriately.
    virtual void rotate(Node* node) {
        if (node->parent) {
            if (node->parent->left == node) {
                if (node->right) node->right->parent = node->parent;
                node->parent->left = node->right;
                node->right = node->parent;
            } else {
                if (node->left) node->left->parent = node->parent;
                node->parent->right = node->left;
                node->left = node->parent;
            }
            if (node->parent->parent) {
                if (node->parent->parent->left == node->parent)
                    node->parent->parent->left = node;
                else
                    node->parent->parent->right = node;
            } else {
                root = node;
            }

            Node* original_parent = node->parent;
            node->parent = node->parent->parent;
            original_parent->parent = node;
        }
    }

    // Splay the given node.
    virtual void splay(Node* node) {
        while (node->parent && node->parent->parent) {
            if ((node->parent->right == node && node->parent->parent->right == node->parent) ||
                (node->parent->left == node && node->parent->parent->left == node->parent)) {
                rotate(node->parent);
                rotate(node);
            } else {
                rotate(node);
                rotate(node);
            }
        }
        if (node->parent)
            rotate(node);
    }

    // Look up the given key in the tree, returning the
    // the node with the requested key or nullptr.
    Node* lookup(int64_t key) {
        Node* node = root;
        Node* node_last = nullptr;
        while (node) {
            node_last = node;
            if (node->key == key)
                break;
            if (key < node->key)
                node = node->left;
            else
                node = node->right;
        }
        if (node_last)
            splay(node_last);
        return node;
    }

    // Insert a (key, value) into the tree.
    // If the key is already present, nothing happens.
    void insert(int64_t key, int64_t value) {
        if (!root) {
            root = new Node(key, value);
            return;
        }

        Node* node = root;
        while (node->key != key) {
            if (key < node->key) {
                if (!node->left)
                    node->left = new Node(key, value, node);
                node = node->left;
            } else {
                if (!node->right)
                    node->right = new Node(key, value, node);
                node = node->right;
            }
        }
        splay(node);
    }

    // Delete given key from the tree.
    // It the key is not present, do nothing.
    //
    // The implementation first splays the element to be removed to
    // the root, and if it has both children, splays the largest element
    // in the left subtree and links it to the original right subtree.
    void remove(int64_t key) {
        if (lookup(key)) {
            Node* right = root->right;
            root = root->left;
            if (!root) {
                root = right;
                right = nullptr;
            }
            if (root)
                root->parent = nullptr;

            if (right) {
                Node* node = root;
                while (node->right)
                    node = node->right;
                splay(node);
                root->right = right;
                right->parent = root;
            }
        }
    }

    // Return the sum of elements with keys in range [left, right].
    //
    // Given a closed range [left, right], return the sum of values of elements
    // in the range, i.e., sum(value | (key, value) in tree, left <= key <= right).
    int64_t range_sum(int64_t left, int64_t right) {
        // TODO: Implement
    }

    // Destructor to free all allocated memory.
    ~Tree() {
        Node* node = root;
        while (node) {
            Node* next;
            if (node->left) {
                next = node->left;
                node->left = nullptr;
            } else if (node->right) {
                next = node->right;
                node->right = nullptr;
            } else {
                next = node->parent;
                delete node;
            }
            node = next;
        }
    }
};
