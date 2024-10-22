#include <algorithm>
#include <cassert>
#include <fstream>
#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "splay_operation.h"

using namespace std;

const int elements = 5000000;

void test_failed_insert() {

    // Test speed
    {
        Tree tree;
        for (int i = 0; i < elements; i++)
            tree.insert(i);
        
        for (int i = 0; i < 2*elements; i++)
            tree.insert(0);
    }
}

vector<pair<string, function<void()>>> tests = {
    { "failed_insert", test_failed_insert },
};
