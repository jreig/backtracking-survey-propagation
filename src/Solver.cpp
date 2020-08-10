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
  totalSPIterations = 0;

  int assignFraction = (int)(N * fraction);
  if (assignFraction < 1) assignFraction = 1;

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
        // printf("X%d H.p:%f - H.m:%f\n", var->id, var->Hp, var->Hm);
        // cout << "X" << var->id << " H.p:" << var->Hp << " - H.m:" << var->Hm
        // << endl;
        double maxBias = var->Hp > var->Hm ? var->Hp : var->Hm;
        sumMaxBias += maxBias;
        unassignedVariables.push_back(var);
      }
    }

    // int prevUnsassignedVars = unassignedVariables.size();

    // printf("<bias>:%f\n", sumMaxBias / unassignedVariables.size());
    // string a;
    // cin >> a;
    // Check paramagnetic state
    // TODO: Entender que significa esto, en el codigo original, este es
    // el unico sitio donde se llama a walksat
    if (sumMaxBias / unassignedVariables.size() < paramagneticState) {
      cout << "Paramagnetic state reached" << endl;
      // cout << fg << endl;
      return walksat();
    }

    // Assign minimum 1 variable
    sort(unassignedVariables.begin(), unassignedVariables.end(),
         [](const Variable* lvar, const Variable* rvar) {
           return std::abs(lvar->evalValue) > std::abs(rvar->evalValue);
         });

    // cout << unassignedVariables[0]->id << ": "
    //      << unassignedVariables[0]->evalValue << ", "
    //      << unassignedVariables[1]->id << ": "
    //      << unassignedVariables[1]->evalValue << ", "
    //      << unassignedVariables[2]->id << ": "
    //      << unassignedVariables[2]->evalValue << endl;

    // ------------------------
    // Fix the set of variables
    // ------------------------
    // int assignFraction = (int)(unassignedVariables.size() * fraction);
    // if (assignFraction < 1) assignFraction = 1;
    int auxAssign = assignFraction;
    for (int i = 0; i < auxAssign; i++) {
      // Variables in the list can be already assigned due to UP being executed
      // in previous iterations
      if (unassignedVariables[i]->assigned) {
        auxAssign++;  // Don't count this variable as a new assignation
        continue;
      }

      // Found the new value and assign the variable
      // The assignation method cleans the graph and execute UP if one of
      // the cleaned clause become unitary
      Variable* var = unassignedVariables[i];

      // Recalculate biases for same reason, previous assignations clean the
      // graph and change relations
      evaluateVar(var);
      bool newValue = var->Hp > var->Hm ? false : true;

      if (!assignVariable(var, newValue)) {
        // Error found when assigning variable
        return CONTRADICTION;
      }
    }

    // int postUnassignVars = fg->GetUnassignedVariables().size();
    // int upAssignedVars =
    //     prevUnsassignedVars - postUnassignVars - assignFraction;
    // cout << "Assigned " << assignFraction << " variables (+" <<
    // upAssignedVars
    //      << " up)" << endl;

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
    totalSPIterations++;
    // cout << "." << flush;
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

      // cout << ":-)" << endl;
      // string aa;
      // cin >> aa;

      // if (maxConvergeDiff < ZERO_EPSILON) {
      //   cout << "Trivial state reached" << endl;
      //   cout << fg << endl;
      //   return walksat();
      // }

      // If not triavial return and continue algorith
      return CONVERGE;
    }
  }
  // cout << ":-(" << endl;
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
          // If edge is negative update positive subproduct of variable
          if (!edge->type) {
            // If edge survey != 1
            if (1.0 - edge->survey > ZERO_EPSILON) {
              var->p *= 1.0 - edge->survey;
            }
            // If edge survey == 1
            else
              var->pzero++;
          }
          // If edge is positive, update negative subproduct of variable
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
    if (edge->enabled && !edge->variable->assigned) {
      Variable* var = edge->variable;
      double m, p, wn, wt;

      // If edge is negative:
      if (!edge->type) {
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
      // If edge is positive
      else {
        p = var->pzero ? 0 : var->p;
        if (var->mzero == 0)
          m = var->m / (1.0 - edge->survey);
        else if (var->mzero == 1 && (1.0 - edge->survey) < ZERO_EPSILON)
          m = var->m;
        else
          m = 0.0;

        wn = m * (1 - p);
        wt = p;
      }

      // Calculate subSurvey
      double subSurvey = wn / (wn + wt);
      subSurveys.push_back(subSurvey);

      // If subsurvey is 0 keep track but don't multiply
      if (subSurvey < ZERO_EPSILON) {
        zeros++;
        if (zeros == 2) break;
      } else
        allSubSurveys *= subSurvey;
    }
  }

  // =========================================================
  // Calculate the survey for each edge with the previous data
  // =========================================================
  int i = 0;
  for (Edge* edge : clause->allNeighbourEdges) {
    if (edge->enabled && !edge->variable->assigned) {
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
      // If edge is negative update positive subproduct
      if (!edge->type) {
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
      // If edge is positive, update negative subproduct
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
      double edgeConvDiff = std::abs(edge->survey - newSurvey);
      if (maxConvDiffInClause < edgeConvDiff)
        maxConvDiffInClause = std::abs(edgeConvDiff);

      edge->survey = newSurvey;
      i++;
    }
  }

  return maxConvDiffInClause;
}

bool Solver::assignVariable(Variable* var, bool value) {
  // Contradiction if variable was already assigned with different value
  if (var->assigned && var->value != value) {
    cout << "ERROR: Variable X" << var->id << " already assigned" << endl;
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
  var->evalValue = std::abs(var->Hp - var->Hm);
}

AlgorithmResult Solver::walksat() {
  // Get variables and clauses of subgraph
  vector<Variable*> variables = fg->GetUnassignedVariables();
  vector<Clause*> clauses = fg->GetEnabledClauses();

  cout << "Subformula has " << clauses.size() << " clauses and "
       << variables.size() << " variables" << endl;

  vector<Clause*> unsatClauses;
  for (int t = 0; t < wsMaxTries; t++) {
    // Assign all Varibles with random values
    for (Variable* var : variables) {
      var->AssignValue(getRandomBool());
    }

    // Separate unsat clauses
    unsatClauses.clear();
    for (Clause* clause : clauses) {
      if (clause->countTrueLiterals() == 0) unsatClauses.push_back(clause);
    }

    for (int f = 0; f < wsMaxFlips; f++) {
      // If there are no unsat clauses, subgraph is solved and it's SAT
      if (unsatClauses.size() == 0) return SAT;

      // Select random unsat clause
      std::uniform_int_distribution<> randomInt(0, unsatClauses.size() - 1);
      int randIndex = randomInt(randomGenerator);
      Clause* selectedClause = unsatClauses[randIndex];
      std::vector<Edge*> selectedClauseEdges =
          selectedClause->GetEnabledEdges();

      // -----------------------------------------------------------------------
      // For each variable in selected clause, calculate break-count (number of
      // currently satisfied clauses that become unsatisfied if the variable
      // value is fliped) and store lowest break-count
      // Fast-walksat is used to compute break-count
      // -----------------------------------------------------------------------
      vector<Variable*> lowestBreakCountVar;
      int lowestBreakCount = N * alpha + 1;
      for (Edge* edge : selectedClauseEdges) {
        int breakCount = 0;
        for (Edge* e : edge->variable->allNeighbourEdges) {
          // Only clauses that are satisfied by the var and have only one
          // literal will become unsat
          if (e->enabled && edge->variable->value == e->type &&
              e->clause->trueLiterals == 1)
            breakCount++;

          // TODO: Remove old way of computing break-count
          // if (e->clause->IsSAT()) {
          //   edge->variable->AssignValue(!edge->variable->value);
          //   if (!e->clause->IsSAT()) breakCount++;
          //   edge->variable->AssignValue(!edge->variable->value);
          // }
        }

        // Update lowest break-count
        if (breakCount == lowestBreakCount)
          lowestBreakCountVar.push_back(edge->variable);
        if (breakCount < lowestBreakCount) {
          lowestBreakCountVar.clear();
          lowestBreakCountVar.push_back(edge->variable);
          lowestBreakCount = breakCount;
        }
      }

      // -----------------------------------------------------------------------
      // Select the variable of the clause that has break-count = 0
      // If not, with probability p (noise), flip a random variable and
      // with probability 1 - p, flip the variable with lowest break-count
      // -----------------------------------------------------------------------
      Variable* var = nullptr;
      // Select the var with lower break-count with probability 1 - p or force
      // it if break-count == 0
      // If multiple vars have same breack-count, select randomly
      if (lowestBreakCount == 0 || getRandomReal01() > wsNoise) {
        if (lowestBreakCountVar.size() == 1) {
          var = lowestBreakCountVar[0];
        } else {
          uniform_int_distribution<> randi(0, lowestBreakCountVar.size() - 1);
          int i = randi(randomGenerator);
          var = lowestBreakCountVar[i];
        }
      }
      // Select random var with probability p
      else {
        std::uniform_int_distribution<> randEdgeIndexDist(
            0, selectedClauseEdges.size() - 1);
        int randomEdgeIndex = randEdgeIndexDist(randomGenerator);
        var = selectedClauseEdges[randomEdgeIndex]->variable;
      }

      // -----------------------------------------------------------------------
      // Flip de selected variable and update unsatClause list by removing all
      // unsat clauses where the variable appear, flip it and then, add the new
      // unsat clauses
      // -----------------------------------------------------------------------
      for (Edge* e : var->allNeighbourEdges) {
        if (e->enabled && e->clause->trueLiterals == 0) {
          unsatClauses.erase(
              find(unsatClauses.begin(), unsatClauses.end(), e->clause));
        }
      }

      var->AssignValue(!var->value);

      for (Edge* e : var->allNeighbourEdges) {
        if (e->enabled && e->clause->countTrueLiterals() == 0)
          unsatClauses.push_back(e->clause);
      }
    }
  }

  // 2. If a sat assignment was not found, return false.
  return INDETERMINATE;
}

}  // namespace sat
