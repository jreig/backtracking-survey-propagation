#pragma once

#include "clause.hpp"
#include "variable.hpp"

namespace sat {

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
    Edge(bool type, Clause* clause, Variable* variable);

    void dissable();

    // friend std::ostream& operator<<(std::ostream& os, const Edge* e);
  };
}  // namespace sat