#include <iomanip>
#include <string>

// Project headers
#include <FactorGraph.hpp>
#include <Utils.hpp>

namespace sat {

// =============================================================================
// Variable class
// =============================================================================
Variable::Variable(const unsigned id)
    : id(id), evalValue(0.0f), assigned(_assigned), value(_value) {}

std::vector<Edge*> Variable::GetEnabledEdges() {
  std::vector<Edge*> enabledNeigbours;
  for (Edge* edge : _allNeighbourEdges) {
    if (edge->enabled) enabledNeigbours.push_back(edge);
  }
  return enabledNeigbours;
}

void Variable::AssignValue(const bool newValue, AssignmentStep* stp) {
  _value = newValue;
  _assigned = true;

  if (stp != nullptr) stp->variables.push_back(this);
}

// std::ostream& operator<<(std::ostream& os, const Variable* var) {
//   os << "X" << var->id << ": "
//      << (var->assigned ? (var->value ? "true" : "false") : "NOT_ASSIGNED");
//   return os;
// }

// =============================================================================
// Clausule class
// =============================================================================
Clausule::Clausule(const unsigned id) : id(id), enabled(_enabled) {}

std::vector<Edge*> Clausule::GetEnabledEdges() {
  std::vector<Edge*> enabledNeigbours;
  for (Edge* edge : _allNeighbourEdges) {
    if (edge->enabled) enabledNeigbours.push_back(edge);
  }
  return enabledNeigbours;
}

void Clausule::Dissable(AssignmentStep* stp) {
  _enabled = false;
  if (stp != nullptr) stp->clausules.push_back(this);

  for (Edge* edge : _allNeighbourEdges) {
    if (edge->enabled) edge->Dissable(stp);
  }
}

bool Clausule::IsSAT() const {
  for (Edge* edge : _allNeighbourEdges) {
    if (!edge->enabled) continue;
    if (edge->type == edge->variable->value) return true;
  }

  return false;
}

// std::ostream& operator<<(std::ostream& os, const Clausule* c) {
//   os << "C" << c->id << ": ";
//   os << c->GetEnabledEdges().size() << " variables - ";
//   os << (c->enabled ? "ENABLED" : "DISABLED");
//   return os;
// }

// =============================================================================
// Edge class
// =============================================================================
Edge::Edge(bool type, Clausule* clausule, Variable* variable)
    : type(type),
      clausule(clausule),
      variable(variable),
      survey(0.0f),
      enabled(_enabled) {}

void Edge::Dissable(AssignmentStep* stp) {
  _enabled = false;
  if (stp != nullptr) stp->edges.push_back(this);
}

// std::ostream& operator<<(std::ostream& os, const Edge* e) {
//   os << "C" << e->clausule->id << " <---> ";
//   os << (e->type ? " X" : "¬X") << e->variable->id;
//   os << " - " << (e->enabled ? "ENABLED " : "DISABLED");
//   os << " (" << std::fixed << std::setprecision(10) << e->survey << ")";
//   return os;
// }

// =============================================================================
// FactorGraph class
// =============================================================================
FactorGraph::FactorGraph(std::ifstream& file) {
  // Process each line of the dimacs file
  bool configured = false;
  int currentClausuleIndex = 0;
  std::string line;
  while (getline(file, line)) {
    // Split the lines into tokens
    const std::vector<std::string> tokens = utils::SplitString(line);

    // If first token is a 'c' ignore the line because is a comment
    if (tokens[0] == "c") continue;

    // If first token is a 'p' and second is 'cnf',
    // the line contains the number of variables (3rd) and clausules (4th)
    else if (tokens[0] == "p" && tokens[1] == "cnf") {
      unsigned int totalVariables = stoi(tokens[2]);
      unsigned int totalClausules = stoi(tokens[3]);

      // Create variables
      for (unsigned i = 0; i < totalVariables; i++) {
        Variable* variable = new Variable(i + 1);
        _variables.push_back(variable);
      }

      // Create clausules
      for (unsigned i = 0; i < totalClausules; i++) {
        Clausule* clausule = new Clausule(i);
        _clausules.push_back(clausule);
      }

      configured = true;
    }

    // Every other line should be a clausule containing variables
    else {
      if (configured) {
        for (const std::string& token : tokens) {
          // "0" means end of the clausule
          if (token != "0") {
            const int variableValue = stoi(token);
            // variables start from 1 and indices from 0
            const int variableIndex = abs(variableValue) - 1;

            // Create an edge
            bool edgeType = variableValue > 0;
            Clausule* clausule = _clausules[currentClausuleIndex];
            Variable* variable = _variables[variableIndex];

            Edge* edge = new Edge(edgeType, clausule, variable);
            _edges.push_back(edge);

            // Connect clausules and variables with the edge
            clausule->_allNeighbourEdges.push_back(edge);
            variable->_allNeighbourEdges.push_back(edge);
          }
        }

        // Next clausule
        currentClausuleIndex += 1;
      }
    }
  }
}

FactorGraph::~FactorGraph() {
  for (Clausule* clausule : _clausules) delete clausule;
  for (Variable* variable : _variables) delete variable;
  for (Edge* edge : _edges) delete edge;
  for (AssignmentStep* step : _assignmentSteps) delete step;
}

std::vector<Variable*> FactorGraph::GetUnassignedVariables() {
  std::vector<Variable*> unassignedVariables;
  for (Variable* variable : _variables) {
    if (!variable->assigned) unassignedVariables.push_back(variable);
  }
  return unassignedVariables;
}

std::vector<Clausule*> FactorGraph::GetEnabledClausules() {
  std::vector<Clausule*> enabledClausules;
  for (Clausule* clausule : _clausules) {
    if (clausule->enabled) enabledClausules.push_back(clausule);
  }
  return enabledClausules;
}

std::vector<Edge*> FactorGraph::GetEnabledEdges() {
  std::vector<Edge*> enabledEdges;
  for (Edge* edge : _edges) {
    if (edge->enabled) enabledEdges.push_back(edge);
  }
  return enabledEdges;
}

bool FactorGraph::IsSAT() const {
  for (Clausule* clausule : _clausules) {
    if (!clausule->enabled) continue;
    if (!clausule->IsSAT()) return false;
  }

  return true;
}

void FactorGraph::RevertLastAssigment() {
  AssignmentStep* lastStep = _assignmentSteps.back();
  _assignmentSteps.pop_back();

  for (Variable* variable : lastStep->variables) variable->Unassign();
  for (Clausule* clausule : lastStep->clausules) clausule->Enable();
  for (Edge* edge : lastStep->edges) edge->Enable();
}

std::ostream& operator<<(std::ostream& os, FactorGraph* fg) {
  unsigned totalVariables = fg->_variables.size();
  unsigned assignedVariables =
      totalVariables - fg->GetUnassignedVariables().size();

  unsigned totalClausules = fg->_clausules.size();
  unsigned satClausules = totalClausules - fg->GetEnabledClausules().size();

  os << "Assigned Variables: ";
  os << assignedVariables << "/" << totalVariables;
  os << " - Satisfied Clausules: ";
  os << satClausules << "/" << totalClausules;
  return os;
}

}  // namespace sat