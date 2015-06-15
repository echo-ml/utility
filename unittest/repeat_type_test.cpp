#include <echo/utility/repeat_type.h>
#include <echo/test.h>

using namespace echo;

TEST_CASE("repeat_type") {
  type_equal<repeat_type<double, int>, int>();                                     
  type_equal<repeat_type_c<7, int>, int>(); 
}
