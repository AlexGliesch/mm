#include <map>
#include <deque>
#include <cassert>

struct CompareGBFSEntries
{
  bool operator()(const std::pair<int, int>& lhs,
                  const std::pair<int, int>& rhs) const
  {
    if (lhs.first != rhs.first) {
      return lhs.first > rhs.first;
    }
    return lhs.second < rhs.second;
  }
};

template<typename State>
class GreedyOpenList
{
  typedef std::deque<State> Bucket;

  std::map<std::pair<int, int>, Bucket, CompareGBFSEntries> buckets;
  int total_size;

public:
  GreedyOpenList()
    : total_size(0)
  {
  }

  void push(const State& entry, const std::pair<int, int>& key)
  {
    buckets[key].push_back(entry);
    ++total_size;
  }

  State pop()
  {
    assert(total_size > 0);
    auto it = buckets.begin();
    assert(it != buckets.end());
    Bucket& bucket = it->second;
    assert(!bucket.empty());
    State result = bucket.front();
    bucket.pop_front();
    if (bucket.empty())
      buckets.erase(it);
    --total_size;
    return result;
  }

  bool empty() { return total_size == 0; }

  size_t size() { return total_size; }
};