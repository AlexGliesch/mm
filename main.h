#pragma once
#include <bitset>
#include <cstdint>
#include <climits>
#include <ciso646>
#include <array>
#include <cstdio>
#include "bounds.h"

#define MM_FORCE_INLINE __attribute__((always_inline))

constexpr int DU = M * N; // Size of U arrays
constexpr int DV = N * P; // Size of V arrays
constexpr int DW = M * P; // Size of W arrays
constexpr int PSIZE = DU * DV * DW; // Problem size
constexpr int UPPER_BOUND = N * M * P;

using VariableSet =
  std::bitset<PSIZE>; // Bitset representing a set of variables

// A* constants
constexpr int MAX_NODES = 100'000'000; // Max expanded nodes
constexpr double TIME_LIMIT_S = 300; // Seconds
constexpr bool STOP_WHEN_SOL_FOUND = true; // Stop immediately once a
                                           // solution is found?
constexpr int PRINT_INTERVAL = 1; // Print to stdout every this many nodes
constexpr bool USE_HASH = true; // Whether to use a hash table
constexpr bool ALLOW_REEXPANSIONS = true; // Whether to allow node re-expansions

constexpr int PDB_MAX = 5;
constexpr bool PDB_USE_HASH = (PDB_MAX > 6);

// Suboptimal parameters
constexpr int INITIAL_BUDGET = 4; // TODO
constexpr int MAX_ONES_DECREASE = 4; // TODO
constexpr int MAX_ONES_INCREASE = 4; // TODO

constexpr bool USE_MAX_ONES = true; // TODO

static constexpr bool HEURISTIC_IS_ADMISSIBLE = true;

// Global variables

// These should be set in main()
inline std::array<int, PSIZE> U_incident, V_incident, W_incident;

/**
 * Print a bitset using printf.
 */
template<size_t SZ>
inline void
print_bitset(std::bitset<SZ> bs)
{
  for (int i = (int)SZ - 1; i >= 0; --i)
    printf("%d", (int)bs[i]);
  printf("\n");
}

/**
 * Count the number of ones in the bit representation of an integer.
 */
inline int
count_ones(int n)
{
  int count = 0;
  while (n) {
    n &= (n - 1);
    ++count;
  }
  return count;
}