#include <echo/utility/ascii_table.h>
#include <echo/test.h>

using namespace echo::ascii_table;

TEST_CASE("ascii_table") {
  auto table = Table({Row({Cell("cat", 2), Cell("dog")}), Row(),
                       Row({Cell("mouse"), Cell("rat"), Cell("bird")})});
  
  print_table(std::cout, table);
}
