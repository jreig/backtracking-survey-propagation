#pragma once

#include <random>
#include <sstream>
#include <string>

namespace sat::utils {

// =============================================================================
// Random number generator
// =============================================================================
static std::mt19937 randomGenerator;
static std::uniform_int_distribution<> randomBoolUD(0, 1);
static std::uniform_real_distribution<> randomReal01UD(0, 1);

inline bool getRandomBool() { return randomBoolUD(randomGenerator); }
inline float getRandomReal01() { return randomReal01UD(randomGenerator); }

// =============================================================================
// String functions
// =============================================================================
// -----------------------------------------------------------------------------
// SplitString
//
// Splits the string into token by the given delimiter
// Return a vector with the tokens
// -----------------------------------------------------------------------------
const std::vector<std::string> SplitString(const std::string& s,
                                           const char delim = ' ');

}  // namespace sat::utils