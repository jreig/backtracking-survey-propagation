#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

// Project headers
#include <FactorGraph.hpp>

namespace sat {

// =============================================================================
// Variable class
// =============================================================================
Variable::Variable(const unsigned id) : id(id), assigned(false) {}

std::vector<Edge*> Variable::GetEnabledEdges() {
  std::vector<Edge*> enabledNeigbours;
  for (Edge* edge : allNeighbourEdges) {
    if (edge->enabled) enabledNeigbours.push_back(edge);
  }
  return enabledNeigbours;
}

void Variable::AssignValue(const bool newValue) {
  value = newValue;
  assigned = true;
}

std::ostream& operator<<(std::ostream& os, const Variable* var) {
  os << "X" << var->id << ": "
     << (var->assigned ? (var->value ? "true" : "false") : "NOT_ASSIGNED");
  return os;
}

// =============================================================================
// Clause class
// =============================================================================
Clause::Clause(const unsigned id) : id(id), enabled(true) {}

std::vector<Edge*> Clause::GetEnabledEdges() const {
  std::vector<Edge*> enabledNeigbours;
  for (Edge* edge : allNeighbourEdges) {
    if (edge->enabled) enabledNeigbours.push_back(edge);
  }
  return enabledNeigbours;
}

void Clause::Dissable() {
  enabled = false;

  for (Edge* edge : allNeighbourEdges) {
    if (edge->enabled) edge->Dissable();
  }
}

bool Clause::IsSAT() const {
  for (Edge* edge : allNeighbourEdges) {
    if (edge->variable->assigned && edge->type == edge->variable->value)
      return true;
  }

  return false;
}

std::ostream& operator<<(std::ostream& os, const Clause* c) {
  os << "C" << c->id << ": ";
  os << c->GetEnabledEdges().size() << " literals - ";
  os << (c->enabled ? "ENABLED" : "DISABLED");
  return os;
}

// =============================================================================
// Edge class
// =============================================================================
Edge::Edge(bool type, Clause* clause, Variable* variable)
    : type(type), enabled(true), clause(clause), variable(variable) {}

void Edge::Dissable() { enabled = false; }

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
const std::vector<std::string> FactorGraph::SplitString(const std::string& s) {
  const char delim = ' ';
  std::stringstream stream(s);
  std::vector<std::string> tokens;

  std::string token;
  while (std::getline(stream, token, delim)) {
    tokens.push_back(token);
  }

  return tokens;
}

FactorGraph::FactorGraph(std::ifstream& file) {
  // Process each line of the dimacs file
  bool configured = false;
  int currentClauseIndex = 0;
  std::string line;
  while (getline(file, line)) {
    // Split the lines into tokens
    const std::vector<std::string> tokens = SplitString(line);

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
        variables.push_back(variable);
      }

      // Create clauses
      for (unsigned i = 0; i < totalClauses; i++) {
        Clause* clause = new Clause(i + 1);
        clauses.push_back(clause);
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
            const int variableIndex = std::abs(variableValue) - 1;

            // Create an edge
            bool edgeType = variableValue > 0;
            Clause* clause = clauses[currentClauseIndex];
            Variable* variable = variables[variableIndex];

            Edge* edge = new Edge(edgeType, clause, variable);
            edges.push_back(edge);

            // Connect clauses and variables with the edge
            clause->allNeighbourEdges.push_back(edge);
            variable->allNeighbourEdges.push_back(edge);
          }
        }

        // Next clause
        currentClauseIndex += 1;
      }
    }
  }
}

FactorGraph::~FactorGraph() {
  for (Clause* clause : clauses) delete clause;
  for (Variable* variable : variables) delete variable;
  for (Edge* edge : edges) delete edge;
}

std::vector<Variable*> FactorGraph::GetUnassignedVariables() {
  std::vector<Variable*> unassignedVariables;
  for (Variable* variable : variables) {
    if (!variable->assigned) unassignedVariables.push_back(variable);
  }
  return unassignedVariables;
}

std::vector<Clause*> FactorGraph::GetEnabledClauses() {
  std::vector<Clause*> enabledClauses;
  for (Clause* clause : clauses) {
    if (clause->enabled) enabledClauses.push_back(clause);
  }
  return enabledClauses;
}

std::vector<Edge*> FactorGraph::GetEnabledEdges() {
  std::vector<Edge*> enabledEdges;
  for (Edge* edge : edges) {
    if (edge->enabled) enabledEdges.push_back(edge);
  }
  return enabledEdges;
}

bool FactorGraph::IsSAT() const {
  for (Clause* clause : clauses) {
    if (!clause->IsSAT()) return false;
  }

  return true;
}

std::ostream& operator<<(std::ostream& os, FactorGraph* fg) {
  unsigned totalVariables = fg->variables.size();
  unsigned assignedVariables =
      totalVariables - fg->GetUnassignedVariables().size();

  unsigned totalClauses = fg->clauses.size();
  unsigned satClauses = totalClauses - fg->GetEnabledClauses().size();

  os << "Assigned Variables: ";
  os << assignedVariables << "/" << totalVariables;
  os << " - Satisfied Clauses: ";
  os << satClauses << "/" << totalClauses;
  return os;
}

}  // namespace sat