#pragma once

#include <vector>

namespace sat {

  class Edge;  // Forward declaration

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
    explicit Variable(const unsigned id);

    std::vector<Edge*> getEnabledEdges();
    void assignValue(const bool newValue);
    void unassign();

    // friend std::ostream& operator<<(std::ostream& os, const Variable* var);
  };

}  // namespace sat