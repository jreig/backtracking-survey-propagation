#include <Solver.hpp>
#include <algorithm>

namespace sat {

// =============================================================================
// Solver
// =============================================================================
Solver::Solver(int N, double a, int seed)
    : initialSeed(seed),
      randomBoolUD(0, 1),
      randomReal01UD(0, 1),
      N(N),
      alpha(a),
      wsMaxFlips(100 * N) {
  // Random number generator initialization
  if (seed == 0) initialSeed = rd();
  randomGenerator.seed(initialSeed);
}

// =============================================================================
// Algorithms
// =============================================================================
AlgorithmResult Solver::SID(FactorGraph* graph, double fraction) {
  fg = graph;
  sidFraction = fraction;

  // --------------------------------
  // Random initialization of surveys
  // --------------------------------
  for (Edge* edge : fg->edges) {
    edge->survey = getRandomReal01();
  }

  // Run until sat, sp unconverge or wlaksat result
  while (true) {
    // ----------------------------
    // Run SP
    // If trivial state is reach, walksat is called and the result returned
    // ----------------------------
    AlgorithmResult spResult = surveyPropagation();
    if (spResult == WALKSAT) cout << fg << endl;
    if (spResult != CONVERGE) return spResult;

    // --------------------------------
    // Build variable list and order it
    // --------------------------------
    vector<Variable*> unassignedVariables;

    // Evaluate and store the sum of the max bias of all unassigned variables
    double sumMaxBias = 0.0;
    for (Variable* var : fg->variables) {
      if (!var->assigned) {
        evaluateVar(var);
        sumMaxBias += var->Hp > var->Hm ? var->Hp : var->Hm;
        unassignedVariables.push_back(var);
      }
    }

    // Check paramagnetic state
    // TODO: Entender que significa esto, en el codigo original, este es
    // el unico sitio donde se llama a walksat
    // if (sumMaxBias / unassignedVariables.size() < paramagneticState) {
    //   cout << "Paramagnetic state reached" << endl;
    //   cout << fg << endl;
    //   return walksat();
    // }

    // Assign minimum 1 variable
    int assignFraction = (int)(unassignedVariables.size() * fraction);
    if (assignFraction < 1) assignFraction = 1;
    sort(unassignedVariables.begin(), unassignedVariables.end(),
         [](const Variable* lvar, const Variable* rvar) {
           return abs(lvar->evalValue) > abs(rvar->evalValue);
         });

    // ------------------------
    // Fix the set of variables
    // ------------------------
    for (int i = 0; i < assignFraction; i++) {
      // Variables in the list can be already assigned due to UP being executed
      // in previous iterations
      if (unassignedVariables[i]->assigned) {
        assignFraction++;  // Don't count this variable as a new assignation
        continue;
      }

      // Found the new value and assign the variable
      // The assignation method cleans the graph and execute UP if one of
      // the cleaned clause become unitary
      Variable* var = unassignedVariables[i];

      // Recalculate biases for same reason, previous assignations clean the
      // graph and change relations
      evaluateVar(var);
      bool newValue = var->Hp > var->Hm ? true : false;

      if (!assignVariable(var, newValue)) {
        // Error found when assigning variable
        return CONTRADICTION;
      }
    }

    // ----------------------------
    // If SAT finish algorithm
    // ----------------------------
    if (fg->IsSAT()) {
      return SAT;
    }
  }
}

AlgorithmResult Solver::surveyPropagation() {
  // Calculate subproducts of all variables
  computeSubProducts();

  for (int i = 0; i < spMaxIt; i++) {
    // Randomize clause iteration
    vector<Clause*> enabledClauses = fg->GetEnabledClauses();
    shuffle(enabledClauses.begin(), enabledClauses.end(), randomGenerator);

    // Calculate surveys
    double maxConvergeDiff = 0.0;
    for (Clause* clause : enabledClauses) {
      double maxConvDiffInClause = updateSurveys(clause);

      // Save max convergence diff
      if (maxConvDiffInClause > maxConvergeDiff)
        maxConvergeDiff = maxConvDiffInClause;
    }

    // Check if converged
    if (maxConvergeDiff <= spEpsilon) {
      // If max difference of convergence is 0, all are 0
      // which is a trivial state and walksat must be called
      if (maxConvergeDiff < ZERO_EPSILON) {
        cout << "Trivial state reached" << endl;
        cout << fg << endl;
        return walksat();
      }

      // If not triavial return and continue algorith
      return CONVERGE;
    }
  }

  // Max itertions reach without convergence
  return UNCONVERGE;
}

void Solver::computeSubProducts() {
  for (Variable* var : fg->variables) {
    if (!var->assigned) {
      var->p = 1.0;
      var->m = 1.0;
      var->pzero = 0;
      var->mzero = 0;

      // For each edge connecting the variable to a clause
      for (Edge* edge : var->allNeighbourEdges) {
        if (edge->enabled) {
          // If edge is positive update positive subproduct of variable
          if (edge->type) {
            // If edge survey != 1
            if (1.0 - edge->survey > ZERO_EPSILON) {
              var->p *= 1.0 - edge->survey;
            }
            // If edge survey == 1
            else
              var->pzero++;
          }
          // If edge is negative, update negative subproduct of variable
          else {
            // If edge survey != 1
            if (1.0 - edge->survey > ZERO_EPSILON) {
              var->m *= 1.0 - edge->survey;
            }
            // If edge survey == 1
            else
              var->mzero++;
          }
        }
      }
    }
  }
}

double Solver::updateSurveys(Clause* clause) {
  double maxConvDiffInClause = 0.0;
  int zeros = 0;
  double allSubSurveys = 1.0;
  vector<double> subSurveys;

  // ==================================================================
  // Calculate subProducts of all literals and keep track of wich are 0
  // ==================================================================
  for (Edge* edge : clause->allNeighbourEdges) {
    if (edge->enabled) {
      Variable* var = edge->variable;
      double m, p, wn, wt;

      // If edge is positive:
      if (edge->type) {
        m = var->mzero ? 0 : var->m;
        if (var->pzero == 0)
          p = var->p / (1.0 - edge->survey);
        else if (var->pzero == 1 && (1.0 - edge->survey) < ZERO_EPSILON)
          p = var->p;
        else
          p = 0.0;

        wn = p * (1.0 - m);
        wt = m;
      }
      // If edge is negative
      else {
        p = var->pzero ? 0 : var->p;
        if (var->mzero == 0)
          m = var->m / (1.0 - edge->survey);
        else if (var->mzero == 1 && (1.0 - edge->survey) < ZERO_EPSILON)
          m = var->m;
        else
          m = 0.0;

        wn = m * (1 - p);
        wt = m;
      }

      // Calculate subSurvey
      double subSurvey = wn / (wn + wt);
      subSurveys.push_back(subSurvey);

      // If subsurvey is 0 keep track but don't multiply
      if (subSurvey < ZERO_EPSILON)
        zeros++;
      else
        allSubSurveys *= subSurvey;
    }
  }

  // =========================================================
  // Calculate the survey for each edge with the previous data
  // =========================================================
  int i = 0;
  for (Edge* edge : clause->allNeighbourEdges) {
    if (edge->enabled) {
      // ---------------------------------------------
      // Calculate new survey from sub survey products
      // ---------------------------------------------
      double newSurvey;
      // If there where no subSurveys == 0, proceed normaly
      if (!zeros) newSurvey = allSubSurveys / subSurveys[i];
      // If this subsurvey is the only one that is 0
      // consider the new survey as the total subSurveys
      else if (zeros == 1 && subSurveys[i] < ZERO_EPSILON)
        newSurvey = allSubSurveys;
      // If there where more that one subSurveys == 0, the new survey is 0
      else
        newSurvey = 0.0;

      // ----------------------------------------------------
      // Update the variable subproducts with new survey info
      // ----------------------------------------------------
      Variable* var = edge->variable;
      // If edge is positive update positive subproduct
      if (edge->type) {
        // If previous survey != 1 (with an epsilon margin)
        if (1.0 - edge->survey > ZERO_EPSILON) {
          // If new survey != 1, update the sub product with the difference
          if (1.0 - newSurvey > ZERO_EPSILON)
            var->p *= ((1.0 - newSurvey) / (1.0 - edge->survey));
          // If new survey == 1, update the subproduct by remove the old survey
          // and keep track of the new survey == 1 (pzero++)
          else {
            var->p /= (1.0 - edge->survey);
            var->pzero++;
          }
        }
        // If previous survey == 1
        else {
          // If new survey == 1, don't do anything (both surveys are the same)
          // If new survey != 1, update subproduct
          if (1.0 - newSurvey > ZERO_EPSILON) {
            var->p *= (1.0 - newSurvey);
            var->pzero--;
          }
        }
      }
      // If edge is negative, update negative subproduct
      else {
        // If previous survey != 1 (with an epsilon margin)
        if (1.0 - edge->survey > ZERO_EPSILON) {
          // If new survey != 1, update the sub product with the difference
          if (1.0 - newSurvey > ZERO_EPSILON)
            var->m *= ((1.0 - newSurvey) / (1.0 - edge->survey));
          // If new survey == 1, update the subproduct by remove the old survey
          // and keep track of the new survey == 1 (pzero++)
          else {
            var->m /= (1.0 - edge->survey);
            var->mzero++;
          }
        }
        // If previous survey == 1
        else {
          // If new survey == 1, don't do anything (both surveys are the same)
          // If new survey != 1, update subproduct
          if (1.0 - newSurvey > ZERO_EPSILON) {
            var->m *= (1.0 - newSurvey);
            var->mzero--;
          }
        }
      }

      // ----------------------------------------------------
      // Store new survey and update max clause converge diff
      // ----------------------------------------------------
      double edgeConvDiff = abs(edge->survey - newSurvey);
      if (edgeConvDiff > maxConvDiffInClause)
        maxConvDiffInClause = edgeConvDiff;

      edge->survey = newSurvey;
      i++;
    }
  }

  return maxConvDiffInClause;
}

bool Solver::assignVariable(Variable* var, bool value) {
  // Contradiction if variable was already assigned with different value
  if (var->assigned && var->value != value) {
    cout << "ERROR: Variable X" << var->id
         << " already assigned with opposite value" << endl;
    return false;
  }

  var->AssignValue(value);
  return cleanGraph(var);
}

bool Solver::cleanGraph(Variable* var) {
  for (Edge* edge : var->allNeighbourEdges) {
    if (edge->enabled) {
      if (edge->type == var->value) {
        edge->clause->Dissable();
      } else {
        edge->Dissable();

        // Execute UP for this clause because can become unitary or empty
        if (!unitPropagation(edge->clause)) return false;
      }
    }
  }

  return true;
}

bool Solver::unitPropagation(Clause* clause) {
  vector<Edge*> enabledEdges = clause->GetEnabledEdges();
  int size = enabledEdges.size();

  // Contradiction if empty clause
  if (size == 0) {
    cout << "ERROR: Clause C" << clause->id << " is empty" << endl;
    return false;
  }

  // Unitary clause
  if (size == 1) {
    // Fix the variable to the edge type. This will execute UP with recursivity
    Edge* edge = enabledEdges[0];  // Unique enabled edge in unitary clause
    return assignVariable(edge->variable, edge->type);
  }

  // Finish unit propagation if clause is not unitary
  return true;
}

void Solver::evaluateVar(Variable* var) {
  double p = var->pzero ? 0 : var->p;
  double m = var->mzero ? 0 : var->m;

  var->Hz = p * m;
  var->Hp = m - var->Hz;
  var->Hm = p - var->Hz;

  // Normalize
  double sum = var->Hm + var->Hz + var->Hp;
  var->Hz /= sum;
  var->Hp /= sum;
  var->Hm /= sum;

  // Store eval value
  var->evalValue = abs(var->Hp - var->Hm);
}

AlgorithmResult Solver::walksat() {
  // 1. For try t = 0 to maxTries
  for (int t = 0; t < wsMaxTries; t++) {
    // 1.1 Assign all Varibles with random values
    for (Variable* var : fg->GetUnassignedVariables()) {
      var->AssignValue(getRandomBool());
    }

    // 1.2 For flip f = 0 to maxFlips:
    for (int f = 0; f < wsMaxFlips; f++) {
      // 1.2.1 If FactorGraph is satisfied, return true
      if (fg->IsSAT()) return SAT;

      // 1.2.2 Randomly select an unsatisfied clause and calculate the
      // break-count of its variables

      // Separate clauses into sat and unsat
      std::vector<Clause*> satClauses;
      std::vector<Clause*> unsatClauses;
      for (Clause* clause : fg->GetEnabledClauses()) {
        if (clause->IsSAT())
          satClauses.push_back(clause);
        else
          unsatClauses.push_back(clause);
      }

      // Select random unsat clause
      std::uniform_int_distribution<> randomInt(0, unsatClauses.size() - 1);
      int randIndex = randomInt(randomGenerator);
      Clause* selectedClause = unsatClauses[randIndex];
      std::vector<Edge*> selectedClauseEdges =
          selectedClause->GetEnabledEdges();

      // Calculate break-count (number of currently satisfied clauses
      // that become unsatisfied if the variable value is fliped) of variables
      // in selected clause
      Variable* lowestBreakCountVar = nullptr;
      int lowestBreakCount = satClauses.size() + 1;
      for (Edge* edge : selectedClauseEdges) {
        int breakCount = 0;
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
        if (getRandomReal01() > wsNoise) {
          lowestBreakCountVar->AssignValue(!lowestBreakCountVar->value);
        }
        // probability p
        else {
          std::uniform_int_distribution<> randEdgeIndexDist(
              0, selectedClauseEdges.size() - 1);
          int randomEdgeIndex = randEdgeIndexDist(randomGenerator);
          Variable* var = selectedClauseEdges[randomEdgeIndex]->variable;
          var->AssignValue(!var->value);
        }
      }
    }
  }

  // 2. If a sat assignment was not found, return false.
  return INDETERMINATE;

  // // Get sub graph to solve
  // vector<Variable*> variables = fg->GetUnassignedVariables();
  // vector<Clause*> clauses = fg->GetEnabledClauses();

  // // http://lcs.ios.ac.cn/~caisw/Paper/Faster_WalkSAT.pdf
  // for (Variable* var : variables) {
  //   for (Edge* edge : var->allNeighbourEdges) {
  //     if (edge->enabled) {
  //       if (edge->type)
  //         var->positiveNeighbourEdges.push_back(edge);
  //       else
  //         var->negativeNeighbourEdges.push_back(edge);
  //     }
  //   }
  // }

  // // Update walksat parameter
  // wsMaxFlips = 100 * variables.size();

  // // Tries loop
  // for (int t = 0; t < wsMaxTries; t++) {
  //   // Assign variables with random values
  //   for (Variable* var : variables) var->AssignValue(getRandomBool());

  //   // Separate clauses into sat and unsat
  //   vector<Clause*> unsatClauses;
  //   for (Clause* clause : clauses) {
  //     clause->trueLiterals = 0;
  //     for (Edge* e : clause->allNeighbourEdges) {
  //       if (e->enabled) {
  //         if (e->type == e->variable->value) clause->trueLiterals++;
  //       }
  //     }
  //     if (clause->trueLiterals == 0) unsatClauses.push_back(clause);
  //   }

  //   // Flips loop
  //   for (int f = 0; f < wsMaxFlips; f++) {
  //     // If no unsat clauses finish algorithm
  //     if (unsatClauses.size() == 0)
  //       return SAT;
  //     else
  //       cout << fg << endl;

  //     // Select random unsat clause, only interest in neighbour variables
  //     uniform_int_distribution<> randomInt(0, unsatClauses.size() - 1);
  //     int randIndex = randomInt(randomGenerator);
  //     vector<Edge*> selectedClauseEdges =
  //         unsatClauses[randIndex]->GetEnabledEdges();

  //     // Calculate break-count (number of currently satisfied clauses
  //     // that become unsatisfied if the variable value is fliped) of
  //     variables
  //     // in selected clause
  //     Variable* lowestBreakCountVar = nullptr;
  //     int lowestBreakCount = clauses.size() + 1;
  //     for (Edge* edge : selectedClauseEdges) {
  //       // Count how many of the clauses where the variable appears are sat
  //       int breakCount = 0;
  //       if (edge->variable->value) {
  //         for (Edge* e : edge->variable->positiveNeighbourEdges) {
  //           if (e->clause->trueLiterals == 1) breakCount++;
  //           if (breakCount > lowestBreakCount) break;
  //         }
  //       } else {
  //         for (Edge* e : edge->variable->negativeNeighbourEdges) {
  //           if (e->clause->trueLiterals == 1) breakCount++;
  //           if (breakCount > lowestBreakCount) break;
  //         }
  //       }

  //       // Update lowest break-count
  //       if (breakCount < lowestBreakCount) {
  //         lowestBreakCountVar = edge->variable;
  //         lowestBreakCount = breakCount;

  //         // Best flip found
  //         if (breakCount < 1) break;
  //       }
  //     }

  //     // Select the variable to flip that has break-count < 1
  //     // If not, with probability p (noise), select a random variable and
  //     // with probability 1 - p, sleect the variable with lowest break-count
  //     Variable* var = nullptr;
  //     if (lowestBreakCount < 1) {
  //       var = lowestBreakCountVar;
  //     } else {
  //       // probability 1 - p
  //       if (getRandomReal01() > wsNoise) {
  //         var = lowestBreakCountVar;
  //       }
  //       // probability p
  //       else {
  //         uniform_int_distribution<> randEdgeIndexDist(
  //             0, selectedClauseEdges.size() - 1);
  //         int randomEdgeIndex = randEdgeIndexDist(randomGenerator);
  //         var = selectedClauseEdges[randomEdgeIndex]->variable;
  //       }
  //     }

  //     // Flip variable
  //     cout << "Fliping variable X" << var->id << " from value " << var->value
  //          << " to value " << !var->value << " (bc: " << lowestBreakCount <<
  //          ")"
  //          << endl;
  //     var->AssignValue(!var->value);
  //     for (Edge* edge : var->allNeighbourEdges) {
  //       if (edge->enabled) {
  //         edge->clause->trueLiterals = 0;
  //         for (Edge* e : edge->clause->allNeighbourEdges) {
  //           if (e->enabled) {
  //             if (e->type == e->variable->value)
  //             edge->clause->trueLiterals++;
  //           }
  //         }
  //         if (edge->clause->trueLiterals == 0)
  //           unsatClauses.push_back(edge->clause);
  //         else
  //           remove(unsatClauses.begin(), unsatClauses.end(), edge->clause);
  //       }
  //     }
  //   }
  //   // Max flips reached
  // }

  // // Max tries reached, solution not found
  // return INDETERMINATE;
}

}  // namespace sat
