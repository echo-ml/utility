#include <echo/utility/static_const.h>
#include <echo/test.h>
using namespace echo;

struct Functor {
  template<class Lhs, class Rhs>
  auto operator()(Lhs x, Rhs y) const {
    return x + y;
  }
};

static constexpr auto& functor = static_const<Functor>();

TEST_CASE("static_const") {
  CHECK(functor(2,3) == 5);
}
