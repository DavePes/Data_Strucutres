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

vector<int> get_linear_sequence() {
    vector<int> numbers;
    for (int i = 0; i < 10000000; i++)
        numbers.push_back((int)(7.13*i));
    return numbers;
}

void test_path(bool right) {
    vector<int> numbers = get_linear_sequence();
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

void test_two_paths() {
    vector<int> numbers = get_linear_sequence();
    Tree tree;
    Node *node = nullptr;
    for(size_t i = numbers.size()/2; i < numbers.size(); i++)
        node = tree.insert(numbers[i], node);
    node = nullptr;
    for(int i = numbers.size()/2 - 1; i >= 0; i--)
        node = tree.insert(numbers[i], node);

    test(numbers, tree);
}

void test_sequence(vector<int> &&sequence) {
    Tree tree;
    for (const auto& element : sequence)
        tree.insert(element);

    sort(sequence.begin(), sequence.end());
    test(sequence, tree);
}

void test_random() {
    vector<int> sequence = {997};
    for (int i = 2; i < 199999; i++)
        sequence.push_back((sequence.back() * int64_t(997)) % 199999);
    test_sequence(move(sequence));
}

void test_trivial() {
    test_sequence({5});
    test_sequence({7,9});
    test_sequence({7,3});
    test_sequence({5,3,7});
}

void test_comb() {
    vector<int> numbers = get_linear_sequence();
    Tree tree;
    Node *node = nullptr;
    for(size_t i = numbers.size()/2; i < numbers.size(); i++)
        node = tree.insert(numbers[i], node);
    node = nullptr;
    for(int i = numbers.size()/2 - 1; i >= 0; i-=2) {
        node = tree.insert(numbers[i-1], node);
        tree.insert(numbers[i], node);
    }
    test(numbers, tree);
}

vector<pair<string, function<void()>>> tests = {
    { "trivial", test_trivial },
    { "right_path", []{ test_path(true); } },
    { "left_path", []{ test_path(false); } },
    { "random_tree", test_random },
    { "two_paths", test_two_paths },
    { "comb", test_comb }
};
