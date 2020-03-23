#include <catch2/catch.hpp>
#include <string>

#include <BacktrackingSP.hpp>

TEST_CASE("BacktrackingSP - fake unit test", "[unit]") {
  bsp::BacktrackingSP solver("invalid path", 0.5f, 10);

  REQUIRE(solver.IsSAT());
};