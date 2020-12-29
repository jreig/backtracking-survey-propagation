#include <factorgraph/clause.hpp>
#include <factorgraph/edge.hpp>

namespace sat {
  Clause::Clause(const unsigned id) : id(id), enabled(true) {}

  std::vector<Edge*> Clause::getEnabledEdges() const {
    std::vector<Edge*> enabledNeigbours;
    for (Edge* edge : allNeighbourEdges) {
      if (edge->enabled) enabledNeigbours.push_back(edge);
    }
    return enabledNeigbours;
  }

  void Clause::dissable() {
    enabled = false;

    for (Edge* edge : allNeighbourEdges) {
      if (edge->enabled) edge->dissable();
    }
  }

  int Clause::countTrueLiterals() {
    trueLiterals = 0;
    for (Edge* edge : allNeighbourEdges) {
      if (edge->variable->assigned && edge->type == edge->variable->value) trueLiterals++;
    }

    return trueLiterals;
  }

  bool Clause::isSAT() const {
    for (Edge* edge : allNeighbourEdges) {
      if (edge->variable->assigned && edge->type == edge->variable->value) return true;
    }

    return false;
  }

  //   std::ostream& operator<<(std::ostream& os, const Clause* c) {
  //     os << "C" << c->id << ": ";
  //     os << c->GetEnabledEdges().size() << " literals - ";
  //     os << (c->enabled ? "ENABLED" : "DISABLED");
  //     return os;
  //   }
}  // namespace sat