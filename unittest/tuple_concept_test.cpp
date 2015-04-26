#include <echo/utility/tuple_concept.h>
#include <echo/test.h>
#include <array>
#include <vector>

using namespace echo;

TEST_CASE("tuple_concept") {
  using T1 = std::tuple<int>;
  using T2 = std::tuple<int&>;
  using T3 = std::pair<double, int>;
  using T4 = std::array<int, 7>;

  CHECK(tuple::concept::tuple<T1>());
  CHECK(tuple::concept::tuple<T2>());
  CHECK(tuple::concept::tuple<T3>());
  CHECK(tuple::concept::tuple<T4>());

  CHECK(!tuple::concept::tuple<int>());
  CHECK(!tuple::concept::tuple<std::vector<int>>());
}
