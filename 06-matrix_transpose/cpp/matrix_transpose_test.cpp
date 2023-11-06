#include <functional>
#include <vector>
#include <iostream>
#include <iomanip>

using namespace std;

// If the condition is not true, report an error and halt.
#define EXPECT(condition, message) do { if (!(condition)) expect_failed(message); } while (0)

void expect_failed(const string& message);

#include "matrix_tests.h"

void generic_test(unsigned N, unsigned M, unsigned B, double max_ratio, int debug_level)
{
    TestMatrix m(N, M, B, debug_level);
    m.fill_matrix();
    m.reset_stats();
    m.transpose();

    cout << "\t" << m.stat_cache_misses << " misses in " << m.stat_accesses << " accesses\n";
    if (m.stat_accesses) {
        double swaps = N * (N-1) / 2;
        double mpa = (double) m.stat_cache_misses / swaps;
        double lb = 2. / B;
        double ratio = mpa / lb;
        cout << "\t" <<
		std::fixed << std::setprecision(6) <<
		mpa << " misses/swap (lower bound is " << lb <<
		" => ratio " << ratio <<
		", limit " << max_ratio << ")\n";
        EXPECT(ratio <= max_ratio, "Algorithm did too many I/O operations.");
    }

    m.check_result();
}

/*** A list of all tests ***/

vector<pair<string, function<void()>>> tests = {
//    name                                N      M     B  max_ratio  debug_level
    { "small2k",     [] { generic_test(   8,    32,    8,         8,     2 ); } },
    { "small",       [] { generic_test(  13,    64,    8,         5,     2 ); } },
    { "n100b16",     [] { generic_test( 100,  1024,   16,         3,     1 ); } },
    { "n1000b16",    [] { generic_test(1000,  1024,   16,         3,     1 ); } },
    { "n1000b64",    [] { generic_test(1000,  8192,   64,         4,     1 ); } },
    { "n1000b256",   [] { generic_test(1000, 65536,  256,         7,     1 ); } },
    { "n1000b4096",  [] { generic_test(1000, 65536, 4096,        60,     1 ); } },
};
