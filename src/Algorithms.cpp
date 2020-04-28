#include <algorithm>
#include <vector>

// Project headers
#include <Algorithms.hpp>
#include <Utils.hpp>

namespace sat {

// -----------------------------------------------------------------------------
// Survey Propagation
// -----------------------------------------------------------------------------
bool SurveyPropagation(FactorGraph* graph, ParamsSP params) {
  // TODO Check that the parameters have correct values

  // 1 - Random initialization of survey values
  std::vector<Edge*> edges = graph->GetEnabledEdges();

  for (Edge* edge : edges) {
    edge->survey = utils::getRandomReal01();
  }

  // 4 - Repeat until all surveys converge or max iterations are reached
  bool allEdgesConverged = false;
  for (uint i = 0; i < params.maxIterations && !allEdgesConverged; i++) {
    // 2 - Order randomly the set of edges
    std::shuffle(edges.begin(), edges.end(), utils::randomGenerator);

    // 3 - Update the survey value of each edge
    allEdgesConverged = true;
    for (Edge* edge : edges) {
      float previousSurveyValue = edge->survey;
      UpdateSurvey(edge);

      // Check if edge converged
      bool hasConverged =
          std::abs(edge->survey - previousSurveyValue) < params.epsilon;
      if (!hasConverged) allEdgesConverged = false;
    }

    // Update metrics
    utils::currentSPIterations++;
  }

  return allEdgesConverged;
}

void UpdateSurvey(Edge* ai) {
  // Param edge is a->i
  float Sai = 1.0f;

  // For each a->j when j != i
  for (Edge* aj : ai->clausule->GetEnabledEdges()) {
    if (aj == ai) continue;  // j == i

    // Product values initalization for all b->j survey values
    float Pubj = 1.0f;
    float Psbj = 1.0f;
    float P0bj = 1.0f;

    // For each b->j when b != a
    for (Edge* bj : aj->variable->GetEnabledEdges()) {
      if (bj == aj) continue;  // b == a

      if (bj->type != aj->type) {
        // Update Pubj if both edges have different edge type -> b€Vua(j)
        Pubj = Pubj * (1 - bj->survey);
      }
      if (bj->type == aj->type) {
        // Update Psbj if both edges have same edge type -> b€Vsa(j)
        Psbj = Psbj * (1 - bj->survey);
      }

      P0bj = P0bj * (1 - bj->survey);
    }

    // Product values for all a->j survey values (Equation 26)
    float Puaj = (1.0f - Pubj) * Psbj;
    float Psaj = (1.0f - Psbj) * Pubj;
    float P0aj = P0bj;

    // Update a->i survey value (Equation 27)
    Sai = Sai * (Puaj / (Puaj + Psaj + P0aj));
  }

  // update a->i survey
  ai->survey = Sai;
}

// -----------------------------------------------------------------------------
// Unit Propagation
// -----------------------------------------------------------------------------
bool UnitPropagation(FactorGraph* graph, AssignmentStep* assignment) {
  // Run until contradiction is found or no unit clausules are found
  while (true) {
    // 1. Found all enabled Clausules with only one enabled Edge
    std::vector<Clausule*> unitClausules;
    for (Clausule* clausule : graph->GetEnabledClausules()) {
      if (clausule->GetEnabledEdges().size() == 1)
        unitClausules.push_back(clausule);
    }

    // Return true if no unitary Clausules are found.
    if (unitClausules.size() == 0) return true;

    // Assign the Variable of the unit clausule to true if Edge is POSITIVE
    // and to false if NEGATIVE.
    for (Clausule* unitClausule : unitClausules) {
      Edge* edge = unitClausule->GetEnabledEdges()[0];
      if (!edge->variable->assigned) {
        edge->variable->AssignValue(edge->type, assignment);
      }
      // If the variable is already assigned with a value distinct from the edge
      // type, return false (contradiction found)
      else {
        if (edge->type != edge->variable->value) return false;
      }
    }

    // 2. For each Clausule in the graph:
    for (Clausule* clausule : graph->GetEnabledClausules()) {
      for (Edge* edge : clausule->GetEnabledEdges()) {
        if (edge->variable->assigned) {
          // 2.1 Disable the clausule if is satisfied by the assignment
          // (contains the assigned literal)
          if (edge->type == edge->variable->value) {
            clausule->Dissable(assignment);
            break;
          }

          // 2.2 Disable each Edge of the clausule that contain an assigned
          // Variable with the oposite literal type.
          else {
            edge->Dissable(assignment);
          }
        }
      }

      // If the Clausule is enabled and have 0 enabled Edges,
      // return false (contradiction found).
      if (clausule->enabled && clausule->GetEnabledEdges().size() == 0)
        return false;
    }
  }
}

// -----------------------------------------------------------------------------
// Walksat
// -----------------------------------------------------------------------------
bool Walksat(FactorGraph* graph, ParamsWalksat params,
             AssignmentStep* assignment) {
  // TODO Check that the parameters have correct values

  // 1. For try t = 0 to maxTries
  for (uint t = 0; t < params.maxTries; t++) {
    // 1.1 Assign all Varibles with random values
    for (Variable* var : graph->GetUnassignedVariables()) {
      var->AssignValue(utils::getRandomBool(), assignment);
    }

    // 1.2 For flip f = 0 to maxFlips:
    for (uint f = 0; f < params.maxFlips; f++) {
      // 1.2.1 If FactorGraph is satisfied, return true
      if (graph->IsSAT()) return true;

      // 1.2.2 Randomly select an unsatisfied clausule and calculate the
      // break-count of its variables

      // Separate clausules into sat and unsat
      std::vector<Clausule*> satClausules;
      std::vector<Clausule*> unsatClausules;
      for (Clausule* clausule : graph->GetEnabledClausules()) {
        if (clausule->IsSAT())
          satClausules.push_back(clausule);
        else
          unsatClausules.push_back(clausule);
      }

      // Select random unsat clausule
      std::uniform_int_distribution<> randomInt(0, unsatClausules.size() - 1);
      int randIndex = randomInt(utils::randomGenerator);
      Clausule* selectedClausule = unsatClausules[randIndex];
      std::vector<Edge*> selectedClausuleEdges =
          selectedClausule->GetEnabledEdges();

      // Calculate break-count (number of currently satisfied clauses
      // that become unsatisfied if the variable value is fliped) of variables
      // in selected clausule
      Variable* lowestBreakCountVar = nullptr;
      uint lowestBreakCount = satClausules.size() + 1;
      for (Edge* edge : selectedClausuleEdges) {
        uint breakCount = 0;
        // Flip variable and count
        edge->variable->AssignValue(!edge->variable->value);
        for (Clausule* satClausule : satClausules) {
          if (!satClausule->IsSAT()) breakCount++;
        }
        // flip again to stay as it was
        edge->variable->AssignValue(!edge->variable->value);

        // Update lowest break-count
        if (lowestBreakCountVar == nullptr || breakCount < lowestBreakCount) {
          lowestBreakCountVar = edge->variable;
          lowestBreakCount = breakCount;
        }

        // If break-count = 0 no need to count more
        if (breakCount == 0) break;
      }

      // 1.2.3 Flip a Variable of the Clausule if has break-count = 0
      // If not, with probability p (noise), flip a random variable and
      // with probability 1 - p, flip the variable with lowest break-count
      if (lowestBreakCount == 0) {
        lowestBreakCountVar->AssignValue(!lowestBreakCountVar->value);
      } else {
        // probability 1 - p
        if (utils::getRandomReal01() > params.noise) {
          lowestBreakCountVar->AssignValue(!lowestBreakCountVar->value);
        }
        // probability p
        else {
          std::uniform_int_distribution<> randEdgeIndexDist(
              0, selectedClausuleEdges.size() - 1);
          int randomEdgeIndex = randEdgeIndexDist(utils::randomGenerator);
          Variable* var = selectedClausuleEdges[randomEdgeIndex]->variable;
          var->AssignValue(!var->value);
        }
      }
    }
  }

  // 2. If a sat assignment was not found, return false.
  return false;
}

// -----------------------------------------------------------------------------
// Survey Inspired Decimation
// -----------------------------------------------------------------------------
bool SID(FactorGraph* graph, float fraction, ParamsSP paramsSP,
         ParamsWalksat paramsWalksat) {
  // Start metrics
  utils::currentSPIterations = 0;

  while (!graph->IsSAT()) {
    // 1. Run UNIT PROPAGTION
    bool UPResult = UnitPropagation(graph);
    // If a contradiction in found, return false
    if (!UPResult) return false;
    // If SAT, return true.
    if (graph->IsSAT()) {
      utils::totalSPIterations += utils::currentSPIterations;
      return true;
    }

    // 2. Run SP. If does not converge return false.
    bool SPResult = SurveyPropagation(graph, paramsSP);
    if (!SPResult) return false;

    // 3. Decimate
    // 3.1 If all surveys are trivial, return WALKSAT result
    bool allTrivial = true;
    for (Edge* edge : graph->GetEnabledEdges()) {
      if (edge->survey != 0.0f) {
        allTrivial = false;
        break;
      }
    }
    if (allTrivial) return Walksat(graph, paramsWalksat);

    // 2.2 Otherwise, evaluate all variables, assign a set of them and clean the
    // graph
    std::vector<Variable*> unassignedVariables =
        graph->GetUnassignedVariables();
    for (Variable* variable : unassignedVariables) {
      EvaluateVariable(variable);
    }

    int assignFraction = (int)(unassignedVariables.size() * fraction) + 1;
    std::sort(unassignedVariables.begin(), unassignedVariables.end(),
              [](const Variable* lvar, const Variable* rvar) {
                return std::abs(lvar->evalValue) > std::abs(rvar->evalValue);
              });

    for (int i = 0; i < assignFraction; i++) {
      Variable* var = unassignedVariables[i];
      bool newValue = var->evalValue > 0;
      var->AssignValue(newValue);
      for (Edge* edge : var->GetEnabledEdges()) {
        if (edge->type == var->value) {
          edge->clausule->Dissable();
        } else {
          edge->Dissable();
        }
      }
    }

    std::cout << graph << std::endl;
  };

  utils::totalSPIterations += utils::currentSPIterations;
  return true;
}

void EvaluateVariable(Variable* variable) {
  // Vi  = V(i)  -> Subset of clausules where the variable i appears
  // ViP = V+(i) -> substed of V(i) where i appears unnegated
  // ViN = V-(i) -> substed of V(i) where i appears negated
  // Product values initialization for all a->i survey values
  float PVi0 = 1.0f;
  float PViP = 1.0f;
  float PViN = 1.0f;

  // For each a->i
  for (Edge* ai : variable->GetEnabledEdges()) {
    if (ai->type) {
      // Update PViP if variable i appears unnegated in clausule a
      PViP = PViP * (1 - ai->survey);
    } else {
      // Update PViN if variable i appears negated in clausule a
      PViN = PViN * (1 - ai->survey);
    }

    PVi0 = PVi0 * (1 - ai->survey);
  }

  // Auxiliar variables to calculate Wi(+) and Wi(-)
  float PiP = (1.0f - PViP) * PViN;
  float PiN = (1.0f - PViN) * PViP;
  float Pi0 = PVi0;

  // Calculate 'biases'
  float WiP = PiP / (PiP + PiN + Pi0);  // Wi(+)
  float WiN = PiN / (PiP + PiN + Pi0);  // Wi(-)

  variable->evalValue = WiP - WiN;
}

}  // namespace sat