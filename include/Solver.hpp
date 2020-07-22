#pragma once

#include <FactorGraph.hpp>
#include <random>

using namespace std;

namespace sat {

enum AlgorithmResult {
  CONVERGE,
  UNCONVERGE,
  DONE,
  CONTRADICTION,
  SAT,
  INDETERMINATE
};

// =============================================================================
// Solver
//
// Control the main flow of the algorithms to solve a CNF
// =============================================================================
class Solver {
 public:
  // Random number generator
  random_device rd;
  unsigned long initialSeed;
  mt19937 randomGenerator;
  uniform_int_distribution<> randomBoolUD;
  uniform_real_distribution<> randomReal01UD;

  // Factor Graph
  FactorGraph* fg;
  int N;
  float alpha;

  // Algorithm parameters
  float sidFraction;
  double paramagneticState = 0.01;

  int spMaxIt = 1000;
  double spEpsilon = 0.001f;

  int wsMaxTries = 100;
  int wsMaxFlips = 100;
  int wsNoise = 0.5f;

  // Metrics
  int lastSPIterations = 0;

 private:
  double maxConvergeDiff = 0.0;

 public:
  // inline void setSeed(int seed) { _randomGenerator.seed(seed); }
  inline bool getRandomBool() { return randomBoolUD(randomGenerator); }
  inline double getRandomReal01() { return randomReal01UD(randomGenerator); }

  explicit Solver(int N, float a, int seed);

  AlgorithmResult SID(FactorGraph* graph, float fraction);

 private:
  AlgorithmResult SP();
  void evaluateVar(Variable* var);
  bool assignVariable(Variable* var, bool value);
  bool cleanGraph(Variable* var);
  bool unitPropagation(Clause* clause);
};
}  // namespace sat