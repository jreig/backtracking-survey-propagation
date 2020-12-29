#pragma once

#include <string>
#include <vector>

#include "clause.hpp"
#include "edge.hpp"
#include "variable.hpp"

namespace sat {
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
    explicit FactorGraph(const std::string& path);
    ~FactorGraph();

    std::vector<Variable*> getUnassignedVariables();
    std::vector<Clause*> getEnabledClauses();
    std::vector<Edge*> getEnabledEdges();

    bool isSAT() const;

    bool storeVariableValues(const std::string& filePath);

    // friend std::ostream& operator<<(std::ostream& os, FactorGraph* fg);
  };
}  // namespace sat