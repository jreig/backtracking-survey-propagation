#pragma once

#include <string>

namespace sat {
  class Metrics {
   public:
    unsigned variables;
    unsigned clauses;
    bool backtrackingEnabled;
    double decimationFraction;
    unsigned SP_MaxIt;
    unsigned SP_Epsilon;
    double WS_Noise;
    unsigned WS_MaxTries;
    unsigned WS_MaxFlipC;
    unsigned long seed;

    std::string result;
    unsigned long totalSeconds;
    unsigned long SP_TotalIt;
    unsigned long SID_TotalIt;
    unsigned long WS_TotalFlips;

    void saveMetrics(const std::string& path);
  };
}  // namespace sat