#include "main.h"
#include "astar.h"
#include <cassert>
#include <vector>
#include <stack>
#include <queue>
#include "timer.h"
#include "state.h"
#include "random.h"
#include "parallel_hashmap/phmap.h"

/**
 * Run A* on an initial state.
 *
 * @param initial state
 * @param pdb_max maximum PDB size allowed for a heuristic
 * @param verbose whether to print messages to stdout
 *
 * @return the best cost found.
 */
int
a_star(State initial, int pdb_max, const bool verbose)
{
  int upper_bound = std::min((int)initial.bs.count(),
                             pdb_max == PDB_MAX ? UPPER_BOUND : pdb_max);
  pdb_max = std::min(pdb_max, upper_bound);

  std::priority_queue<State> pq;
  State s, s2, best;
  best.g = INT_MAX;
  int nodes_expanded = 0;
  phmap::flat_hash_map<std::bitset<PSIZE>, int> dist;

  int ones_list[PSIZE]; // temporary array
  Timer timer(TIME_LIMIT_S);

  initial.compute_h(pdb_max);

  if (verbose) {
    printf("N: %d\nM: %d\nP: %d\nPSIZE: %d\n", N, M, P, PSIZE);
    printf(
      "Initial ones: %d\nUpper bound: %d\nPDB max: %d\nInitial state (bits): ",
      initial.num_ones,
      upper_bound,
      pdb_max);
    print_bitset(initial.bs);
  }

  pq.push(initial);

  if (USE_HASH) {
    dist[initial.bs] = 0;
  }

  while (pq.size()) {
    s = pq.top();
    pq.pop();

    ++nodes_expanded;

    assert((int)s.bs.count() == s.num_ones);

    // Stop because g-values don't match (since we are not using updates)
    if (USE_HASH and dist[s.bs] != s.g)
      continue;

    // Break because of node limit
    if (nodes_expanded >= MAX_NODES)
      break;

    // Break because of time limit
    if (timer.TimedOut())
      break;

    // Get list of ones
    [[maybe_unused]] int ones = 0;
    for (int i = 0; i < PSIZE; ++i)
      if (s.bs[i])
        ones_list[ones++] = i;
    assert(ones == s.num_ones);

    // Print to stdout?
    if (verbose and nodes_expanded % PRINT_INTERVAL == 0) {
      printf(
        "#%-3d %-6.2lf open %-10zu closed %-10zu f %-2d g %-2d budget %-2d "
        "gc %d/%d ones %d\n",
        nodes_expanded,
        timer.Elapsed(),
        pq.size(),
        dist.size(),
        s.f(),
        s.g,
        s.budget,
        PSIZE - s.num_ones,
        PSIZE,
        ones);
    }

    // Found goal state?
    if (s.is_goal()) {
      // Update best solution found so far
      if (s.g < best.g) {
        best = s;
        if (verbose)
          printf("-- Found best (g = %d, num_ones = %d), upper_bound: %d, "
                 "nodes: %d, "
                 "time: %.2lf\n",
                 s.g,
                 s.num_ones,
                 upper_bound,
                 nodes_expanded,
                 timer.Elapsed());
        if (STOP_WHEN_SOL_FOUND)
          return s.g;
      }
      continue; // goal state - no need to continue expanding
    }

    static_assert(DU + DV + DW < 64); // Must fit in 64bit integer
                                      // If not, we'll need something else

    const int max_ones_to_remove = std::min(ones, MAX_ONES_DECREASE);

    // TODO rewrite these loops

    // For each subset of r 1's in the solution, try to remove it
    for (int r = 1; r <= max_ones_to_remove; ++r) {
      static_assert(N * M * P <= 32,
                    "Won't work, because 1<<ones doesn't fit (see below).");
      for (int i = 1; i < (1 << ones); ++i) {
        if (count_ones(i) != r)
          continue;
        int u = 0, v = 0, w = 0, count = 0;
        for (int j = 0; j < ones; ++j) {
          if (i & (1 << j)) {
            u |= (1 << U_incident[ones_list[j]]);
            v |= (1 << V_incident[ones_list[j]]);
            w |= (1 << W_incident[ones_list[j]]);
            ++count;
          }
        }
        assert(count == r);

        // Visit the new state s2
        s2 = s;
        s2.apply_move(u, v, w);

        // Stop if s2 has more than the maximum number of ones allowed
        if (USE_MAX_ONES and s2.num_ones > upper_bound + MAX_ONES_INCREASE)
          continue;

        s2.g = s.g + 1;
        s2.compute_h(pdb_max);

        // Stop if s2's heuristic exceeds upper bound - only valid if the
        // heuristic is admissible
        if (HEURISTIC_IS_ADMISSIBLE and s2.f() >= upper_bound)
          continue;

        const int ones_delta = s.num_ones - s2.num_ones;

        // Stop if we are creating more ones than the maximum allowed in a
        // single move
        if (ones_delta > MAX_ONES_INCREASE)
          continue;

        const bool budget_ok = s2.budget > 0 // (has budget)
                               or ones_delta <= 0; // (# ones didn't increase)

        // Stop if we don't have the budget for more GC decreases
        if (not budget_ok)
          continue;

        if (ones_delta > 0) // number of ones increased: use budget
          --s2.budget;

        if (USE_HASH) {
          const auto it = dist.find(s2.bs);
          if (it == dist.end() or (ALLOW_REEXPANSIONS and it->second > s2.g)) {
            dist[s2.bs] = s2.g;
            pq.push(s2);
          }
        } else {
          pq.push(s2);
        }
      } // for i
    } // for r
  }
  return std::min(best.g, upper_bound);
}