test: splay_operation_test splay_operation_more_tests
	./$<
	./splay_operation_more_tests

CXXFLAGS=-std=c++11 -O2 -Wall -Wextra -g -Wno-sign-compare

splay_operation_test: splay_operation.h splay_operation_test.cpp test_main.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

splay_operation_more_tests: splay_operation.h splay_operation_more_tests.cpp test_main.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

clean::
	rm -f splay_operation_test

.PHONY: clean test
