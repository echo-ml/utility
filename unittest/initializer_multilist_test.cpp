#include <echo/utility/initializer_multilist.h>
#include <echo/test.h>

using namespace echo::initializer;

TEST_CASE("initializer_multilist_accessor") {
  auto accessor = InitializerMultilistAccessor<double, 2>({{1, 2, 3}, {4,5,6}});
  CHECK(accessor(0, 0) == 1);
  CHECK(accessor(1, 2) == 6);

  auto extents = accessor.extents();
  CHECK(extents[0] == 2);
  CHECK(extents[1] == 3);
}

TEST_CASE("initialize") {
  int a[3][2];
  auto accessor = [&](int i, int j) -> int& {
    return a[i][j];
  };
  initialize<int, 2>({{1, 2}, {3,4}, {5,6}}, accessor);
  CHECK(a[0][0] == 1);
  CHECK(a[0][1] == 2);

  CHECK(a[1][0] == 3);
  CHECK(a[1][1] == 4);

  CHECK(a[2][0] == 5);
  CHECK(a[2][1] == 6);
}
