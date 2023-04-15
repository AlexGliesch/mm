#include <bitset>
#include <cstdint>
#include <climits>
#include <ciso646>
#include <cstdio>

using u32 = uint32_t;

constexpr u32 N = 2;
constexpr u32 M = 3;
constexpr u32 P = 3;
constexpr u32 DU = M * N;
constexpr u32 DV = N * P;
constexpr u32 DW = M * P;
constexpr u32 NUM_VARS = DU * DV * DW;
using VariableSet = std::bitset<NUM_VARS>;

/// @brief Get the flattened index in the 3D tensor
inline u32
Idx(const u32 i, const u32 j, const u32 k)
{
  return i * DV * DW + j * DW + k;
}

/// @brief Print a bitset using printf
template<size_t SZ>
inline void
PrintBitset(std::bitset<SZ> bs)
{
  for (int i = (int)SZ - 1; i >= 0; --i)
    printf("%d", (int)bs[i]);
  printf("\n");
}


constexpr u32 MAX_NODES = 1'000'000; // Max expanded nodes
constexpr double TIME_LIMIT_S = 30; // Seconds
constexpr bool STOP_WHEN_SOL_FOUND =
  false; // Stop immediately once a solution is found
constexpr bool USE_HASH = false; // Use hash table?
constexpr u32 PRINT_INTERVAL = 1;
