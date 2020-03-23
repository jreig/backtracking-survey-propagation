#include <FactorGraph.hpp>

using namespace std;

namespace bsp {
// -----------------------------------------------------------------------------
// SplitString
//
// Splits the string into token by the given delimiter
// Return a vector with the tokens
// -----------------------------------------------------------------------------
const vector<string> SplitString(const string& s, const char delim = '\n') {
  stringstream stream(s);
  vector<string> tokens;

  string token;
  while (getline(stream, token, delim)) {
    tokens.push_back(token);
  }

  return tokens;
}

// -----------------------------------------------------------------------------
// Node class
// -----------------------------------------------------------------------------
Node::Node(const int id, const NodeType type) : id(id), type(type){};

void Node::AddNeighbourEdge(Edge* edge) { neighbourEdges.push_back(edge); }

// -----------------------------------------------------------------------------
// Edge class
// -----------------------------------------------------------------------------
Edge::Edge(EdgeType type, Node* clausule, Node* literal)
    : type(type), clausule(clausule), literal(literal), survey(0) {}

// -----------------------------------------------------------------------------
// FactorGraph class
// -----------------------------------------------------------------------------
FactorGraph::FactorGraph(const string& dimacs) {
  // Split the dimacs file content into lines
  const vector<string> lines = SplitString(dimacs);

  bool configured = false;
  int currentClausuleId = 0;
  for (const string& line : lines) {
    // Split the lines into tokens
    const vector<string> tokens = SplitString(line, ' ');

    // If first token is a 'c' ignore the line because is a comment
    if (tokens[0] == "c") continue;

    // If first token is a 'p' and second is 'cnf',
    // the line contains the number of literals (3rd) and clausules (4th)
    else if (tokens[0] == "p" && tokens[1] == "cnf") {
      _NLiterals = stoi(tokens[2]);
      _NClausules = stoi(tokens[3]);

      // Create literals
      for (int i = 0; i < _NLiterals; i++) {
        Node* literal = new Node(i, LITERAL);
        literals.push_back(literal);
      }

      // Create clausules
      for (int i = 0; i < _NClausules; i++) {
        Node* clausule = new Node(i, CLAUSULE);
        clausules.push_back(clausule);
      }

      configured = true;
    }

    // Every other line should be a clausule containing literals
    else {
      if (configured) {
        for (const string& token : tokens) {
          if (token != "0") {
            const int literalValue = stoi(token);
            // literals start from 1 and indices from 0
            const int literalId = abs(literalValue) - 1;

            // Create an edge
            EdgeType type = (literalValue < 0) ? NEGATIVE : POSITIVE;
            Node* clausule = clausules[currentClausuleId];
            Node* literal = literals[literalId];

            Edge* edge = new Edge(type, clausule, literal);
            edges.push_back(edge);

            // Connect clausules and literals with the edge
            clausule->AddNeighbourEdge(edge);
            literal->AddNeighbourEdge(edge);
          }
        }

        // Next clausule
        currentClausuleId += 1;
      }
    }
  }
}

FactorGraph::~FactorGraph() {
  for (const Node* clausule : clausules) delete clausule;
  for (const Node* literal : literals) delete literal;
  for (const Edge* edge : edges) delete edge;
}

void FactorGraph::Print() {
  cout << "FactorGraph Edges:" << endl;
  for (const Edge* edge : edges) {
    cout << (char)(edge->clausule->id + 'a') << " ---> "
         << ((edge->type == NEGATIVE) ? "¬" : "") << edge->literal->id + 1
         << endl;
  }
}

}  // namespace bsp