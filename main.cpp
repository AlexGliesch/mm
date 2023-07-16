#include "main.h"
#include "pdb.h"
#include "astar.h"
#include "random.h"
#include "state.h"
#include <cassert>
#include <string>

/**
 * Initialize global data structures:
 *  - incidence arrays
 *  - pdbs
 *  - random seeds
 *  - etc
 */
void
init()
{
  // Prepare incidence arrays. These will never be changed again
  for (int i = 0, idx = 0; i < DU; ++i)
    for (int j = 0; j < DV; ++j)
      for (int k = 0; k < DW; ++k, ++idx) {
        assert(idx < PSIZE);
        U_incident[idx] = i;
        V_incident[idx] = j;
        W_incident[idx] = k;
      }

  // Initialize random number generation
  rng.seed(unique_random_seed());

  // Initialize PDBs
  init_pdbs();
}

/**
 *
 */
void
check_222()
{
  assert(N == 2 and M == 2 and P == 2);
  State s = initial_state();
  const int u[7] = {0b1001, 0b0011, 0b1000, 0b0001, 0b1100, 0b1010, 0b0101};
  const int v[7] = {0b1001, 0b1000, 0b0101, 0b1010, 0b0001, 0b1100, 0b0011};
  const int w[7] = {0b1001, 0b0011, 0b0101, 0b1010, 0b1100, 0b0001, 0b1000};

  for (int i = 0; i < 7; ++i) {
    s.apply_move(u[i], v[i], w[i]);

    std::string str(PSIZE, '0');
    for (int j = 0; j < PSIZE; ++j)
      if (s.bs[j])
        str[j] = '1';

    // TODO use print_bitset
    printf("ones: %d s: %s\n", (int)s.bs.count(), str.c_str());
  }
}

/**
 *
 */
void
check_subset()
{
  constexpr int MAX_TESTS = 100000;
  for (int k = 3; k <= 4; ++k) {
    for (int i = 0; i < MAX_TESTS; ++i) {
      State initial = random_state_with_ones(k);
      const int cost = a_star(initial, PDB_MAX, false);

      int ones[initial.num_ones], n = 0;
      for (int i = 0; i < PSIZE; ++i)
        if (initial.bs[i])
          ones[n++] = i;
      assert(n == k);

      if (n >= 2)
        for (int i = 0; i < n; ++i) {
          State s2 = initial;
          s2.bs[ones[i]] = false;
          s2.num_ones--;
          const int s2_cost = a_star(s2, PDB_MAX, false);
          // printf("A* without i: %d; cost: %d\n", i, s2_cost);
          if (s2_cost > cost) {
            printf(
              "s2_cost: %d, cost: %d, n: %d, i: %d\n", s2_cost, cost, n, i);
          }
          // assert(s2_cost <= cost);
        }

      if (n >= 3)
        for (int i = 0; i < n; ++i)
          for (int j = i + 1; j < n; ++j) {
            State s2 = initial;
            s2.bs[ones[i]] = false;
            s2.bs[ones[j]] = false;
            s2.num_ones -= 2;
            const int s2_cost = a_star(s2, PDB_MAX, false);
            // printf("A* without i: %d, j: %d; cost: %d\n", i, j, s2_cost);
            if (s2_cost > cost) {
              printf("s2_cost: %d, cost: %d, n: %d, i: %d, j: %d\n",
                     s2_cost,
                     cost,
                     n,
                     i,
                     j);
            }
            // assert(s2_cost <= cost);
          }

      if (n >= 4)
        for (int i = 0; i < n; ++i)
          for (int j = i + 1; j < n; ++j)
            for (int k = j + 1; k < n; ++k) {
              State s2 = initial;
              s2.bs[ones[i]] = false;
              s2.bs[ones[j]] = false;
              s2.bs[ones[k]] = false;
              s2.num_ones -= 3;
              const int s2_cost = a_star(s2, PDB_MAX, false);
              // printf(
              // "A* without i: %d, j: %d, k: %d; cost: %d\n", i, j, k,
              // s2_cost);
              if (s2_cost > cost) {
                printf("s2_cost: %d, cost: %d, n: %d, i: %d, j: %d,k: %d\n",
                       s2_cost,
                       cost,
                       n,
                       i,
                       j,
                       k);
              }
              // assert(s2_cost <= cost);
            }
      if (i % 100 == 0)
        printf("Test %d/%d, k: %d is ok.\n", i, MAX_TESTS, k);
    }
  }
}

/**
 *
 */
int
main()
{
  init();
  check_subset();
  // check_222();
  // const int cost = a_star(initial_state(), PDB_MAX, false);
  // printf("A* done; cost: %d\n", cost);
}