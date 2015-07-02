#include <echo/index.h>
#include <echo/test.h>
#include <catch.hpp>

using namespace echo;

TEST_CASE("static index") {
  SECTION("comparison") {
    type_equal<
        decltype(StaticIndex<1>() < StaticIndex<3>())
      , std::true_type
    >();
    type_equal<
        decltype(StaticIndex<1>() == StaticIndex<3>())
      , std::false_type
    >();
  }
  SECTION("arithmetic") {
    type_equal<
        decltype(StaticIndex<1>() + StaticIndex<2>())
      , StaticIndex<3>
    >();
  }
}

TEST_CASE("static index literal") {
  type_equal<
      decltype(123_index)
    , StaticIndex<123>
  >();

  type_equal<
      decltype(-123_index)
    , StaticIndex<-123>
  >();
}

