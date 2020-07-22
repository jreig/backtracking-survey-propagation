#include <Solver.hpp>
#include <algorithm>

namespace sat {

// =============================================================================
// Solver
// =============================================================================
Solver::Solver(int N, float a, int seed)
    : initialSeed(seed),
      randomBoolUD(0, 1),
      randomReal01UD(0, 1),
      N(N),
      alpha(a),
      wsMaxFlips(100 * N) {
  // Random number generator initialization
  if (seed = 0) initialSeed = rd();
  randomGenerator.seed(initialSeed);
}

// =============================================================================
// Algorithms
// =============================================================================
AlgorithmResult Solver::SID(FactorGraph* graph, float fraction) {
  fg = graph;
  sidFraction = fraction;

  // --------------------------------
  // Random initialization of surveys
  // --------------------------------
  for (Edge* edge : fg->edges) {
    edge->survey = getRandomReal01();
  }

  // ----------------------------
  // Run SP and check convergence
  // ----------------------------
  // TODO

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
    }
  }

  // Check paramagnetic state
  // TODO: Entender que significa esto
  if (sumMaxBias / unassignedVariables.size() < paramagneticState) {
    // Call walksat
    // TODO
  }

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
    // Variables in the list can be assigned due to UP being executed
    // in previous iterations when assigning a variable
    if (unassignedVariables[i]->assigned) {
      i--;  // Don't count his variable as a new assignation
      continue;
    }

    // Found the new value and assign the variable
    // The assignation method cleans the graph and execute UP if one of
    // the cleaned clause become unitary
    Variable* var = unassignedVariables[i];

    // Recalculate biases for same reason, previous assignations clean the graph
    // and change relations
    evaluateVar(var);
    bool newValue = var->Hp > var->Hm ? true : false;

    if (!assignVariable(var, newValue)) {
      // Error found when assigning variable
      return CONTRADICTION;
    }
  }

  // Print graph status
  cout << fg << endl;

  // ----------------------------
  // If SAT return solution found
  // ----------------------------
  if (fg->IsSAT()) {
    return SAT;
  }

  // If not converge return INDETERMINATE
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

AlgorithmResult Solver::SP() {}

}  // namespace sat
