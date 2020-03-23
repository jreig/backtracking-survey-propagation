#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace bsp {

enum NodeType { CLAUSULE, LITERAL };
enum EdgeType { POSITIVE, NEGATIVE };

// Declarations to avoid circular dependencies
class Node;
class Edge;

// -----------------------------------------------------------------------------
// Node
//
// Each Node represents a literal or a clausule of the Factor Graph and has a
// list of its neighboring edges, that is, those edges that connect to the node.
// -----------------------------------------------------------------------------
class Node {
 public:
  const int id;
  const NodeType type;
  std::vector<Edge*> neighbourEdges;

  explicit Node(const int id, const NodeType type);

  void AddNeighbourEdge(Edge* edge);
};

// -----------------------------------------------------------------------------
// Edge
//
// Each Edge represent a connection between a literal and a clausule.
// It's negative id the literal appears negated in the clausule, positive
// otherwise.
// -----------------------------------------------------------------------------
class Edge {
 public:
  EdgeType type;
  Node* clausule;
  Node* literal;
  float survey;

  Edge(EdgeType type, Node* clausule, Node* literal);
};

// -----------------------------------------------------------------------------
// FactorGraph
//
// Graph representation of a CNF. Must be initialized with the content of a
// valid DIMACS CNF file.
// -----------------------------------------------------------------------------
class FactorGraph {
 private:
  int _NLiterals;
  int _NClausules;

 public:
  std::vector<Node*> clausules;
  std::vector<Node*> literals;
  std::vector<Edge*> edges;

  explicit FactorGraph(std::ifstream& file);
  ~FactorGraph();

  void Print();
};
}  // namespace bsp