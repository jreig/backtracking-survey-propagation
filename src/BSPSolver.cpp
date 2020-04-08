#include <algorithm>
#include <fstream>
#include <iostream>

// Project headers
#include <BSPSolver.hpp>

using namespace std;

namespace bsp {

const float BSPSolver::DEFAULT_EPSILON = 0.001f;
const int BSPSolver::DEFAULT_MAX_ITERATIONS = 10000;
const int BSPSolver::RANDOM_SEED = 0;

// -----------------------------------------------------------------------------
// CONSTRUCTOR AND DESTRUCTOR
// -----------------------------------------------------------------------------
BSPSolver::BSPSolver(float epsilon, int maxIterations, int seed) {
  // SP parameters
  _epsilon = epsilon;
  maxIterations = maxIterations;

  // Initialize random gen with seed or random
  _randomGenerator.seed(seed);
  if (seed == RANDOM_SEED) {
    random_device rd;
    _randomGenerator.seed(rd());
  }

  // Initialize graph to null pointer
  _graph = nullptr;
};

BSPSolver::~BSPSolver() {
  if (_graph != nullptr) delete _graph;
}

// -----------------------------------------------------------------------------
// ALGORITHMS
// -----------------------------------------------------------------------------

BSPResult BSPSolver::SID(const string& path) {
  // 1 - Build a factor graph from a DIMACS file
  if (!StoreFactorGraph(path)) return ERROR;

  // 4 - Repeat untill all variables are assigned or a contradiction is found
  bool contradictionFound = false;
  bool allVariablesAssigned = false;
  while (!allVariablesAssigned && !contradictionFound) {
    // 2 - Run SP. If does not converge, return UNCONVERGED
    BSPResult SPResult = SP(_graph);
    if (SPResult == UNCONVERGED) {
      cout << "WARNING: SP Algorithm coudn't converge" << endl;
      return UNCONVERGED;
    }

    // 3 - Found and assign a variable using decimation
    // assign the variable with the highest abs of evaluation value to true
    // if evaluation value is positive, false otherwise.
    Variable* highestVariable = nullptr;
    float highestValue = -1.0f;
    bool valueOfHighest = true;
    for (Variable* variable : _graph->GetVariables()) {
      float evalValue = EvaluateVariable(variable);

      // Update the higest variable found
      if (abs(evalValue) > highestValue) {
        highestVariable = variable;
        highestValue = abs(evalValue);
        valueOfHighest = evalValue > 0;
      }
    }

    _graph->AssignVariable(highestVariable, valueOfHighest);
    allVariablesAssigned = _graph->GetVariables().size() == 0;
  }

  return contradictionFound ? SAT : UNSAT;
};

BSPResult BSPSolver::SP(FactorGraph* graph) {
  // 1 - Random initialization of survey values
  InitSurveys(graph);

  // 4 - Repeat until all surveys converge or max iterations are reached
  bool allEdgesConverged = false;
  for (int i = 0; i < _maxIterations && !allEdgesConverged; i++) {
    // 2 - Order randomly the set of edges
    ShuffleEdgesOrder(graph);

    // 3 - Update the survey value of each edge
    allEdgesConverged = true;
    for (Edge* edge : graph->GetEdges()) {
      float previousSurveyValue = edge->survey;
      UpdateSurvey(edge);

      // Check if edge converged
      bool hasConverged = abs(edge->survey - previousSurveyValue) < _epsilon;
      if (!hasConverged) allEdgesConverged = false;
    }
  }

  return allEdgesConverged ? CONVERGED : UNCONVERGED;
}

// -----------------------------------------------------------------------------
// PRIVATE FUNCTIONS
// -----------------------------------------------------------------------------

bool BSPSolver::StoreFactorGraph(const string& path) {
  // Delete previous graph if any
  if (_graph != nullptr) {
    delete _graph;
    _graph = nullptr;
  }

  // Open file and check
  ifstream file(path);
  if (!file.is_open()) {
    cerr << "ERROR: Can't open file " << path << endl;
    return false;
  }

  // Initialize graph
  _graph = new FactorGraph(file);

  file.close();

  // Check correct initialization
  if (_graph == nullptr) {
    cerr << "ERROR: FactorGraph not initialized" << endl;
    return false;
  }

  cout << _graph << endl;
  return true;
}

void BSPSolver::InitSurveys(FactorGraph* graph) {
  uniform_real_distribution<> randomNormalDistribution(0, 1);
  for (Edge* edge : graph->GetEdges()) {
    edge->survey = randomNormalDistribution(_randomGenerator);
  }
}

void BSPSolver::ShuffleEdgesOrder(FactorGraph* graph) {
  vector<Edge*> edges = graph->GetEdges();
  shuffle(edges.begin(), edges.end(), _randomGenerator);
}

void BSPSolver::UpdateSurvey(Edge* ai) {
  // Param edge is a->i
  float Sai = 1.0f;

  // For each a->j when j != i
  for (Edge* aj : ai->clausule->GetNeighbourEdges()) {
    if (aj == ai) continue;  // j == i

    // Product values initalization for all b->j survey values
    float Pubj = 1.0f;
    float Psbj = 1.0f;
    float P0bj = 1.0f;

    // For each b->j when b != a
    for (Edge* bj : aj->variable->GetNeighbourEdges()) {
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
};

float BSPSolver::EvaluateVariable(Variable* variable) {
  // Vi  = V(i)  -> Subset of clausules where the variable i appears
  // ViP = V+(i) -> substed of V(i) where i appears unnegated
  // ViN = V-(i) -> substed of V(i) where i appears negated
  // Product values initialization for all a->i survey values
  float PVi0 = 1.0f;
  float PViP = 1.0f;
  float PViN = 1.0f;

  // For each a->i
  for (Edge* ai : variable->GetNeighbourEdges()) {
    if (ai->type == POSITIVE) {
      // Update PViP if variable i appears unnegated in clausule a
      PViP = PViP * (1 - ai->survey);
    }
    if (ai->type == NEGATIVE) {
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

  return WiP - WiN;
}

}  // namespace bsp