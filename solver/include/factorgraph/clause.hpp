#pragma once

#include <vector>

namespace sat {

  class Edge;  // Forward declaration

  class Clause {
   public:
    const unsigned id;
    bool enabled;
    int trueLiterals = 0;

    std::vector<Edge*> allNeighbourEdges;

   public:
    explicit Clause(const unsigned id);

    std::vector<Edge*> getEnabledEdges() const;
    void dissable();
    int countTrueLiterals();
    bool isSAT() const;

    // friend std::ostream& operator<<(std::ostream& os, const Clause* c);
  };
}  // namespace sat