#include <cstdlib>
#include <string>

#include "random.h"

using namespace std;

// If the condition is not true, report an error and halt.
#define EXPECT(condition, message) do { if (!(condition)) expect_failed(message); } while (0)

void expect_failed(const string& message);

class TabulationHash {
    /*
     * Hash function for hashing by tabulation.
     *
     * The 32-bit key is split to four 8-bit parts. Each part indexes
     * a separate table of 256 randomly generated values. Obtained values
     * are XORed together.
     */

    size_t num_buckets;
    RandomGen &random_gen;
    uint32_t tables[4][256];

public:
    TabulationHash(size_t num_buckets, RandomGen &random_gen) : num_buckets(num_buckets), random_gen(random_gen) {
        regenerate();
    }

    void regenerate() {
        for (int i=0; i<4; i++)
            for (int j=0; j<256; j++)
                tables[i][j] = random_gen.next_u32();
    }

    uint32_t hash(uint32_t key) const {
        uint32_t h0 = key & 0xff;
        uint32_t h1 = (key >> 8) & 0xff;
        uint32_t h2 = (key >> 16) & 0xff;
        uint32_t h3 = (key >> 24) & 0xff;
        return (tables[0][h0] ^ tables[1][h1] ^ tables[2][h2] ^ tables[3][h3]) % num_buckets;
    }
};

class FixedHash {
public:
    static constexpr uint32_t keys = 5, max_regenerations = 6, table_size = 16;

private:
    static constexpr uint32_t hashes[max_regenerations][2][keys] {
        { // Two items hashed into the same bucket by both functions
            { 1, 7, 3, 7, 10 },
            { 2, 7, 4, 7, 11 }
        },
        { // Three items stored in two positions
            { 1, 7, 3, 8, 7 },
            { 2, 8, 4, 7, 8 }
        },
        { // Four items stored in three positions
            { 1, 7, 7, 8, 9 },
            { 2, 8, 9, 7, 8 }
        },
        { // Five should be possible to store in five positions, but the cuckoo's insert operation may not find the proper locations
            { 1, 2, 3, 4, 5 },
            { 2, 3, 4, 5, 1 }
        },
        { // Five should be possible to store in six positions, the timeout in the insert may not be sufficient
            { 1, 2, 3, 4, 5 },
            { 2, 3, 4, 5, 6 }
        },
        { // This should be easy
            { 8, 7, 7, 8, 12 },
            { 11, 6, 7, 9, 0 }
        }
    };

    size_t regenerations, id;

public:
    FixedHash(size_t id) : regenerations{0}, id{id} {}

    uint32_t hash(uint32_t key) const {
        EXPECT(key < keys, "Invalid key");
        return hashes[regenerations][id][key];
    }

    void regenerate() {
        regenerations++;
        EXPECT(regenerations < max_regenerations, "Too many rehashes");
    }
};
