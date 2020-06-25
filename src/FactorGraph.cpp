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

void Variable::AssignValue(const bool newValue) {
  _value = newValue;
  _assigned = true;
}

// std::ostream& operator<<(std::ostream& os, const Variable* var) {
//   os << "X" << var->id << ": "
//      << (var->assigned ? (var->value ? "true" : "false") : "NOT_ASSIGNED");
//   return os;
// }

// =============================================================================
// Clause class
// =============================================================================
Clause::Clause(const unsigned id) : id(id), enabled(_enabled) {}

std::vector<Edge*> Clause::GetEnabledEdges() {
  std::vector<Edge*> enabledNeigbours;
  for (Edge* edge : _allNeighbourEdges) {
    if (edge->enabled) enabledNeigbours.push_back(edge);
  }
  return enabledNeigbours;
}

void Clause::Dissable() {
  _enabled = false;

  for (Edge* edge : _allNeighbourEdges) {
    if (edge->enabled) edge->Dissable();
  }
}

bool Clause::IsSAT() const {
  for (Edge* edge : _allNeighbourEdges) {
    if (edge->enabled && edge->variable->assigned &&
        edge->type == edge->variable->value)
      return true;
  }

  return false;
}

// std::ostream& operator<<(std::ostream& os, const Clause* c) {
//   os << "C" << c->id << ": ";
//   os << c->GetEnabledEdges().size() << " variables - ";
//   os << (c->enabled ? "ENABLED" : "DISABLED");
//   return os;
// }

// =============================================================================
// Edge class
// =============================================================================
Edge::Edge(bool type, Clause* clause, Variable* variable)
    : type(type),
      clause(clause),
      variable(variable),
      survey(0.0f),
      enabled(_enabled) {}

void Edge::Dissable() { _enabled = false; }

std::ostream& operator<<(std::ostream& os, const Edge* e) {
  os << "C" << e->clause->id << " <---> ";
  os << (e->type ? " X" : "¬X") << e->variable->id;
  os << " - " << (e->enabled ? "ENABLED " : "DISABLED");
  // os << " (" << std::fixed << std::setprecision(10) << e->survey << ")";
  os << " (" << e->survey << ")";
  return os;
}

// =============================================================================
// FactorGraph class
// =============================================================================
FactorGraph::FactorGraph(std::ifstream& file) {
  // Process each line of the dimacs file
  bool configured = false;
  int currentClauseIndex = 0;
  std::string line;
  while (getline(file, line)) {
    // Split the lines into tokens
    const std::vector<std::string> tokens = utils::SplitString(line);

    // If first token is a 'c' ignore the line because is a comment
    if (tokens[0] == "c") continue;

    // If first token is a 'p' and second is 'cnf',
    // the line contains the number of variables (3rd) and clauses (4th)
    else if (tokens[0] == "p" && tokens[1] == "cnf") {
      unsigned int totalVariables = stoi(tokens[2]);
      unsigned int totalClauses = stoi(tokens[3]);

      // Create variables
      for (unsigned i = 0; i < totalVariables; i++) {
        Variable* variable = new Variable(i + 1);
        _variables.push_back(variable);
      }

      // Create clauses
      for (unsigned i = 0; i < totalClauses; i++) {
        Clause* clause = new Clause(i);
        _clauses.push_back(clause);
      }

      configured = true;
    }

    // Every other line should be a clause containing variables
    else {
      if (configured) {
        for (const std::string& token : tokens) {
          // "0" means end of the clause
          if (token != "0") {
            const int variableValue = stoi(token);
            // variables start from 1 and indices from 0
            const int variableIndex = abs(variableValue) - 1;

            // Create an edge
            bool edgeType = variableValue > 0;
            Clause* clause = _clauses[currentClauseIndex];
            Variable* variable = _variables[variableIndex];

            Edge* edge = new Edge(edgeType, clause, variable);
            _edges.push_back(edge);

            // Connect clauses and variables with the edge
            clause->_allNeighbourEdges.push_back(edge);
            variable->_allNeighbourEdges.push_back(edge);
          }
        }

        // Next clause
        currentClauseIndex += 1;
      }
    }
  }
}

FactorGraph::~FactorGraph() {
  for (Clause* clause : _clauses) delete clause;
  for (Variable* variable : _variables) delete variable;
  for (Edge* edge : _edges) delete edge;
}

std::vector<Variable*> FactorGraph::GetUnassignedVariables() {
  std::vector<Variable*> unassignedVariables;
  for (Variable* variable : _variables) {
    if (!variable->assigned) unassignedVariables.push_back(variable);
  }
  return unassignedVariables;
}

std::vector<Clause*> FactorGraph::GetEnabledClauses() {
  std::vector<Clause*> enabledClauses;
  for (Clause* clause : _clauses) {
    if (clause->enabled) enabledClauses.push_back(clause);
  }
  return enabledClauses;
}

std::vector<Edge*> FactorGraph::GetEnabledEdges() {
  std::vector<Edge*> enabledEdges;
  for (Edge* edge : _edges) {
    if (edge->enabled) enabledEdges.push_back(edge);
  }
  return enabledEdges;
}

bool FactorGraph::IsSAT() const {
  for (Clause* clause : _clauses) {
    if (clause->enabled && !clause->IsSAT()) return false;
  }

  return true;
}

std::ostream& operator<<(std::ostream& os, FactorGraph* fg) {
  unsigned totalVariables = fg->_variables.size();
  unsigned assignedVariables =
      totalVariables - fg->GetUnassignedVariables().size();

  unsigned totalClauses = fg->_clauses.size();
  unsigned satClauses = totalClauses - fg->GetEnabledClauses().size();

  os << "Assigned Variables: ";
  os << assignedVariables << "/" << totalVariables;
  os << " - Satisfied Clauses: ";
  os << satClauses << "/" << totalClauses;
  return os;
}

}  // namespace sat