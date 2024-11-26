#include <functional>
#include <cstdlib>
#include <vector>
#include <string>
#include <array>
#include <iostream>

#include "hash_functions.h"
#include "cuckoo_hash.h"

template<class Hash>
void inspect_table(const CuckooTable<Hash> &cuckoo, const array<Hash,2> &hashes, uint32_t n, uint32_t table_size, uint32_t step) {
    const vector<uint32_t> &table = cuckoo.get_table();
    EXPECT(table.size() == table_size, "The size of table is given and it is expected not to be changed.");
    for (uint32_t i = 0; i < n; i++) {
        uint32_t k = step*i;
        uint32_t h0 = hashes[0].hash(k), h1 = hashes[1].hash(k);;
        EXPECT(table[h0] == k || table[h1] == k, "Item should be stored on one of two positions given by hash functions.");
        EXPECT(h0 == h1 || table[h0] != k || table[h1] != k, "Item should be stored only on one position.");
    }
    for (uint32_t t = 0; t < table_size; t++) {
        uint32_t k = table[t];
        if (k != UNUSED) {
            EXPECT(k % step == 0 && k < step * n, "Only inserted items should be stored.");
            EXPECT(hashes[0].hash(k) == t || hashes[1].hash(k) == t, "Item should be stored on one of two positions given by hash functions.");
        }
    }
}

void simple_test(uint32_t n, uint32_t table_size_percentage) {
    const uint32_t table_size = n * table_size_percentage / 100;
    RandomGen random_gen(42);
    array<TabulationHash,2> hashes{TabulationHash(table_size, random_gen), TabulationHash(table_size, random_gen)};
    CuckooTable cuckoo(table_size, hashes);

    for (uint32_t i=0; i < n; i++)
        cuckoo.insert(37*i);

    for (uint32_t i=0; i < n; i++) {
        EXPECT(cuckoo.lookup(37*i), "Item not present in table, but it should be.");
        EXPECT(!cuckoo.lookup(37*i+1), "Item present in table, even though it should not be.");
    }

    inspect_table(cuckoo, hashes, n, table_size, 37);
}

void multiple_test(uint32_t min_n, uint32_t max_n, uint32_t step_n, uint32_t table_size_percentage) {
    for (uint32_t n = min_n; n < max_n; n += step_n) {
        printf("\tn=%u\n", n);
        simple_test(n, table_size_percentage);
    }
}

void fixed_test() {
    const uint32_t table_size = FixedHash::table_size;
    array<FixedHash,2> hashes{FixedHash(0), FixedHash(1)};
    CuckooTable cuckoo(table_size, hashes);
    for (uint32_t k = 0; k < FixedHash::keys; k++) {
        cuckoo.insert(k);
    }
    inspect_table(cuckoo, hashes, FixedHash::keys, table_size, 1);
}

/*** A list of all tests ***/

vector<pair<string, function<void()>>> tests = {
    { "small",   [] { simple_test(100, 400); } },
    { "middle",  [] { simple_test(31415, 300); } },
    { "big",     [] { simple_test(1000000, 300); } },
    { "tight",   [] { multiple_test(20000, 40000, 500, 205); } },
    { "fixed",   fixed_test }
};
