#pragma once

#include <FactorGraph.hpp>

namespace sat {

// -----------------------------------------------------------------------------
// Survey Propagation
// -----------------------------------------------------------------------------
struct ParamsSP {
  uint maxIterations = 1000;
  float epsilon = 0.001f;
};
bool SurveyPropagation(FactorGraph* graph, ParamsSP params);

void UpdateSurvey(Edge* edge);

// -----------------------------------------------------------------------------
// Unit Propagation
// -----------------------------------------------------------------------------
bool UnitPropagation(FactorGraph* graph, AssignmentStep* assignment = nullptr);

// -----------------------------------------------------------------------------
// Walksat
// -----------------------------------------------------------------------------
struct ParamsWalksat {
  uint maxTries = 100;
  uint maxFlips = 100;
  float noise = 0.54f;
};
bool Walksat(FactorGraph* graph, ParamsWalksat params,
             AssignmentStep* assignment = nullptr);

// -----------------------------------------------------------------------------
// Survey Inspired Decimation
// -----------------------------------------------------------------------------
bool SID(FactorGraph* graph, float fraction, ParamsSP paramsSP,
         ParamsWalksat paramsWalksat);

void EvaluateVariable(Variable* variable);

}  // namespace sat