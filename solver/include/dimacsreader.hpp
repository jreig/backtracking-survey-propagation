#pragma once

#include <fstream>
#include <string>
#include <vector>

namespace sat {
  // =============================================================================
  // DimacsReader
  //
  // Helper to read a CNF from a DIMACS file
  // =============================================================================
  class DimacsReader {
   public:
    explicit DimacsReader(const std::string& path);
    ~DimacsReader();

    const std::vector<int> getHeader();  // [<n_variables>, <n_clauses>]
    const std::vector<int> getNextClause();

   private:
    std::ifstream m_file;
    bool m_headerReaded = false;

    const std::vector<std::string> splitString(const std::string& s);
  };
}  // namespace sat