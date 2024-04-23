#include <vector>
#include <cstdint>
#include <iostream>

const uint32_t UNUSED = 0xffffffff;

template<class Hash>
class CuckooTable {
    /*
     * Hash table with Cuckoo hashing.
     *
     * We have two hash functions, which map 32-bit keys to buckets of a common
     * hash table. Unused buckets contain 0xffffffff.
     */

    // The array of buckets
    vector<uint32_t> table;
    uint32_t num_buckets;

    // Hash functions and the random generator used to create them
    array<Hash,2> &hashes;

public:

    CuckooTable(uint32_t num_buckets, array<Hash,2> &hashes) : num_buckets{num_buckets}, hashes{hashes}
    {
        // Initialize the table with the given number of buckets.
        // The number of buckets is expected to stay constant.

        table.resize(num_buckets, UNUSED);

    }

    const vector<uint32_t>& get_table() const {
        return table;
    }

    bool lookup(uint32_t key) const {
        // Check if the table contains the given key. Returns True or False.
        uint32_t h0 = hashes[0].hash(key);
        uint32_t h1 = hashes[1].hash(key);
        return (table[h0] == key || table[h1] == key);
    }

    void insert(uint32_t key) {
        // Insert a new key to the table. Assumes that the key is not present yet.
        EXPECT(key != UNUSED, "Keys must differ from UNUSED.");

        // TODO: Implement
    }

    uint32_t rehash(uint32_t key) {
        // Relocate all items using new hash functions and insert a given key.
        for (int i=0; i<2; i++)
            hashes[i].regenerate();

        // TODO: Implement
        return key;
    }
};
