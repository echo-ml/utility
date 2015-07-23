#include <echo/utility/ascii_table.h>
#include <echo/test.h>
#include <sstream>

using namespace echo::ascii_table;

TEST_CASE("ascii_table") {
  auto table = Table({Row({Cell("cat", 2, alignment_t::center), Cell("dog")}),
                      Row(), Row({Cell("mouse"), Cell("rat"), Cell("bird")}),
                      Row({Cell("moose"), Cell("giraffe"), Cell("wolf")})});
  const std::string result =
      R"XXX(+-----------------+------+
|       cat       | dog  |
+-------+---------+------+
| mouse | rat     | bird |
| moose | giraffe | wolf |
+-------+---------+------+
)XXX";
  std::ostringstream oss;
  print_table(oss, table);
  CHECK(oss.str() == result);
}
