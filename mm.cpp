#include "mm.h"
#include "timer.h"
#include <cassert>
#include <queue>
#include <unordered_map>

/// @brief Search state
struct State
{
  u32 gc = 0;
  u32 g = 0;
  VariableSet bs;

  /// @brief order by goal count and g, for now
  bool operator<(const State& o) const
  {
    // return std::make_pair(-(int)g, gc) < std::make_pair(-(int)o.g, o.gc);
    return std::make_pair(gc, -(int)g) < std::make_pair(o.gc, -(int)o.g);
  }

  /// @brief apply a move defined by u,v, and w
  void ApplyMove(const u32 _u, const u32 _v, const u32 _w)
  {
    VariableSet o;

    // Compute outer product
    for (u32 i = 0, idx = 0; i < DU; ++i) {
      for (u32 j = 0; j < DV; ++j) {
        for (u32 k = 0; k < DW; ++k, ++idx) {
          assert(idx == Idx(i, j, k));
          o[idx] = (_u & (1 << i)) && (_v & (1 << j)) && (_w & (1 << k));
        }
      }
    }
    // printf("Flat: ");
    // PrintBitset(o);
    bs ^= o;
  }

  /// @brief get the goal count for this state
  u32 GoalCount() const { return (u32)(NUM_VARS - bs.count()); }

  /// @brief test whether a state is a goal state
  bool IsGoal() const
  {
    assert(gc == GoalCount());
    return gc == NUM_VARS;
  }

  /// @brief get initial state
  static State InitialState()
  {
    State s;
    for (u32 i = 0; i < M; ++i)
      for (u32 j = 0; j < P; ++j)
        for (u32 k = 0; k < N; ++k)
          s.bs[Idx(N * i + k, P * k + j, P * i + j)] = 1;
    s.gc = s.GoalCount();
    s.g = 0;
    return s;
  }
};

/// @brief run main algorithm
void
RunAlgorithm()
{
  std::priority_queue<State> pq;
  std::unordered_map<std::bitset<NUM_VARS>, u32> dist;
  u32 nodes_expanded = 0;

  State initial = State::InitialState(), s, s2, best;
  pq.push(initial);
  dist[initial.bs] = 0;

  printf("N: %u\nM: %u\nP: %u\nVARS: %u\n", N, M, P, NUM_VARS);
  printf("Initial GC: %u\nInitial state (bits): ", initial.GoalCount());
  PrintBitset(initial.bs);

  Timer timer(TIME_LIMIT_S);
  
  best.g = INT_MAX;

  while (pq.size()) {
    ++nodes_expanded;

    s = pq.top();
    pq.pop();

    if (nodes_expanded % PRINT_INTERVAL == 0)
      printf("#%-3u %-10.2lf open %-10zu closed %-10zu gc %-4u g %-2u\n",
             nodes_expanded,
             timer.Elapsed(),
             pq.size(),
             dist.size(),
             s.gc,
             s.g);

    if (nodes_expanded >= MAX_NODES)
      break;

    if (timer.TimedOut())
      break;

    if (USE_HASH and dist[s.bs] != s.g)
      continue;

    if (s.IsGoal()) {
      if (s.g < best.g) {
        best = s;
        printf("-- Found best (g = %u, gc = %u), nodes: %u\n",
               s.g,
               s.gc,
               nodes_expanded);

        if (STOP_WHEN_SOL_FOUND)
          break;
      }
      continue;
    }

    // generate all possible moves?
    for (u32 u = 0; u < (1U << DU); ++u) {
      for (u32 v = 0; v < (1U << DV); ++v) {
        for (u32 w = 0; w < (1U << DW); ++w) {
          s2 = s;
          s2.ApplyMove(u, v, w);
          s2.g = s.g + 1;
          if (USE_HASH) {
            if (dist.count(s2.bs) == 0 or dist[s2.bs] > s2.g) {
              dist[s2.bs] = s2.g;
              s2.gc = s2.GoalCount();
              pq.push(s2);
            }
          } else {
            s2.gc = s2.GoalCount();
            pq.push(s2);
          }
        }
      }
    }
  }
}

int
main()
{
  RunAlgorithm();
}