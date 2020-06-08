#pragma once

#include <fstream>
#include <iostream>
#include <vector>

namespace sat {

// Declarations to avoid circular dependencies
struct AssignmentStep;
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
  friend class FactorGraph;

 public:
  const unsigned id;
  long double evalValue;

  // Read-only variables pointing to its private values
  // The linking is done in the constructor
  const bool& assigned;
  const bool& value;

 private:
  bool _assigned = false;
  bool _value = false;

  std::vector<Edge*> _allNeighbourEdges;

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
  // Sets _assigned to true and _value to the new value. An AssigmentStep can be
  // provided to keep track of the assignments.
  // ---------------------------------------------------------------------------
  void AssignValue(const bool newValue,
                   AssignmentStep* assignmentStep = nullptr);

  // ---------------------------------------------------------------------------
  // Unassign
  //
  // Sets _assigned to false. Used to revert an assignment when backtracking
  // ---------------------------------------------------------------------------
  inline void Unassign() { _assigned = false; }

  // ---------------------------------------------------------------------------
  // operator<<
  //
  // X{id}: [{value}|NOT_ASSIGNED]
  // ---------------------------------------------------------------------------
  // friend std::ostream& operator<<(std::ostream& os, const Variable* var);
};

// =============================================================================
// Clause
//
// Class to represent a clause in the graph.
// Has a vector of neighbour edges that connects the clause with all variables
// that appear in it.
// =============================================================================
class Clause {
  friend class FactorGraph;

 public:
  const unsigned id;

  // Read-only variable pointing to its private value
  // The linking is done in the constructor
  const bool& enabled;

 private:
  bool _enabled = true;

  std::vector<Edge*> _allNeighbourEdges;

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
  std::vector<Edge*> GetEnabledEdges();

  // ---------------------------------------------------------------------------
  // Dissable
  //
  // Dissable the clause and all its neighbour edges
  // ---------------------------------------------------------------------------
  void Dissable(AssignmentStep* assignmentStep = nullptr);

  // ---------------------------------------------------------------------------
  // Enable
  //
  // Enable ONLY the clause, not its neighbour edges
  // ---------------------------------------------------------------------------
  inline void Enable() { _enabled = true; }

  // ---------------------------------------------------------------------------
  // IsSAT
  //
  // Check that the clause contains an assigned variable that satisfies it
  // ---------------------------------------------------------------------------
  bool IsSAT() const;

  // ---------------------------------------------------------------------------
  // operator<<
  //
  // C{id}: N variables - [ENABLED|DISABLED]
  // ---------------------------------------------------------------------------
  // friend std::ostream& operator<<(std::ostream& os, const Clause* c);
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
  Clause* clause;
  Variable* variable;

  long double survey;

  // Read-only variable pointing to its private value
  // The linking is done in the constructor
  const bool& enabled;

 private:
  bool _enabled = true;

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
  void Dissable(AssignmentStep* assignmentStep = nullptr);

  // ---------------------------------------------------------------------------
  // Enable
  //
  // Enable the edge
  // ---------------------------------------------------------------------------
  inline void Enable() { _enabled = true; }

  // ---------------------------------------------------------------------------
  // operator<<
  //
  // C{clau.id} <---> [¬]X{var.id} - [ENABLED|DISABLED] ({survey})
  // ---------------------------------------------------------------------------
  friend std::ostream& operator<<(std::ostream& os, const Edge* e);
};

// =============================================================================
// AssignmentStep
//
// Store the modifications done to the graph to be able to revert them.
// =============================================================================
struct AssignmentStep {
  std::vector<Variable*> variables;
  std::vector<Clause*> clauses;
  std::vector<Edge*> edges;
};

// =============================================================================
// FactorGraph
//
// Graph representation of a CNF. Must be initialized with the content of a
// valid DIMACS CNF file.
// =============================================================================
class FactorGraph {
 private:
  std::vector<Variable*> _variables;
  std::vector<Clause*> _clauses;
  std::vector<Edge*> _edges;
  std::vector<AssignmentStep*> _assignmentSteps;

 public:
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
  inline std::vector<Variable*> GetAllVariables() { return _variables; }
  inline std::vector<Clause*> GetAllClauses() { return _clauses; }
  inline std::vector<Edge*> GetAllEdges() { return _edges; }

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
  // StoreAssignmentStep
  //
  // Store an assignment step
  // ---------------------------------------------------------------------------
  void StoreAssignmentStep(AssignmentStep* step);

  // ---------------------------------------------------------------------------
  // RevertLastAssigment
  //
  // Unassign the last assigned variables and enable all clauses and edges
  // that were disabled.
  // ---------------------------------------------------------------------------
  void RevertLastAssigment();

  // ---------------------------------------------------------------------------
  // operator<<
  //
  // Assigned Variables: N/N - Satisfied Clauses: N/N
  // ---------------------------------------------------------------------------
  friend std::ostream& operator<<(std::ostream& os, FactorGraph* fg);
};
}  // namespace sat