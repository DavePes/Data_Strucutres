test: cuckoo_hash_test
	./$<

INCLUDE ?= .
CXXFLAGS=-std=c++20 -O2 -Wall -Wextra -g -Wno-sign-compare -Wno-array-bounds -I$(INCLUDE)

cuckoo_hash_test: cuckoo_hash_test.cpp test_main.cpp cuckoo_hash.h hash_functions.h random.h
	$(CXX) $(CXXFLAGS) $(filter-out %.h,$^)  -o $@

clean:
	rm -f cuckoo_hash_test

.PHONY: clean test
