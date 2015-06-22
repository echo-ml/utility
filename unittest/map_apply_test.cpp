#include <echo/utility/map_apply.h>
#include <echo/test.h>
#include <functional>

using namespace echo::utility;

std::false_type map_apply(...);

constexpr struct A {
} a{};

TEST_CASE("map_apply") {
  auto evaluators1 = std::make_tuple([](double i, double j) { return i + j; },
                                     [](double i, double j) { return i * j; });
  const auto& evaluators2 = evaluators1;
  auto result1 = map_apply(std::plus<double>(), evaluators1, 3.0, 7.0);
  auto result2 = map_apply(std::plus<double>(), evaluators1, 3, 7);
  auto result3 = map_apply(std::plus<double>(), evaluators2, 3, 7);
  CHECK(result1 == 31);
  CHECK(result2 == 31);
  CHECK(result3 == 31);

  using Valid1 = decltype(map_apply(std::plus<double>(), evaluators1, 3, a));
  CHECK(std::is_same<Valid1, std::false_type>());

  auto functor = [](A, double x) { return x; };
  using Valid2 = decltype(map_apply(functor, evaluators1, 3, 7));
  CHECK(std::is_same<Valid2, std::false_type>());
}
