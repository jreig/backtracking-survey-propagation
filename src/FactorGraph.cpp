#include <algorithm>
#include <iostream>
#include <sstream>

// Project headers
#include <FactorGraph.hpp>

using namespace std;

namespace bsp {
// -----------------------------------------------------------------------------
// SplitString
//
// Splits the string into token by the given delimiter
// Return a vector with the tokens
// -----------------------------------------------------------------------------
const vector<string> SplitString(const string& s, const char delim = ' ') {
  stringstream stream(s);
  vector<string> tokens;

  string token;
  while (getline(stream, token, delim)) {
    tokens.push_back(token);
  }

  return tokens;
}

// =============================================================================
// Node class
// =============================================================================
Node::Node(const unsigned id, const NodeType type) : id(id), type(type){};

void Node::AddNeighbourEdge(Edge* edge) { _allNeighbourEdges.push_back(edge); }

vector<Edge*> Node::GetNeighbourEdges() const {
  vector<Edge*> enabledNeigbours;
  for (Edge* edge : _allNeighbourEdges) {
    if (edge->enabled) enabledNeigbours.push_back(edge);
  }
  return enabledNeigbours;
}

// =============================================================================
// Variable class
// =============================================================================
Variable::Variable(const unsigned id)
    : Node(id, VARIABLE), assigned(_assigned), value(_value) {}

ostream& operator<<(ostream& os, const Variable* var) {
  os << "X" << (var->id + 1) << ": "
     << (var->assigned ? (var->value ? "true" : "false") : "NOT_ASSIGNED");
  return os;
}

// =============================================================================
// Clausule class
// =============================================================================
Clausule::Clausule(const unsigned id) : Node(id, CLAUSULE), enabled(_enabled) {}

ostream& operator<<(ostream& os, const Clausule* c) {
  os << (c->id + 'a') << ": ";
  os << c->GetNeighbourEdges().size() << " variables - ";
  os << (c->enabled ? "ENABLED" : "DISABLED");
  return os;
}

// =============================================================================
// Edge class
// =============================================================================
Edge::Edge(EdgeType type, Clausule* clausule, Variable* variable)
    : type(type),
      clausule(clausule),
      variable(variable),
      survey(0),
      enabled(_enabled) {}

ostream& operator<<(ostream& os, const Edge* e) {
  os << (e->clausule->id + 'a') << " <---> ";
  os << (e->type == NEGATIVE ? "¬X" : "X") << (e->variable->id + 1);
  os << " - " << (e->enabled ? "ENABLED" : "DISABLED");
  return os;
}

// =============================================================================
// FactorGraph class
// =============================================================================
FactorGraph::FactorGraph(ifstream& file)
    : NTotalClausules(_NTotalClausules),
      NTotalVariables(_NTotalVariables),
      NTotalEdges(_NTotalEdges) {
  // Process each line of the dimacs file
  bool configured = false;
  int currentClausuleId = 0;
  string line;
  while (getline(file, line)) {
    // Split the lines into tokens
    const vector<string> tokens = SplitString(line);

    // If first token is a 'c' ignore the line because is a comment
    if (tokens[0] == "c") continue;

    // If first token is a 'p' and second is 'cnf',
    // the line contains the number of variables (3rd) and clausules (4th)
    else if (tokens[0] == "p" && tokens[1] == "cnf") {
      _NTotalVariables = stoi(tokens[2]);
      _NTotalClausules = stoi(tokens[3]);

      // Create variables
      for (unsigned i = 0; i < _NTotalVariables; i++) {
        Variable* variable = new Variable(i);
        _variables.push_back(variable);
      }

      // Create clausules
      for (int i = 0; i < _NTotalClausules; i++) {
        Clausule* clausule = new Clausule(i);
        _clausules.push_back(clausule);
      }

      configured = true;
    }

    // Every other line should be a clausule containing variables
    else {
      if (configured) {
        for (const string& token : tokens) {
          if (token != "0") {  // "0" means end of the clausule
            const int variableValue = stoi(token);
            // variables start from 1 and indices from 0
            const int variableId = abs(variableValue) - 1;

            // Create an edge
            EdgeType type = (variableValue < 0) ? NEGATIVE : POSITIVE;
            Clausule* clausule = _clausules[currentClausuleId];
            Variable* variable = _variables[variableId];

            Edge* edge = new Edge(type, clausule, variable);
            _edges.push_back(edge);

            // Connect clausules and variables with the edge
            clausule->AddNeighbourEdge(edge);
            variable->AddNeighbourEdge(edge);
          }
        }

        // Next clausule
        currentClausuleId += 1;
      }
    }
  }
}

FactorGraph::~FactorGraph() {
  for (const Clausule* clausule : _clausules) delete clausule;
  for (const Variable* variable : _variables) delete variable;
  for (const Edge* edge : _edges) delete edge;
}

vector<Clausule*> FactorGraph::GetClausules() const {
  vector<Clausule*> enabledClausules;
  for (Clausule* clausule : _clausules) {
    if (clausule->enabled) enabledClausules.push_back(clausule);
  }
  return enabledClausules;
}

vector<Variable*> FactorGraph::GetVariables() const {
  vector<Variable*> enabledVariables;
  for (Variable* variable : _variables) {
    if (variable->assigned) enabledVariables.push_back(variable);
  }
  return enabledVariables;
}

vector<Edge*> FactorGraph::GetEdges() const {
  vector<Edge*> enabledEdges;
  for (Edge* edge : _edges) {
    if (edge->enabled) enabledEdges.push_back(edge);
  }
  return enabledEdges;
}

ostream& operator<<(ostream& os, const FactorGraph* fg) {
  os << "Assigned Variables: ";
  os << fg->GetVariables().size() << "/" << fg->NTotalVariables;
  os << "Satisfied Clausules: ";
  os << fg->GetClausules().size() << "/" << fg->NTotalClausules;
}

void FactorGraph::AssignVariable(Variable* variable, bool value) {
  BactrackingStep step;

  variable->_assigned = true;
  variable->_value = value;
  step.variable = variable;

  for (Edge* edge : variable->_allNeighbourEdges) {
    if (!edge->enabled)
      continue;
    else {
      edge->_enabled = false;
      step.edges.push_back(edge);
    }

    // Disable clausules satisfied by the assignment
    if ((value && edge->type == POSITIVE) ||
        (!value && edge->type == NEGATIVE)) {
      edge->clausule->_enabled = false;
      step.clausules.push_back(edge->clausule);

      // Disable clausule edges
      for (Edge* clausuleEdge : edge->clausule->_allNeighbourEdges) {
        if (!clausuleEdge->enabled)
          continue;
        else {
          clausuleEdge->_enabled = false;
          step.edges.push_back(clausuleEdge);
        }
      }
    }
  }

  _bactrackingSteps.push(step);
}

void FactorGraph::RevertLastAssigment() {
  BactrackingStep lastStep = _bactrackingSteps.top();
  _bactrackingSteps.pop();

  lastStep.variable->_assigned = false;

  for (Clausule* clausule : lastStep.clausules) {
    clausule->_enabled = true;
  }

  for (Edge* edge : lastStep.edges) {
    edge->_enabled = true;
  }
}

}  // namespace bsp