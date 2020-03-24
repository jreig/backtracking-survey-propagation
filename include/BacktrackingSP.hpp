#pragma once

#include <time.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>

#include <FactorGraph.hpp>

namespace bsp {

enum BSPResult { UNCONVERGED, CONVERGED, ERROR };

class BacktrackingSP {
 public:
  FactorGraph* graph;
  float threshold;
  int maxIterations;

 public:
  explicit BacktrackingSP(float threshold, int maxSeconds);
  ~BacktrackingSP();

  BSPResult BSP(std::ifstream& file);

 private:
  void UpdateSurvey(Edge* edge);
};

}  // namespace bsp