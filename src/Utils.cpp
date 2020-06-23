#include <Utils.hpp>
#include <sstream>

namespace sat::utils {

// =============================================================================
// Random number generator
// =============================================================================
std::random_device RandomGen::rd;
std::mt19937 RandomGen::randomGenerator(RandomGen::rd());
std::uniform_int_distribution<> RandomGen::randomBoolUD(0, 1);
std::uniform_real_distribution<> RandomGen::randomReal01UD(0, 1);

// =============================================================================
// String functions
// =============================================================================
const std::vector<std::string> SplitString(const std::string& s) {
  const char delim = ' ';
  std::stringstream stream(s);
  std::vector<std::string> tokens;

  std::string token;
  while (std::getline(stream, token, delim)) {
    tokens.push_back(token);
  }

  return tokens;
}
}  // namespace sat::utils