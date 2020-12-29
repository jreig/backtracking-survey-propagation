#include <dimacsreader.hpp>
#include <factorgraph/clause.hpp>
#include <factorgraph/edge.hpp>
#include <factorgraph/factorgraph.hpp>
#include <factorgraph/variable.hpp>
#include <fstream>

namespace sat {
  FactorGraph::FactorGraph(const std::string& path) {
    DimacsReader reader(path);

    // Get structure info from dimacs file
    std::vector<int> header = reader.getHeader();
    unsigned int totalVariables = header[0];
    unsigned int totalClauses = header[1];

    // Create variables
    for (unsigned i = 0; i < totalVariables; i++) {
      Variable* variable = new Variable(i + 1);
      variables.push_back(variable);
    }

    // Create clauses
    for (unsigned i = 0; i < totalClauses; i++) {
      // Initialize new empty clause
      Clause* clause = new Clause(i + 1);
      clauses.push_back(clause);

      // Read clause variables from dimacs file
      std::vector<int> clauseVars = reader.getNextClause();
      for (const int varValue : clauseVars) {
        const int varIndex = std::abs(varValue) - 1;

        // Create edge
        bool edgeType = varValue > 0;
        Variable* variable = variables[varIndex];
        Edge* edge = new Edge(edgeType, clause, variable);
        edges.push_back(edge);

        // Connect clause and variables with the new edge
        clause->allNeighbourEdges.push_back(edge);
        variable->allNeighbourEdges.push_back(edge);
      }
    }
  }

  FactorGraph::~FactorGraph() {
    for (Clause* clause : clauses) delete clause;
    for (Variable* variable : variables) delete variable;
    for (Edge* edge : edges) delete edge;
  }

  std::vector<Variable*> FactorGraph::getUnassignedVariables() {
    std::vector<Variable*> unassignedVariables;
    for (Variable* variable : variables) {
      if (!variable->assigned) unassignedVariables.push_back(variable);
    }
    return unassignedVariables;
  }

  std::vector<Clause*> FactorGraph::getEnabledClauses() {
    std::vector<Clause*> enabledClauses;
    for (Clause* clause : clauses) {
      if (clause->enabled) enabledClauses.push_back(clause);
    }
    return enabledClauses;
  }

  std::vector<Edge*> FactorGraph::getEnabledEdges() {
    std::vector<Edge*> enabledEdges;
    for (Edge* edge : edges) {
      if (edge->enabled) enabledEdges.push_back(edge);
    }
    return enabledEdges;
  }

  bool FactorGraph::isSAT() const {
    for (Clause* clause : clauses) {
      if (!clause->isSAT()) return false;
    }

    return true;
  }

  bool FactorGraph::storeVariableValues(const std::string& filePath) {
    std::ofstream resultFile;
    resultFile.open(filePath);
    for (Variable* var : variables) {
      resultFile << var->value << "\n";
    }
    resultFile.close();
    return true;
  }

  //   std::ostream& operator<<(std::ostream& os, FactorGraph* fg) {
  //     unsigned totalVariables = fg->variables.size();
  //     unsigned assignedVariables = totalVariables - fg->GetUnassignedVariables().size();

  //     unsigned totalClauses = fg->clauses.size();
  //     unsigned satClauses = totalClauses - fg->GetEnabledClauses().size();

  //     os << "Assigned Variables: ";
  //     os << assignedVariables << "/" << totalVariables;
  //     os << " - Satisfied Clauses: ";
  //     os << satClauses << "/" << totalClauses;
  //     return os;
  //   }
}  // namespace sat