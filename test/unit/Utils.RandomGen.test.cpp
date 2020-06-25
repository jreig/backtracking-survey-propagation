#include <catch2/catch.hpp>
#include <iostream>

// Project headders
#include <Utils.hpp>

using namespace sat::utils;

TEST_CASE("Utils - RandomGen (bool)", "[unit]") {
  // Fix seed
  RandomGen::setSeed(7357);

  bool results[5];

  for (uint i = 0; i < 5; i++) {
    results[i] = RandomGen::getRandomBool();
  }

  CHECK_FALSE(results[0]);
  CHECK_FALSE(results[1]);
  CHECK_FALSE(results[2]);
  CHECK(results[3]);
  CHECK_FALSE(results[4]);
};
TEST_CASE("Utils - RandomGen (real)", "[unit]") {
  // Fix seed
  RandomGen::setSeed(7357);

  double results[5];

  for (uint i = 0; i < 5; i++) {
    results[i] = RandomGen::getRandomReal01();
  }

  CHECK(results[0] - 0.1297653309f < 0.001f);
  CHECK(results[1] - 0.5826444825f < 0.001f);
  CHECK(results[2] - 0.021574746949f < 0.001f);
  CHECK(results[3] - 0.6759686312f < 0.001f);
  CHECK(results[4] - 0.8261042689f < 0.001f);
};