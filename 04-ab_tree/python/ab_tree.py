#!/usr/bin/env python3

class ABNode:
    """Single node in an ABTree.

    Each node contains keys and children
    (with one more children than there are keys).
    We also store a pointer to node's parent (None for root).
    """
    def __init__(self, keys = None, children = None, parent = None):
        self.keys = keys if keys is not None else []
        self.children = children if children is not None else []
        self.parent = parent

    def find_branch(self, key):
        """ Try finding given key in this node.

        If this node contains the given key, returns (True, key_position).
        If not, returns (False, first_position_with_key_greater_than_the_given).
        """
        i = 0
        while (i < len(self.keys) and self.keys[i] < key):
            i += 1

        return (i < len(self.keys) and self.keys[i] == key, i)

    def insert_branch(self, i, key, child):
        """ Insert a new key and a given child between keys i and i+1."""
        self.keys.insert(i, key)
        self.children.insert(i + 1, child)

class ABTree:
    """A class representing the whole ABTree."""
    def __init__(self, a, b):
        assert a >= 2 and b >= 2 * a - 1, "Invalid values of a, b: {}, {}".format(a, b)
        self.a = a
        self.b = b
        self.root = ABNode(children=[None])

    def find(self, key):
        """Find a key in the tree.

        Returns True if the key is present, False otherwise.
        """
        node = self.root
        while node:
            found, i = node.find_branch(key)
            if found: return True
            node = node.children[i]
        return False

    def delete_min(self):
        """ Delete the smallest element. """
        node = self.root
        while node.children[0]:
            node = node.children[0]

        node.children.pop(0)
        node.keys.pop(0)

        while len(node.children) < self.a and node.parent:
            node = node.parent
            first = node.children[0]
            second = node.children[1]

            # Merge the second to the first
            if len(second.children) == self.a:
                if second.children[0]:
                    for c in second.children:
                        c.parent = first
                first.children.extend(second.children)
                first.keys.append(node.keys.pop(0))
                first.keys.extend(second.keys)
                node.children.pop(1)

            # Move the leftest child of the second to the first
            else:
                second.children[0].parent = first
                first.children.append(second.children.pop(0))
                first.keys.append(node.keys[0])
                node.keys[0] = second.keys.pop(0)

        if len(node.children) == 1:
            assert node == self.root
            node.parent = None
            self.root = node.children[0]

    def split_node(self, node, size):
        """Helper function for insert

        Split node into two nodes such that original node contains first _size_ children.
        Return new node and the key separating nodes.
        """
        # TODO: Implement and use in insert method
        raise NotImplementedError

    def insert(self, key):
        """Add a given key to the tree, unless already present."""
        # TODO: Implement
        raise NotImplementedError
