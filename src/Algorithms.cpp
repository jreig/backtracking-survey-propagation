#include <algorithm>
#include <cmath>
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

  std::cout << "Initial random surveys:" << std::endl;
  for (Edge* edge : edges) {
    edge->survey = utils::getRandomReal01();
    std::cout << edge << std::endl;
  }

  // 4 - Repeat until all surveys converge or max iterations are reached
  bool allEdgesConverged = false;
  for (uint i = 0; i < params.maxIterations && !allEdgesConverged; i++) {
    std::cout << std::endl;
    std::cout << "Iteration: " << i << std::endl;
    // 2 - Order randomly the set of edges
    std::shuffle(edges.begin(), edges.end(), utils::randomGenerator);

    // 3 - Update the survey value of each edge
    allEdgesConverged = true;
    for (Edge* edge : edges) {
      long double previousSurveyValue = edge->survey;
      UpdateSurvey(edge);

      // Check if edge converged
      std::cout << edge << std::endl;

      bool hasConverged =
          std::abs(edge->survey - previousSurveyValue) < params.epsilon;
      if (!hasConverged) allEdgesConverged = false;
    }

    // Update metrics
    utils::currentSPIterations++;
  }

  std::cout << std::endl;
  for (Edge* edge : edges) {
    std::cout << edge << std::endl;
  }
  return allEdgesConverged;
}

void UpdateSurvey(Edge* ai) {
  // Param edge is a->i
  long double Sai = 1.0;

  // For each a->j when j != i
  for (Edge* aj : ai->clause->GetEnabledEdges()) {
    if (aj == ai) continue;  // j == i

    // Product values initalization for all b->j survey values
    long double Pubj = 1.0;
    long double Psbj = 1.0;
    long double P0bj = 1.0;

    // For each b->j when b != a
    for (Edge* bj : aj->variable->GetEnabledEdges()) {
      if (bj == aj) continue;  // b == a

      if (bj->type != aj->type) {
        // Update Pubj if both edges have different edge type -> b€Vua(j)
        Pubj = Pubj * (1.0 - bj->survey);
      }
      if (bj->type == aj->type) {
        // Update Psbj if both edges have same edge type -> b€Vsa(j)
        Psbj = Psbj * (1.0 - bj->survey);
      }

      P0bj = P0bj * (1.0 - bj->survey);
    }

    // Product values for all a->j survey values (Equation 26)
    long double Puaj = (1.0 - Pubj) * Psbj;
    long double Psaj = (1.0 - Psbj) * Pubj;
    long double P0aj = P0bj;

    // Update a->i survey value (Equation 27)
    long double aux1 = (Puaj + Psaj + P0aj);
    long double aux2 = Puaj / aux1;
    Sai = Sai * aux2;

    if (Sai == 1.0 || std::isnan(Sai)) {
      std::cout << "=================================" << std::endl;
      if (Sai == 1.0) std::cout << "Survey is 1.0 exactly" << std::endl;
      if (std::isnan(Sai)) std::cout << "Survey is NaN" << std::endl;
      std::cout << "Edge a->i: " << ai << std::endl;
      std::cout << "Edge a->j: " << aj << std::endl;
      std::cout << "Pubj = " << Pubj << std::endl;
      std::cout << "Psbj = " << Psbj << std::endl;
      std::cout << "P0bj = " << P0bj << std::endl;
      std::cout << "Puaj = " << Puaj << std::endl;
      std::cout << "Psaj = " << Psaj << std::endl;
      std::cout << "P0aj = " << P0aj << std::endl;
      std::cout << "Sai = " << Sai << std::endl;
      std::cout << "===================================" << std::endl;
    }
  }

  // update a->i survey
  ai->survey = Sai;
}

// -----------------------------------------------------------------------------
// Unit Propagation
// -----------------------------------------------------------------------------
bool UnitPropagation(FactorGraph* graph, AssignmentStep* assignment) {
  // Run until contradiction is found or no unit clauses are found
  while (true) {
    // 1. Found all enabled Clauses with only one enabled Edge
    std::vector<Clause*> unitClauses;
    for (Clause* clause : graph->GetEnabledClauses()) {
      if (clause->GetEnabledEdges().size() == 1) unitClauses.push_back(clause);
    }

    // Return true if no unitary Clauses are found.
    if (unitClauses.size() == 0) return true;

    // Assign the Variable of the unit clause to true if Edge is POSITIVE
    // and to false if NEGATIVE.
    for (Clause* unitClause : unitClauses) {
      Edge* edge = unitClause->GetEnabledEdges()[0];
      if (!edge->variable->assigned) {
        edge->variable->AssignValue(edge->type, assignment);
        std::cout << "UP assign: " << edge->variable->id << " - " << edge->type
                  << std::endl;
      }
      // If the variable is already assigned with a value distinct from the edge
      // type, return false (contradiction found)
      else {
        if (edge->type != edge->variable->value) return false;
      }
    }

    // 2. For each Clause in the graph:
    for (Clause* clause : graph->GetEnabledClauses()) {
      for (Edge* edge : clause->GetEnabledEdges()) {
        if (edge->variable->assigned) {
          // 2.1 Disable the clause if is satisfied by the assignment
          // (contains the assigned literal)
          if (edge->type == edge->variable->value) {
            clause->Dissable(assignment);
            break;
          }

          // 2.2 Disable each Edge of the clause that contain an assigned
          // Variable with the oposite literal type.
          else {
            edge->Dissable(assignment);
          }
        }
      }

      // If the Clause is enabled and have 0 enabled Edges,
      // return false (contradiction found).
      if (clause->enabled && clause->GetEnabledEdges().size() == 0)
        return false;
    }

    std::cout << "UP:" << graph << std::endl;
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

      // 1.2.2 Randomly select an unsatisfied clause and calculate the
      // break-count of its variables

      // Separate clauses into sat and unsat
      std::vector<Clause*> satClauses;
      std::vector<Clause*> unsatClauses;
      for (Clause* clause : graph->GetEnabledClauses()) {
        if (clause->IsSAT())
          satClauses.push_back(clause);
        else
          unsatClauses.push_back(clause);
      }

      // Select random unsat clause
      std::uniform_int_distribution<> randomInt(0, unsatClauses.size() - 1);
      int randIndex = randomInt(utils::randomGenerator);
      Clause* selectedClause = unsatClauses[randIndex];
      std::vector<Edge*> selectedClauseEdges =
          selectedClause->GetEnabledEdges();

      // Calculate break-count (number of currently satisfied clauses
      // that become unsatisfied if the variable value is fliped) of variables
      // in selected clause
      Variable* lowestBreakCountVar = nullptr;
      uint lowestBreakCount = satClauses.size() + 1;
      for (Edge* edge : selectedClauseEdges) {
        uint breakCount = 0;
        // Flip variable and count
        edge->variable->AssignValue(!edge->variable->value);
        for (Clause* satClause : satClauses) {
          if (!satClause->IsSAT()) breakCount++;
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

      // 1.2.3 Flip a Variable of the Clause if has break-count = 0
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
              0, selectedClauseEdges.size() - 1);
          int randomEdgeIndex = randEdgeIndexDist(utils::randomGenerator);
          Variable* var = selectedClauseEdges[randomEdgeIndex]->variable;
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

  while (true) {
    // 1. Run UNIT PROPAGTION
    bool UPResult = UnitPropagation(graph);
    // If a contradiction in found, return false
    if (!UPResult) {
      std::cout << "UP found a contradiction" << std::endl;
      return false;
    }
    // If SAT, return true.
    if (graph->IsSAT()) {
      utils::totalSPIterations += utils::currentSPIterations;
      std::cout << "IsSAT()" << std::endl;
      return true;
    }

    // 2. Run SP. If does not converge return false.
    bool SPResult = SurveyPropagation(graph, paramsSP);
    if (!SPResult) {
      std::cout << "SP don't converge" << std::endl;
      return false;
    }

    // 3. Decimate
    // 3.1 If all surveys are trivial, return WALKSAT result
    bool allTrivial = true;
    for (Edge* edge : graph->GetEnabledEdges()) {
      if (edge->survey != 0.0) {
        allTrivial = false;
        break;
      }
    }
    if (allTrivial) {
      bool walksatResult = Walksat(graph, paramsWalksat);
      if (!walksatResult)
        std::cout << "Walksat can't found a solution" << std::endl;
      else {
        utils::totalSPIterations += utils::currentSPIterations;
        std::cout << "Solved with Walksat" << std::endl;
      }
      return walksatResult;
    }

    // 2.2 Otherwise, evaluate all variables, assign a set of them and clean
    // the graph
    std::vector<Variable*> unassignedVariables =
        graph->GetUnassignedVariables();
    std::cout << std::endl;
    for (Variable* variable : unassignedVariables) {
      EvaluateVariable(variable);
      std::cout << variable->id << "(" << variable->evalValue << ")"
                << std::endl;
    }

    // Assign minimum 1 variable
    int assignFraction = (int)(unassignedVariables.size() * fraction);
    if (assignFraction < 1) assignFraction = 1;
    std::sort(unassignedVariables.begin(), unassignedVariables.end(),
              [](const Variable* lvar, const Variable* rvar) {
                return std::abs(lvar->evalValue) > std::abs(rvar->evalValue);
              });

    for (int i = 0; i < assignFraction; i++) {
      Variable* var = unassignedVariables[i];
      bool newValue = var->evalValue > 0;
      std::cout << "Assigned: X" << var->id << " - "
                << (newValue ? "true" : "false") << std::endl;
      var->AssignValue(newValue);
      for (Edge* edge : var->GetEnabledEdges()) {
        if (edge->type == var->value) {
          edge->clause->Dissable();
        } else {
          edge->Dissable();
        }
      }
    }

    std::cout << graph << std::endl;
  };
}

void EvaluateVariable(Variable* variable) {
  // Vi  = V(i)  -> Subset of clauses where the variable i appears
  // ViP = V+(i) -> substed of V(i) where i appears unnegated
  // ViN = V-(i) -> substed of V(i) where i appears negated
  // Product values initialization for all a->i survey values
  long double PVi0 = 1.0;
  long double PViP = 1.0;
  long double PViN = 1.0;

  // For each a->i
  for (Edge* ai : variable->GetEnabledEdges()) {
    if (ai->type) {
      // Update PViP if variable i appears unnegated in clause a
      PViP = PViP * (1.0 - ai->survey);
    } else {
      // Update PViN if variable i appears negated in clause a
      PViN = PViN * (1.0 - ai->survey);
    }

    PVi0 = PVi0 * (1.0 - ai->survey);
  }

  // Auxiliar variables to calculate Wi(+) and Wi(-)
  long double PiP = (1.0 - PViP) * PViN;
  long double PiN = (1.0 - PViN) * PViP;
  long double Pi0 = PVi0;

  // Calculate 'biases'
  long double WiP = PiP / (PiP + PiN + Pi0);  // Wi(+)
  long double WiN = PiN / (PiP + PiN + Pi0);  // Wi(-)

  variable->evalValue = WiP - WiN;
}

}  // namespace sat