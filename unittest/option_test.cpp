#include <echo/utility/option.h>
#include <echo/test.h>

using namespace echo;
using namespace echo::option;

enum class level_t { high, low };
constexpr Option<level_t, level_t::high> high{};
constexpr Option<level_t, level_t::low> low{};

enum class speed_t { fast, slow };
constexpr Option<speed_t, speed_t::fast> fast{};
constexpr Option<speed_t, speed_t::slow> slow{};

enum class weight_t { heavy, light };
constexpr Option<weight_t, weight_t::heavy> heavy{};
constexpr Option<weight_t, weight_t::light> light{};

TEST_CASE("option") {
  auto option_set1 = fast | high;
  auto option_set2 = fast | high | heavy;
  using OptionSet1 = decltype(option_set1);
  REQUIRE(has_option<speed_t, OptionSet1>());
  REQUIRE(has_option<level_t, OptionSet1>());
  REQUIRE(!has_option<weight_t, OptionSet1>());
  
  REQUIRE(get_option<speed_t, OptionSet1>() == fast);
  REQUIRE(get_option<speed_t, OptionSet1>() != slow);
  REQUIRE(get_option<weight_t, OptionSet1>() == kNullOption);

  using OptionSet3 = decltype(option_set1 | slow);
  REQUIRE(get_option<speed_t, OptionSet3>() == slow);
}
