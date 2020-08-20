#pragma once

#include <FactorGraph.hpp>
#include <random>

using namespace std;

namespace sat {

// This constant is to ensure correct comparsion of doubles when checking
// if a number is 0. All numbers below 1.0e-16 are considered 0.
#define ZERO_EPSILON (1.0e-16)

enum AlgorithmResult {
  CONVERGE,
  UNCONVERGE,
  DONE,
  CONTRADICTION,
  SAT,
  INDETERMINATE,
  WALKSAT  // TODO remove when walksat is implemented
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
  double alpha;

  // Algorithm parameters
  double sidFraction;
  double paramagneticState = 0.01;

  int spMaxIt = 1000;
  double spEpsilon = 0.001;

  int wsMaxTries = 10;
  int wsMaxFlips = 100;
  double wsNoise = 0.57;

  // Metrics
  int totalSPIterations = 0;
  int totalSIDIterations = 0;

 public:
  // inline void setSeed(int seed) { _randomGenerator.seed(seed); }
  inline bool getRandomBool() { return randomBoolUD(randomGenerator); }
  inline double getRandomReal01() { return randomReal01UD(randomGenerator); }

  explicit Solver(int N, double a, int seed);

  AlgorithmResult SID(FactorGraph* graph, double fraction);

 private:
  AlgorithmResult walksat();
  AlgorithmResult surveyPropagation();
  double updateSurveys(Clause* clause);
  void computeSubProducts();
  void evaluateVar(Variable* var);
  bool assignVariable(Variable* var, bool value);
  bool cleanGraph(Variable* var);
  bool unitPropagation(Clause* clause);
};
}  // namespace sat