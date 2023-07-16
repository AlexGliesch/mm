#pragma once
#include "main.h"
#include "pdb.h"
#include "random.h"
#include <cassert>
#include <numeric>

/**
 * An A* search state.
 */
struct State
{
  int num_ones = 0; // Number of ones in the solution
  int g = 0; // A* g-value
  int h = 0; // A* h-value
  int budget = INITIAL_BUDGET; // TODO explain
  VariableSet bs; // List of variables

  /**
   * @return A*'s f-value
   */
  MM_FORCE_INLINE int f() const { return g + h; }

  /**
   * Node ordering function for a max-heap.
   */
  MM_FORCE_INLINE bool operator<(const State& o) const
  {
    return o.should_be_expanded_first(*this);
  }

  /**
   * Whether this should be expanded before 'o', in an A*.
   */
  MM_FORCE_INLINE bool should_be_expanded_first(const State& o) const
  {
    return std::pair(f(), num_ones) < std::pair(o.f(), num_ones);
    // return std::pair(num_ones, f()) < std::pair(o.num_ones, o.f());
  }

  /**
   * Applies a move comprised ov u,v, and w vectors. Here, u,v, and w
   * represent bit arrays.
   */
  void apply_move(const int _u, const int _v, const int _w)
  {
    VariableSet o;

    static_assert(DU <= 32 and DV <= 32 and DW <= 32,
                  "Make sure DU,DV and DW fit in a 32bit integer.");

    // Compute outer product
    for (int i = 0, idx = 0; i < DU; ++i) {
      for (int j = 0; j < DV; ++j) {
        for (int k = 0; k < DW; ++k, ++idx) {
          o[idx] = (_u & (1 << i)) && (_v & (1 << j)) && (_w & (1 << k));
        }
      }
    }
    bs ^= o;
    num_ones = (int)bs.count();
  }

  /**
   * TODO
   */
  MM_FORCE_INLINE void compute_h(const int pdb_max)
  {
    h = pdb_heuristic(*this, pdb_max);
    // TODO if pdb_max = PSIZE-gc, h is optimal. Do something about it?
  }

  /**
   * Test whether a state is a goal state.
   */
  MM_FORCE_INLINE bool is_goal() const { return num_ones == 0; }
};

/**
 * Get the flattened index in the 3D tensor.
 */
inline int
tensor_idx(const int i, const int j, const int k)
{
  return i * DV * DW + j * DW + k;
}

/**
 * Create the initial state.
 */
inline State
initial_state()
{
  State s;
  for (int i = 0; i < M; ++i)
    for (int j = 0; j < P; ++j)
      for (int k = 0; k < N; ++k)
        s.bs[tensor_idx(N * i + k, P * k + j, P * i + j)] = 1;
  s.num_ones = M * P * N;
  assert(s.num_ones == (int)s.bs.count());
  assert(s.g == 0);
  assert(s.budget == INITIAL_BUDGET);
  return s;
}

/**
 * Create a random state with a given number of ones.
 */
inline State
random_state_with_ones(const int ones)
{
  std::vector<int> indices(PSIZE), chosen;
  std::iota(indices.begin(), indices.end(), 0);
  random_choice(indices, chosen, ones);
  State s;
  for (int i : chosen)
    s.bs[i] = true;
  s.num_ones = ones;
  assert(s.num_ones == (int)s.bs.count());
  return s;
}