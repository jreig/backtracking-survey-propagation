#include <Utils.hpp>

namespace sat::utils {
// =============================================================================
// String functions
// =============================================================================
const std::vector<std::string> SplitString(const std::string& s,
                                           const char delim) {
  std::stringstream stream(s);
  std::vector<std::string> tokens;

  std::string token;
  while (std::getline(stream, token, delim)) {
    tokens.push_back(token);
  }

  return tokens;
}
}  // namespace sat::utils