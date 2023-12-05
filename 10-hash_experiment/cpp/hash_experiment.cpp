#include <vector>
#include <functional>
#include <algorithm>
#include <utility>
#include <stdexcept>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "random.h"

using namespace std;

RandomGen rng(42);

typedef uint32_t uint;

typedef function<uint(uint)> HashFunction;
typedef function<HashFunction(unsigned num_buckets)> HashFunctionFactory;

/*
 * Hash function for hashing by tabulation.
 *
 * The 32-bit key is split to four 8-bit parts. Each part indexes
 * a separate table of 256 randomly generated values. Obtained values
 * are XORed together.
 */
class TabulationHash {
    unsigned num_buckets;
    vector<uint> tables;

    TabulationHash(unsigned num_buckets) : num_buckets(num_buckets), tables(4 * 256) {
        for (uint& x : tables) x = rng.next_u32();
    }

  public:
    static HashFunction factory(unsigned num_buckets) {
        return HashFunction(TabulationHash(num_buckets));
    }

    uint operator()(uint key) {
        return (
            tables[key & 0xff] ^
            tables[((key >> 8) & 0xff) | 0x100] ^
            tables[((key >> 16) & 0xff) | 0x200] ^
            tables[((key >> 24) & 0xff) | 0x300]
        ) % num_buckets;
    }
};

// Hash function using polynomial modulo a prime.
template < int degree, uint prime = 2147483647 >
class PolynomialHash {
    unsigned num_buckets;
    vector<uint> coefs;

    PolynomialHash(unsigned num_buckets) : num_buckets(num_buckets), coefs(degree + 1) {
        for (uint& x : coefs) x = rng.next_u32();
    }

  public:
    static HashFunction factory(unsigned num_buckets) {
        return HashFunction(PolynomialHash(num_buckets));
    }

    uint operator()(uint key) {
        uint64_t acc = 0;
        for (uint c : coefs) acc = (acc * key + c) % prime;
        return (uint)(acc % num_buckets);
    }
};

typedef PolynomialHash<1> LinearHash;
typedef PolynomialHash<2> QuadraticHash;

// Multiply-shift hash function taking top bits of 32-bit word
// Note: not evaluated in Makefile; its inclusion in experiments is voluntary
class MultiplyShiftLowHash {
    uint mult;
    uint mask;
    int shift = 0;

    MultiplyShiftLowHash(unsigned num_buckets) {
        mult = rng.next_u32() | 0x1;
        mask = num_buckets - 1;

        if (mask & num_buckets)
            throw runtime_error("MultiplyShiftLowHash: num_buckets must be power of 2");

        unsigned tmp = num_buckets - 1;
        while ((0x80000000U & tmp) == 0) {
            tmp <<= 1;
            shift++;
        }
    }

  public:
    static HashFunction factory(unsigned num_buckets) {
        return HashFunction(MultiplyShiftLowHash(num_buckets));
    }

    uint operator()(uint key) {
        return ((key * mult) >> shift) & mask;
    }
};

// Multiply-shift hash function taking low bits of upper half of 64-bit word
class MultiplyShiftHighHash {
    uint mask;
    uint64_t mult;

    MultiplyShiftHighHash(unsigned num_buckets) {
        mult = rng.next_u64() | 0x1;
        mask = num_buckets - 1;

        if (mask & num_buckets)
            throw runtime_error("MultiplyShiftHighHash: num_buckets must be power of 2");
    }

  public:
    static HashFunction factory(unsigned num_buckets) {
        return HashFunction(MultiplyShiftHighHash(num_buckets));
    }

    uint operator()(uint key) {
        return ((key * mult) >> 32) & mask;
    }
};

// Hash table with linear probing
class HashTable {
    HashFunction hash;
    vector<uint> table;
    unsigned size = 0;

    unsigned ops;
    unsigned max_;
    uint64_t steps;

  public:
    // We reserve one integer to mark unused buckets. This integer
    // cannot be stored in the table.
    static constexpr uint UNUSED = ~((uint)0);

    HashTable(const HashFunctionFactory& factory, unsigned num_buckets) :
        hash(factory(num_buckets)), table(num_buckets, +UNUSED) {
        reset_counter();
    }

    // Check whether key is present in the table.
    bool lookup(uint key) {
        if (key == UNUSED) throw runtime_error("Cannot lookup UNUSED");

        bool ret = false;
        unsigned steps = 1;

        uint b = hash(key);
        while (table[b] != UNUSED) {
            if (table[b] == key) {
                ret = true;
                break;
            }
            steps++;
            b = next_bucket(b);
        }

        update_counter(steps);
        return ret;
    }

    // Add the key in the table.
    void insert(uint key) {
        if (key == UNUSED) throw runtime_error("Cannot insert UNUSED");
        if (size >= table.size()) throw runtime_error("Insert: Table is full");

        unsigned steps = 1;
        uint b = hash(key);

        while (table[b] != UNUSED) {
            if (table[b] == key) goto key_found;
            steps++;
            b = next_bucket(b);
        }

        table[b] = key;
        size++;

      key_found:
        update_counter(steps);
    }

/*
    Return expected number of steps for removing one random element.
    I.e. the average number of positions between an element's hash position and the first empty position.
    Note: not used in experiments
*/
    double delete_avg() {
        vector<unsigned> hashed(table.size(), 0);
        for(uint x : table)
            if(x != UNUSED)
                hashed[hash(x)]++;

        const unsigned first_unused = distance(table.begin(), find_if(table.begin(), table.end(), [](uint x){ return x == UNUSED; }));
        unsigned total_steps = 0, elements = 0;
        for(unsigned i = first_unused+1; i < first_unused+table.size(); i++)
            if(table[i % table.size()] == UNUSED)
                elements = 0;
            else {
                elements += hashed[i % table.size()];
                total_steps += elements;
            }
        
        return (double)total_steps / size;
    }

    void reset_counter() { ops = steps = max_ = 0; }
    double report_avg() { return ((double)steps) / max(1U, ops); }
    double report_max() { return max_; }

  private:
    void update_counter(unsigned steps) {
        ops++;
        this->steps += steps;
        max_ = max(steps, max_);
    }

    unsigned next_bucket(unsigned b) { return (b + 1) % table.size(); }
};

// Usage test with inserting a prefix of 1...N
void usageSeq_test(HashFunctionFactory factory, int max_usage = 90, int retry = 100) { 
    vector<double> avg(max_usage, 0.0);
    vector<double> avg2(max_usage, 0.0);
    vector<double> maximum(max_usage, 0.0);

    unsigned N = 1 << 20;
    unsigned step_size = N / 100;

    for (int t = 0; t < retry; t++) {
        HashTable H(factory, N);

        for (int s = 0; s < max_usage; s++) {
            H.reset_counter();
            for (unsigned i = 0; i < step_size; i++)
                H.insert((s*step_size + i));

            avg[s] += H.report_avg();
            avg2[s] += H.report_avg() * H.report_avg();
            maximum[s] = max(maximum[s], H.report_avg());
        }
    }

    for (int i = 0; i < max_usage; i++) {
        avg[i] /= retry;
        avg2[i] /= retry;
        double std_dev = sqrt(avg2[i] - avg[i]*avg[i]);

        printf("%i %.03lf %.03lf %.03lf\n", i+1, avg[i], std_dev, maximum[i]);
    }
}

// Usage test with inserting a prefix of a random permutation of 1...N
// Note: this test is not evaluated in Makefile; its inclusion in experiments is voluntary
void usageRnd_test(HashFunctionFactory factory, int max_usage = 90, int retry = 100) {
    vector<double> avg(max_usage, 0.0);
    vector<double> avg2(max_usage, 0.0);
    vector<double> maximum(max_usage, 0.0);

    unsigned N = 1 << 20;
    unsigned step_size = N / 100;

    vector<uint> elements(N);
    for (unsigned i = 0; i < N; i++) elements[i] = i;

    for (int t = 0; t < retry; t++) {
        HashTable H(factory, N);
        for (unsigned i = 0; i < N-1; i++)
            swap(elements[i], elements[i + (rng.next_u32() % (N-i))]);

        for (int s = 0; s < max_usage; s++) {
            H.reset_counter();
            for (unsigned i = 0; i < step_size; i++)
                H.insert(elements[s*step_size + i]);

            avg[s] += H.report_avg();
            avg2[s] += H.report_avg() * H.report_avg();
            maximum[s] = max(maximum[s], H.report_avg());
        }
    }

    for (int i = 0; i < max_usage; i++) {
        avg[i] /= retry;
        avg2[i] /= retry;
        double std_dev = sqrt(avg2[i] - avg[i]*avg[i]);

        printf("%i %.03lf %.03lf %.03lf\n", i+1, avg[i], std_dev, maximum[i]);
    }
}

// Grow test with inserting 1...0.6*N
void growSeq_test(HashFunctionFactory factory, int usage = 60, int retry = 100,
               int begin = 7, int end = 22) {

    for (int n = begin; n < end; n++) {
        double avg = 0;
        double avg2 = 0;
        double maximum = 0;
        unsigned N = 1 << n;

        vector<uint> elements(N);
        for (unsigned i = 0; i < N; i++) elements[i] = i;

        for (int t = 0; t < retry; t++) {
            HashTable H(factory, N);

            for (unsigned i = 0; i < ((uint64_t)N) * usage / 100; i++)
                H.insert(elements[i]);

            //for (unsigned i = 0; i < N; i++)
            //    H.lookup(i);

            avg += H.report_avg();
            avg2 += H.report_avg() * H.report_avg();
            maximum = max(maximum, H.report_avg());
        }

        avg /= retry;
        avg2 /= retry;
        double std_dev = sqrt(avg2 - avg*avg);

        printf("%i %.03lf %.03lf %.03lf\n", N, avg, std_dev, maximum);
    }
}

// Grow test with inserting a prefix (first 60%) of a random permutation of 1...N
// Note: this test is not evaluated in Makefile; its inclusion in experiments is voluntary
void growRnd_test(HashFunctionFactory factory, int usage = 60, int retry = 100, 
               int begin = 7, int end = 22) {

    for (int n = begin; n < end; n++) {
        double avg = 0;
        double avg2 = 0;
        double maximum = 0;
        unsigned N = 1 << n;

        vector<uint> elements(N);
        for (unsigned i = 0; i < N; i++) elements[i] = i;

        for (int t = 0; t < retry; t++) {
            HashTable H(factory, N);
            for (unsigned i = 0; i < N-1; i++)
                swap(elements[i], elements[i + (rng.next_u32() % (N-i))]);

            for (unsigned i = 0; i < ((uint64_t)N) * usage / 100; i++)
                H.insert(elements[i]);

            for (unsigned i = 0; i < N; i++)
                H.lookup(i);

            avg += H.report_avg();
            avg2 += H.report_avg() * H.report_avg();
            maximum = max(maximum, H.report_avg());
        }

        avg /= retry;
        avg2 /= retry;
        double std_dev = sqrt(avg2 - avg*avg);

        printf("%i %.03lf %.03lf %.03lf\n", N, avg, std_dev, maximum);
    }
}

int main(int argc, char** argv) {
    vector<pair<string, HashFunctionFactory>> growRnd_tests = {
        {"growRnd-ms-low", MultiplyShiftLowHash::factory},
        {"growRnd-ms-high", MultiplyShiftHighHash::factory},
        {"growRnd-poly-1", LinearHash::factory},
        {"growRnd-poly-2", QuadraticHash::factory},
        {"growRnd-tab", TabulationHash::factory}
    };
    vector<pair<string, HashFunctionFactory>> growSeq_tests = {
        {"growSeq-ms-low", MultiplyShiftLowHash::factory},
        {"growSeq-ms-high", MultiplyShiftHighHash::factory},
        {"growSeq-poly-1", LinearHash::factory},
        {"growSeq-poly-2", QuadraticHash::factory},
        {"growSeq-tab", TabulationHash::factory}
    };
    vector<pair<string, HashFunctionFactory>> usageRnd_tests = {
        {"usageRnd-ms-low", MultiplyShiftLowHash::factory},
        {"usageRnd-ms-high", MultiplyShiftHighHash::factory},
        {"usageRnd-poly-1", LinearHash::factory},
        {"usageRnd-poly-2", QuadraticHash::factory},
        {"usageRnd-tab", TabulationHash::factory}
    };
    vector<pair<string, HashFunctionFactory>> usageSeq_tests = {
        {"usageSeq-ms-low", MultiplyShiftLowHash::factory},
        {"usageSeq-ms-high", MultiplyShiftHighHash::factory},
        {"usageSeq-poly-1", LinearHash::factory},
        {"usageSeq-poly-2", QuadraticHash::factory},
        {"usageSeq-tab", TabulationHash::factory}
    };

    if (argc != 3) goto fail;

    rng = RandomGen(atoi(argv[2]));

    for (auto t : growRnd_tests) {
        if (t.first == argv[1]) {
            growRnd_test(t.second);
            return 0;
        }
    }
    for (auto t : growSeq_tests) {
        if (t.first == argv[1]) {
            growSeq_test(t.second);
            return 0;
        }
    }

    for (auto t : usageRnd_tests) {
        if (t.first == argv[1]) {
            usageRnd_test(t.second);
            return 0;
        }
    }

    for (auto t : usageSeq_tests) {
        if (t.first == argv[1]) {
            usageSeq_test(t.second);
            return 0;
        }
    }

  fail:
    printf("Usage: %s <test> <seed>\nAvailable tests are:", argv[0]);
    for (auto t : growRnd_tests) printf(" %s", t.first.c_str());
    for (auto t : growSeq_tests) printf(" %s", t.first.c_str());
    for (auto t : usageRnd_tests) printf(" %s", t.first.c_str());
    for (auto t : usageSeq_tests) printf(" %s", t.first.c_str());
    return 1;
}

