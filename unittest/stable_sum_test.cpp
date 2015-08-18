#include <echo/utility/stable_sum.h>
#include <echo/test.h>
#include <random>
#include <iostream>
#include <limits>

using namespace echo;

template <class T, class Iterator>
T sum(Iterator first, Iterator last) {
  T sum = 0;
  for (Iterator i = first; i != last; ++i) sum += *i;
  return sum;
}

TEST_CASE("stable_sum") {
  using Scalar = float;
  std::mt19937 generator(0);
  std::uniform_real_distribution<Scalar> dist(-1e9, 1e9);
  std::vector<Scalar> xs;
  for (int i = 0; i < 100000; ++i) {
    auto x = dist(generator);
    xs.push_back(x);
    xs.push_back(-x);
  }
  std::shuffle(xs.begin(), xs.end(), generator);

  float sum1 = sum<float>(xs.begin(), xs.end());
  float sum2 = kahan_sum<float>(xs.begin(), xs.end());
  CHECK(sum2 == kahan_sum<float>(xs));
  CHECK((0 < std::abs(sum2) && std::abs(sum2) < std::abs(sum1)));

  float sum3 = stable_sum(xs.begin(), xs.end());
  CHECK(sum3 == stable_sum(xs));
  CHECK(sum3 == 0.0f);
}
