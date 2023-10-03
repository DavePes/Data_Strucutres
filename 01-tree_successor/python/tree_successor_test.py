#!/usr/bin/env python3
import sys

from tree_successor import Tree

def test_tree(tree, sequence):
    node = tree.successor(None)
    for element in sequence:
        assert node is not None, "Expected successor {}, got None".format(element)
        assert node.key == element, "Expected successor {}, got {}".format(element, node.key)
        node = tree.successor(node)
    assert node is None, "Expected no successor, got {}".format(node.key)

def test_random_tree():
    sequence = [pow(997, i, 199999) for i in range(1, 199999)]
    tree = Tree()
    for i in sequence:
        tree.insert(i)
    sequence.sort()
    test_tree(tree, sequence)

def test_path(right):
    sequence = [int(7.13*i) for i in range(1000000)]
    tree = Tree()
    node = None
    sequence_insert = sequence if right else reversed(sequence)
    for key in sequence_insert:
        node = tree.insert(key, node)
    test_tree(tree, sequence)

tests = [
    ("random_tree", test_random_tree),
    ("right_path", lambda: test_path(True)),
    ("left_path", lambda: test_path(False)),
]

if __name__ == "__main__":
    for required_test in sys.argv[1:] or [name for name, _ in tests]:
        for name, test in tests:
            if name == required_test:
                print("Running test {}".format(name), file=sys.stderr)
                test()
                break
        else:
            raise ValueError("Unknown test {}".format(name))
