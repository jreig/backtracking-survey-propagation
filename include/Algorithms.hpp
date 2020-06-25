#pragma once

#include <FactorGraph.hpp>
#include <chrono>

namespace sat {

// -----------------------------------------------------------------------------
// Survey Propagation
// -----------------------------------------------------------------------------
struct SPResult {
  bool converged;
  unsigned int iterations;
};
SPResult SurveyPropagation(FactorGraph* graph);

void UpdateSurvey(Edge* edge);

// -----------------------------------------------------------------------------
// Unit Propagation
// -----------------------------------------------------------------------------
bool UnitPropagation(FactorGraph* graph);

// -----------------------------------------------------------------------------
// Walksat
// -----------------------------------------------------------------------------
bool Walksat(FactorGraph* graph);

// -----------------------------------------------------------------------------
// Survey Inspired Decimation
// -----------------------------------------------------------------------------
struct SIDResult {
  bool converged;
  bool SAT;
  unsigned int totalSPIterations;
  std::chrono::steady_clock::time_point begin;
  std::chrono::steady_clock::time_point end;
};
SIDResult SID(FactorGraph* graph, float fraction);

void EvaluateVariable(Variable* variable);

}  // namespace sat