#include <echo/utility/functional.h>
#include <echo/test.h>
using namespace echo;
using namespace echo::functional;

TEST_CASE("functional") {
  CHECK(and_(true, true));
}
