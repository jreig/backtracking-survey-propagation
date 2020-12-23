#include <dimacsreader.hpp>
#include <sstream>
#include <stdexcept>

namespace sat {

  DimacsReader::DimacsReader(const std::string& path) { m_file.open(path); }
  DimacsReader::~DimacsReader() { m_file.close(); }

  const std::vector<int> DimacsReader::getHeader() {
    std::string line;
    while (std::getline(m_file, line)) {
      const std::vector<std::string> tokens = splitString(line);

      // Ignore comments
      if (tokens[0] == "c") continue;

      if (tokens[0] == "p" && tokens[1] == "cnf") {
        // p cnf <n_variables> <n_clauses>
        std::vector<int> header{std::stoi(tokens[2]), std::stoi(tokens[3])};
        return header;
      }

      throw std::runtime_error("ERROR: Cannot get header of DIMACS file");
    }
  }

  const std::vector<int> DimacsReader::getNextClause() {
    std::vector<int> clause;

    std::string line;
    if (std::getline(m_file, line)) {
      const std::vector<std::string> tokens = splitString(line);

      for (const std::string& token : tokens) {
        if (token != "0") clause.push_back(std::stoi(token));
      }

      return clause;
    }

    throw std::runtime_error("ERROR: Cannot get next clause of DIMACS file");
  }

  const std::vector<std::string> DimacsReader::splitString(const std::string& s) {
    const char delimiter = ' ';
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);

    while (std::getline(tokenStream, token, delimiter)) {
      tokens.push_back(token);
    }
    return tokens;
  }
}  // namespace sat