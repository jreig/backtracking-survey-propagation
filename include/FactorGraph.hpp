#pragma once

#include <fstream>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

namespace bsp {

enum NodeType { CLAUSULE, VARIABLE };
enum EdgeType { POSITIVE, NEGATIVE };

// Declarations to avoid circular dependencies
class FactorGraph;
class Node;
class Edge;

// =============================================================================
// Node
//
// Each Node represents a variable or a clausule of the Factor Graph and has a
// list of its neighboring edges, that is, those edges that connect to the node.
// If the node is not enabled it can't be accessed through connecting edges.
// =============================================================================
class Node {
  friend class FactorGraph;

 public:
  const unsigned id;
  const NodeType type;

 protected:
  std::vector<Edge*> _allNeighbourEdges;

 public:
  // ---------------------------------------------------------------------------
  // Node constructor
  //
  // - id: Id of the node in base 0
  // - type: Type of the node (clausule or variable)
  // ---------------------------------------------------------------------------
  Node(const unsigned id, const NodeType type);

  // ---------------------------------------------------------------------------
  // AddNeighbourEdge
  //
  // Add an edge to the neighbour edges list, no matter if enabled or not
  // ---------------------------------------------------------------------------
  void AddNeighbourEdge(Edge* edge);

  // ---------------------------------------------------------------------------
  // GetNeighbourEdges
  //
  // Get only the enabled edges that connect to the node
  // ---------------------------------------------------------------------------
  std::vector<Edge*> GetNeighbourEdges() const;
};

// =============================================================================
// Variable
//
// Class to represent a variable in the graph, it's a type of Node.
// Can be assigned with a value (true, false).
// =============================================================================
class Variable : public Node {
  friend class FactorGraph;

 public:
  // Read-only variables pointing to its private values
  // The linking is done in the constructor
  const bool& assigned;
  const bool& value;

 private:
  // Can only be enabled/disabled by FactorGraph
  bool _assigned = false;
  bool _value = false;

 public:
  // ---------------------------------------------------------------------------
  // Variable constructor
  //
  // Initialices a Node with the id and type VARIABLE. Link the read-only values
  // to the private ones to provide with more easy use of the class and avoid
  // external modifications
  // ---------------------------------------------------------------------------
  explicit Variable(const unsigned id);

  // ---------------------------------------------------------------------------
  // operator<<
  //
  // X{id + 1}: [{value}|NOT_ASSIGNED]
  // ---------------------------------------------------------------------------
  friend std::ostream& operator<<(std::ostream& os, const Variable* var);
};

// =============================================================================
// Clausule
//
// Class to represent a clausule in the graph, it's a type of Node.
// Can be evaluated.
// =============================================================================
class Clausule : public Node {
  friend class FactorGraph;

 public:
  // Read-only variable pointing to its private value
  // The linking is done in the constructor
  const bool& enabled;

 private:
  // Can only be enabled/disabled by FactorGraph
  bool _enabled = true;

 public:
  // ---------------------------------------------------------------------------
  // Clausule constructor
  //
  // Initialices a Node with an id and type CLAUSULE.
  // ---------------------------------------------------------------------------
  explicit Clausule(const unsigned id);

  // ---------------------------------------------------------------------------
  // operator<<
  //
  // {id char}: N variables - [ENABLED|DISABLED]
  // ---------------------------------------------------------------------------
  friend std::ostream& operator<<(std::ostream& os, const Clausule* c);
};

// =============================================================================
// Edge
//
// Each Edge represent a connection between a variable and a clausule.
// It's negative if the variable appears negated in the clausule, positive
// otherwise.
// =============================================================================
class Edge {
  friend class FactorGraph;

 public:
  EdgeType type;
  Clausule* clausule;
  Variable* variable;
  float survey;

  // Read-only variable pointing to its private value
  // The linking is done in the constructor
  const bool& enabled;

 private:
  // Can only be enabled/disabled by FactorGraph
  bool _enabled = true;

 public:
  // ---------------------------------------------------------------------------
  // Edge constructor
  //
  // - type: Type of the edge. Negative means that the variable appears negated
  //         in the clausule.
  // - clausule: Pointer to one node of the edge
  // - variable: Pointer to the other node of the edge
  // ---------------------------------------------------------------------------
  Edge(EdgeType type, Clausule* clausule, Variable* variable);

  // ---------------------------------------------------------------------------
  // operator<<
  //
  // {clau.id char} <---> [¬]X{var.id + 1} - [ENABLED|DISABLED]
  // ---------------------------------------------------------------------------
  friend std::ostream& operator<<(std::ostream& os, const Edge* e);
};

// =============================================================================
// BacktrackingStep
//
// Store the modifications done by an assigment to be able to revert them.
// =============================================================================
struct BactrackingStep {
  Variable* variable;
  std::vector<Clausule*> clausules;
  std::vector<Edge*> edges;
};

// =============================================================================
// FactorGraph
//
// Graph representation of a CNF. Must be initialized with the content of a
// valid DIMACS CNF file.
// =============================================================================
class FactorGraph {
 public:
  // Read-only variable pointing to its private value
  // The linking is done in the constructor
  const unsigned& NTotalClausules;
  const unsigned& NTotalVariables;
  const unsigned& NTotalEdges;

 private:
  unsigned _NTotalClausules = 0;
  unsigned _NTotalVariables = 0;
  unsigned _NTotalEdges = 0;

  std::vector<Clausule*> _clausules;
  std::vector<Variable*> _variables;
  std::vector<Edge*> _edges;

  std::stack<BactrackingStep> _bactrackingSteps;

 public:
  // ---------------------------------------------------------------------------
  // FactorGraph constructor
  //
  // - file: Open ifstream of the DIMACS file with a valid CNF
  // ---------------------------------------------------------------------------
  explicit FactorGraph(std::ifstream& file);
  ~FactorGraph();

  // ---------------------------------------------------------------------------
  // Getters
  // ---------------------------------------------------------------------------
  std::vector<Clausule*> GetClausules() const;
  std::vector<Variable*> GetVariables() const;
  std::vector<Edge*> GetEdges() const;

  // ---------------------------------------------------------------------------
  // operator<<
  //
  // Assigned Variables: N/N - Satisfied Clausules: N/N
  // ---------------------------------------------------------------------------
  friend std::ostream& operator<<(std::ostream& os, const FactorGraph* fg);

  // ---------------------------------------------------------------------------
  // AssignVariable
  //
  // Assign a value to a variable and clean the graph, which means to dissable
  // the clauses satisfied by this assignment and to reduce the clauses
  // (dissable edges) that involve the assigned variable with opposite literal.
  // ---------------------------------------------------------------------------
  void AssignVariable(Variable* variable, bool value);

  // ---------------------------------------------------------------------------
  // RevertLastAssigment
  //
  // Unassign the last assigned variable and enable all clausules and edges
  // that were disabled because of the assigment
  // ---------------------------------------------------------------------------
  void RevertLastAssigment();
};
}  // namespace bsp