#include <factorgraph/edge.hpp>

namespace sat {
  Edge::Edge(bool type, Clause* clause, Variable* variable)
      : type(type), enabled(true), clause(clause), variable(variable) {}

  void Edge::dissable() { enabled = false; }

  //   std::ostream& operator<<(std::ostream& os, const Edge* e) {
  //     os << "C" << e->clause->id << " <---> ";
  //     os << (e->type ? " X" : "¬X") << e->variable->id;
  //     os << " - " << (e->enabled ? "ENABLED " : "DISABLED");
  //     // os << " (" << std::fixed << std::setprecision(10) << e->survey << ")";
  //     os << " (" << e->survey << ")";
  //     return os;
  //   }
}  // namespace sat