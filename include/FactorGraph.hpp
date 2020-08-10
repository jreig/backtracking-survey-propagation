#pragma once

#include <fstream>
#include <iostream>
#include <vector>

namespace sat {

// Declarations to avoid circular dependencies
class Edge;
class FactorGraph;

// =============================================================================
// Variable
//
// Class to represent a variable in the graph.
// Can be assigned with a value (true, false) and store an evaluation value.
// Has a vector of neighbour edges that connects the variable with all clauses
// where it appears.
// =============================================================================
class Variable {
 public:
  const unsigned id;
  bool assigned;
  bool value;

  std::vector<Edge*> allNeighbourEdges;
  std::vector<Edge*> positiveNeighbourEdges;
  std::vector<Edge*> negativeNeighbourEdges;

  // Variables to store sub products to optimize the calculation of
  // equations 26 and 31
  double p;   // Pa€V+(i) (1 - na->i)
  double m;   // Pa€V-(i) (1 - na->i)
  int pzero;  // Number of surveys == 1 in p
  int mzero;  // Number of surveys == 1 in m

  double Hp;
  double Hz;
  double Hm;

  double evalValue;

 public:
  // ---------------------------------------------------------------------------
  // Variable constructor
  //
  // Initialices a Variable with the id. Links the read-only values
  // to the private ones to provide with more easy use of the class and avoid
  // external modifications
  // ---------------------------------------------------------------------------
  explicit Variable(const unsigned id);

  // ---------------------------------------------------------------------------
  // GetEnabledEdges
  //
  // Get only the enabled edges that connect to the node
  // ---------------------------------------------------------------------------
  std::vector<Edge*> GetEnabledEdges();

  // ---------------------------------------------------------------------------
  // AssignValue
  //
  // Sets assigned to true and value to the new value.
  // ---------------------------------------------------------------------------
  void AssignValue(const bool newValue);

  // ---------------------------------------------------------------------------
  // operator<<
  //
  // X{id}: [{value}|NOT_ASSIGNED]
  // ---------------------------------------------------------------------------
  friend std::ostream& operator<<(std::ostream& os, const Variable* var);
};

// =============================================================================
// Clause
//
// Class to represent a clause in the graph.
// Has a vector of neighbour edges that connects the clause with all variables
// that appear in it.
// =============================================================================
class Clause {
 public:
  const unsigned id;
  bool enabled;
  int trueLiterals = 0;

  std::vector<Edge*> allNeighbourEdges;

 public:
  // ---------------------------------------------------------------------------
  // Clause constructor
  //
  // Initialices a Node with an id. Links the read-only values
  // to the private ones to provide with more easy use of the class and avoid
  // external modifications
  // ---------------------------------------------------------------------------
  explicit Clause(const unsigned id);

  // ---------------------------------------------------------------------------
  // GetEnabledEdges
  //
  // Get only the enabled edges that connect to the node
  // ---------------------------------------------------------------------------
  std::vector<Edge*> GetEnabledEdges() const;

  // ---------------------------------------------------------------------------
  // Dissable
  //
  // Dissable the clause and all its neighbour edges
  // ---------------------------------------------------------------------------
  void Dissable();

  // ---------------------------------------------------------------------------
  // countTrueLiterals
  //
  // Return and store how many true literals has the clause
  // ---------------------------------------------------------------------------
  int countTrueLiterals();

  // ---------------------------------------------------------------------------
  // IsSAT
  //
  // Check that the clause contains an assigned variable that satisfies it
  // ---------------------------------------------------------------------------
  bool IsSAT() const;

  // ---------------------------------------------------------------------------
  // operator<<
  //
  // C{id}: N literals - [ENABLED|DISABLED]
  // ---------------------------------------------------------------------------
  friend std::ostream& operator<<(std::ostream& os, const Clause* c);
};

// =============================================================================
// Edge
//
// Each Edge represent a connection between a variable and a clause.
// Can store a survey value.
// Type is false if the variable appears negated in the clause, true
// otherwise.
// =============================================================================
class Edge {
 public:
  const bool type;
  bool enabled;

  Clause* clause;
  Variable* variable;

  double survey;

 public:
  // ---------------------------------------------------------------------------
  // Edge constructor
  //
  // Initialices an Edge with type, clause and variable. Links the read-only
  // values to the private ones to provide with more easy use of the class
  // and avoid external modifications
  // ---------------------------------------------------------------------------
  Edge(bool type, Clause* clause, Variable* variable);

  // ---------------------------------------------------------------------------
  // Dissable
  //
  // Dissable the edge
  // ---------------------------------------------------------------------------
  void Dissable();

  // ---------------------------------------------------------------------------
  // operator<<
  //
  // C{clau.id} <---> [¬]X{var.id} - [ENABLED|DISABLED] ({survey})
  // ---------------------------------------------------------------------------
  friend std::ostream& operator<<(std::ostream& os, const Edge* e);
};

// =============================================================================
// FactorGraph
//
// Graph representation of a CNF. Must be initialized with the content of a
// valid DIMACS CNF file.
// =============================================================================
class FactorGraph {
 public:
  std::vector<Variable*> variables;
  std::vector<Clause*> clauses;
  std::vector<Edge*> edges;

 public:
  const std::vector<std::string> SplitString(const std::string& s);

  // ---------------------------------------------------------------------------
  // FactorGraph constructor
  //
  // Build the Variables, Clauses and Edges of the CNF
  // ---------------------------------------------------------------------------
  explicit FactorGraph(std::ifstream& file);
  ~FactorGraph();

  // ---------------------------------------------------------------------------
  // Getters
  // ---------------------------------------------------------------------------
  std::vector<Variable*> GetUnassignedVariables();
  std::vector<Clause*> GetEnabledClauses();
  std::vector<Edge*> GetEnabledEdges();

  // ---------------------------------------------------------------------------
  // IsSat
  //
  // If all variables are asigned, check that all clauses have a variable that
  // satisfies it
  // ---------------------------------------------------------------------------
  bool IsSAT() const;

  // ---------------------------------------------------------------------------
  // storeVariableValues
  //
  // Store the variable values in a file to validatethe formula
  // ---------------------------------------------------------------------------
  bool storeVariableValues(const std::string& filePath);

  // ---------------------------------------------------------------------------
  // operator<<
  //
  // Assigned Variables: N/N - Satisfied Clauses: N/N
  // ---------------------------------------------------------------------------
  friend std::ostream& operator<<(std::ostream& os, FactorGraph* fg);
};
}  // namespace sat