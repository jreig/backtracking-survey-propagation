#pragma once

#include <random>
#include <string>

namespace sat::utils {

// =============================================================================
// Random number generator
// =============================================================================
class RandomGen {
 public:
  static std::random_device rd;
  static std::mt19937 randomGenerator;
  static std::uniform_int_distribution<> randomBoolUD;
  static std::uniform_real_distribution<> randomReal01UD;

  inline static void setSeed(int seed) { randomGenerator.seed(seed); }
  inline static bool getRandomBool() { return randomBoolUD(randomGenerator); }
  inline static double getRandomReal01() {
    return randomReal01UD(randomGenerator);
  }
};

// =============================================================================
// String functions
// =============================================================================
const std::vector<std::string> SplitString(const std::string& s);

}  // namespace sat::utils