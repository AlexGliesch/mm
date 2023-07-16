#include "pdb.h"
#include "astar.h"
#include "main.h"
#include "state.h"
#include "random.h"
#include "parallel_hashmap/phmap.h"
#include <cassert>
#include <array>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <queue>

static_assert(PDB_MAX >= 3);
static_assert(PDB_MAX <= 6); // won't fit in memory

// PDB[k] holds 3^{binom(k,2)} values
// #if PDB_USE_HASH
std::array<std::vector<int>, PDB_MAX + 1> pdb;
// std::array<phmap::flat_hash_map<int, int>, PDB_MAX + 1> pdb;

// The number of PDBs computed for each PDB size
std::array<int, PDB_MAX + 1> pdbs_computed;

/**
 * Get index for a state in the PDB table.
 *
 * @param k PDB size
 * @param one_indices indices of variables set to 1 in a state. Assumed to have
 * size k or more, otherwise this function will have undefined behaviour.
 *
 * @return index for the corresponding state's heuristic in PDB[k]
 */
int
pdb_idx(const int k, const int* const one_indices)
{
  int idx = 0, t = 1;
  for (int i = 0; i < k; ++i) {
    const int onei = one_indices[i];
    assert(onei < PSIZE);
    for (int j = i + 1; j < k; ++j) {
      const int onej = one_indices[j];
      assert(onej < PSIZE);
      assert(onei != onej);
      const int num_share = (U_incident[onei] == U_incident[onej]) +
                            (V_incident[onei] == V_incident[onej]) +
                            (W_incident[onei] == W_incident[onej]);
      idx += num_share * t;
      t *= 3;
    }
  }
  assert(idx < (int)pdb[k].size());
  return idx;
}

/**
 * Get the PDB heuristic for a state in a lazy way, i.e. if it hasn't
 * been computed before, it computes it.
 *
 * Lazy PDB is ok, but precomputed may perform better. How can we generate
 * subinstances that witness all patterns?
 *
 * @param k PDB size
 * @param one_indices indices of variables set to 1 in a state. Assumed to have
 * size k or more, otherwise this function will have undefined behaviour.
 *
 * @return heuristic value for the corresponding state
 */
int
lazy_pdb(const int k, const int* const one_indices)
{
  if (k <= 1)
    return k;
  assert(k >= 2 and k <= PDB_MAX);
  const int idx = pdb_idx(k, one_indices);
  assert(idx >= 0 and idx < (int)pdb[k].size());
  if (pdb[k][idx] == -1) {
    State s;
    for (int i = 0; i < k; ++i)
      s.bs[one_indices[i]] = true;
    s.num_ones = s.bs.count();
    const int cost = a_star(s, k - 1, false);
    pdb[k][idx] = cost; // TODO if parallel, add mutex here

    ++pdbs_computed[k];
    if (false)
      printf("    pdb[%d]: %8d/%-8d (%d)\n",
             k,
             pdbs_computed[k],
             (int)pdb[k].size(),
             cost);
  }
  return pdb[k][idx];
}

/**
 * TODO
 */
void
init_pdbs()
{
  for (int k = 1; k <= PDB_MAX; ++k) {
     const int binom_k_2 = (k * (k - 1)) / 2;
     pdb[k].assign(std::pow(3, binom_k_2), -1);
    printf("PDB %d, size: %d\n", k, (int)pdb[k].size());
  }
  assert(pdb_idx(2, std::array<int, 2>{0, PSIZE - 1}.data()) == 0);
  assert(pdb_idx(2, std::array<int, 2>{0, 1}.data()) == 2);
  assert(pdb[0].empty());
  assert(pdb[1].size() == 1);
  pdb[1][0] = 1;
  assert(pdb[2].size() == 3);
  pdb[2][0] = 2;
  pdb[2][1] = 2;
  pdb[2][2] = 1;
  std::fill(pdbs_computed.begin(), pdbs_computed.end(), 0);
  pdbs_computed[1] = 1;
  pdbs_computed[2] = 3;
}

/**
 * TODO
 */
int
pdb_heuristic(const State& s, int pdb_max)
{
  assert(pdb_max >= 1 and pdb_max <= PDB_MAX);
  int ones = s.num_ones;
  if (ones == 0)
    return 0;
  int one_indices[ones + 1];
  [[maybe_unused]] int size = 0;
  for (int i = 0; i < PSIZE; ++i)
    if (s.bs[i])
      one_indices[size++] = i;
  assert(size == ones);
  assert(ones > 0 and ones <= PSIZE);
  assert(pdb_max == PDB_MAX or not USE_MAX_ONES or
         ones <= pdb_max + 1 + MAX_ONES_INCREASE);

  pdb_max = std::min(ones, pdb_max);

  // return lazy_pdb(pdb_max, one_indices); // This will just take the first
  // pdb_max values

  // Begin additive PDB idea

  const bool VERBOSE = false; // pdb_max == PDB_MAX;

  if (VERBOSE)
    printf("  a_pdb[");

  int heuristic_value = 0;

  while (ones > 0) {
    int group_size = 0;
    int group[pdb_max + 1];
    std::queue<int> q;

    const int r = rand_int(0, ones - 1);
    const int start = one_indices[r];

    std::swap(one_indices[r], one_indices[ones - 1]);
    --ones;
    q.push(start);

    while (q.size() and group_size < pdb_max) {
      const int idx = q.front();
      q.pop();
      group[group_size++] = idx;
      for (int j = 0; j < ones; ++j) {
        // TODO For randomization, we could start j from a random index, too
        const int jdx = one_indices[j];
        assert(jdx < PSIZE);
        if (U_incident[idx] == U_incident[jdx] or
            V_incident[idx] == V_incident[jdx] or
            W_incident[idx] == W_incident[jdx]) {
          q.push(jdx);
          std::swap(one_indices[j], one_indices[ones - 1]);
          --ones, --j;
        }
      }
    }
    // At this point, every node that remains in q is a buffer, and it already
    // got removed from one_indices.
    assert(1 <= group_size and group_size <= pdb_max);
    const int h = lazy_pdb(group_size, group);
    if (VERBOSE) {
      printf("%d(%d)%c", group_size, h, ones == 0 ? ']' : '+');
    }
    heuristic_value += h;
  }
  if (VERBOSE) {
    printf("    o %d (%d)\n", s.num_ones, heuristic_value);
  }
  assert(ones == 0);
  return heuristic_value;
}