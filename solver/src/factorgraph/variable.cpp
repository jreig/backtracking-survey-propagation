#include <factorgraph/edge.hpp>
#include <factorgraph/variable.hpp>

namespace sat {
  Variable::Variable(const unsigned id) : id(id), assigned(false) {}

  std::vector<Edge*> Variable::getEnabledEdges() {
    std::vector<Edge*> enabledNeigbours;
    for (Edge* edge : allNeighbourEdges) {
      if (edge->enabled) enabledNeigbours.push_back(edge);
    }
    return enabledNeigbours;
  }

  void Variable::assignValue(const bool newValue) {
    value = newValue;
    assigned = true;
  }

  void Variable::unassign() { assigned = false; }

  //   std::ostream& operator<<(std::ostream& os, const Variable* var) {
  //     os << "X" << var->id << ": " << (var->assigned ? (var->value ? "true" : "false") : "NOT_ASSIGNED");
  //     return os;
  //   }
}  // namespace sat