#pragma once

#include <time.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>

#include <FactorGraph.hpp>

namespace bsp {

enum BSPResult { UNCONVERGED, SAT, UNSAT, ERROR };

class BacktrackingSP {
 public:
  FactorGraph* graph;
  float threshold;
  clock_t maxTime;

 public:
  explicit BacktrackingSP(std::ifstream& file, float threshold, int maxSeconds);
  explicit BacktrackingSP(std::string route, float threshold, int maxSeconds);
  ~BacktrackingSP();

  BSPResult IsSAT();

 private:
  void InitParams(std::ifstream& file, float threshold, int maxSeconds);

  void UpdateSurvey(Edge* edge);
};

}  // namespace bsp