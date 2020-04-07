#include <BSPSolver.hpp>
#include <catch2/catch.hpp>
#include <string>

TEST_CASE("BSPSolver - fake unit test", "[unit]") {
  bsp::BSPSolver solver(0.001f, 1000, 1234);

  REQUIRE(true);
};