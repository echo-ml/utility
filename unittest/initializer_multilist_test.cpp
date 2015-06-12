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
