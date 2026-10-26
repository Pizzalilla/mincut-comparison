# Build with: make          (tests + bench)
#             make test     (build and run the correctness tests)
#             make bench    (build the benchmark harness, optimised)
#             make clean
#
# Tests build with sanitizers and debug info to catch memory bugs early.
# The benchmark builds with -O2 and no sanitizers so timings are meaningful.
# Record any change to BENCH_FLAGS in the README: the report must state them.

CXX         ?= c++
STD         := -std=c++20
WARN        := -Wall -Wextra
INCLUDES    := -Iinclude -Icommon
TEST_FLAGS  := $(STD) $(WARN) $(INCLUDES) -g -O1 -fsanitize=address,undefined
BENCH_FLAGS := $(STD) $(WARN) $(INCLUDES) -O2 -DNDEBUG

ALGO_SRCS   := stoer_wagner/stoer_wagner.cpp karger/karger.cpp \
               karger_stein/karger_stein.cpp
COMMON_SRCS := common/brute_force.cpp common/generators.cpp
HEADERS     := $(wildcard include/*.hpp common/*.hpp)

all: build/tests build/bench

build/tests: tests/tests.cpp $(ALGO_SRCS) $(COMMON_SRCS) $(HEADERS) | build
	$(CXX) $(TEST_FLAGS) tests/tests.cpp $(ALGO_SRCS) $(COMMON_SRCS) -o $@

build/bench: bench/bench.cpp $(ALGO_SRCS) $(COMMON_SRCS) $(HEADERS) | build
	$(CXX) $(BENCH_FLAGS) bench/bench.cpp $(ALGO_SRCS) $(COMMON_SRCS) -o $@

build:
	mkdir -p build

test: build/tests
	./build/tests

bench: build/bench

clean:
	rm -rf build

.PHONY: all test bench clean
