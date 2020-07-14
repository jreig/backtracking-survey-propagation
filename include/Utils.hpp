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
  static unsigned long initialSeed;
  static std::mt19937 randomGenerator;
  static std::uniform_int_distribution<> randomBoolUD;
  static std::uniform_real_distribution<> randomReal01UD;

  inline static void setSeed(int seed) { randomGenerator.seed(seed); }
  inline static bool getRandomBool() {
    return RandomGen::randomBoolUD(RandomGen::randomGenerator);
  }
  inline static float getRandomReal01() {
    return RandomGen::randomReal01UD(RandomGen::randomGenerator);
  }
};

// =============================================================================
// String functions
// =============================================================================
const std::vector<std::string> SplitString(const std::string& s);

}  // namespace sat::utils