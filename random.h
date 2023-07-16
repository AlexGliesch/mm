#pragma once
#include <ctime>
#include <vector>
#include <random>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// Global rng
inline std::mt19937 rng;

/**
 * @return a random integer in range [from, to], inclusive.
 */
template<typename T>
inline T
rand_int(const T from, const T to)
{
  static_assert(std::is_integral<T>::value, "must be integer");
  static std::uniform_int_distribution<T> d;
  return d(rng, typename decltype(d)::param_type{from, to});
}

/**
 * Chooses k elements from v uniformly at random, and places them in
 * 'result'. 'result' will be resized to size k.
 *
 * @param v will select k elements from here
 * @param result will put the k elements here
 * @param k number of elements to sample
 */
template<typename T>
inline void
random_choice(const std::vector<T>& v, std::vector<T>& result, const size_t k)
{
  const size_t n = v.size();
  result.resize(k);
  for (size_t i = 0; i < k; ++i)
    result[i] = v[i];
  for (size_t i = k + 1; i < n; ++i) {
    size_t j = rand_int(size_t{0}, i);
    if (j < k)
      result[j] = v[i];
  }
}

/**
 * @return a unique random seed based on current time and process ID.
 */
inline size_t
unique_random_seed()
{
  size_t a = (size_t)clock(), b = (size_t)time(nullptr);
#ifdef _WIN32
  size_t c = (size_t)GetCurrentProcessId();
#else
  size_t c = (size_t)getpid();
#endif
  a = (a - b - c) ^ (c >> 13);
  b = (b - c - a) ^ (a << 8);
  c = (c - a - b) ^ (b >> 13);
  a = (a - b - c) ^ (c >> 12);
  b = (b - c - a) ^ (a << 16);
  c = (c - a - b) ^ (b >> 5);
  a = (a - b - c) ^ (c >> 3);
  b = (b - c - a) ^ (a << 10);
  c = (c - a - b) ^ (b >> 15);
  return c;
}