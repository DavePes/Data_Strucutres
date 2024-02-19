#include <algorithm>
#include <functional>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "tree_successor.h"

using namespace std;

// If the condition is not true, report an error and halt.
#define EXPECT(condition, message) do { if (!(condition)) expect_failed(message); } while (0)
void expect_failed(const string& message);

void test(const vector<int>& sequence, Tree &tree) {
    Node* node = tree.successor(nullptr);
    for (const auto& element : sequence) {
        EXPECT(node, "Expected successor " + to_string(element) + ", got nullptr");
        EXPECT(node->key == element,
               "Expected successor " + to_string(element) + ", got " + to_string(node->key));
        node = tree.successor(node);
    }
    EXPECT(!node, "Expected no successor, got " + to_string(node->key));
}

void test_path(bool right) {
    vector<int> numbers;
    for (int i = 0; i < 10000000; i++)
        numbers.push_back((int)(7.13*i));

    Tree tree;
    Node *node = nullptr;
    if (right)
        for (int key : numbers)
            node = tree.insert(key, node);
    else
        for (int index = numbers.size() - 1; index >= 0; --index)
            node = tree.insert(numbers[index], node);

    test(numbers, tree);
}

void test_random() {
    vector<int> sequence = {997};
    for (int i = 2; i < 199999; i++)
        sequence.push_back((sequence.back() * int64_t(997)) % 199999);

    Tree tree;
    for (const auto& element : sequence)
        tree.insert(element);

    sort(sequence.begin(), sequence.end());
    test(sequence, tree);
}

vector<pair<string, function<void()>>> tests = {
    { "right_path", []{ test_path(true); } },
    { "left_path", []{ test_path(false); } },
    {"random_tree", test_random },
};
