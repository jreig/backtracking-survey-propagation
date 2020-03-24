#include <catch2/catch.hpp>
#include <string>

#include <BacktrackingSP.hpp>

TEST_CASE("BacktrackingSP - fake unit test", "[unit]") {
  bsp::BacktrackingSP solver(0.001f, 1000);

  REQUIRE(true);
};