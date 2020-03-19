#include <catch2/catch.hpp>

#include <BacktrackingSP.hpp>

TEST_CASE("BacktrackingSP - fake unit test", "[unit]") {
  bsp::BacktrackingSP solver;

  REQUIRE(solver.IsSAT(NULL));
};