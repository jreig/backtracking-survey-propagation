#include <Validator.hpp>

const vector<string> Validator::splitString(const string& s) {
  vector<string> tokens;

  const char delim = ' ';
  stringstream stream(s);
  string token;
  while (getline(stream, token, delim)) {
    tokens.push_back(token);
  }

  return tokens;
}

bool Validator::validateResult(const string& cnf, const string& resultPath) {
  unsigned int totalVariables = 0;
  unsigned int totalClauses = 0;
  unsigned int totalSATClauses = 0;

  // Read variables values from the result file
  vector<bool> varValues;
  ifstream file(resultPath);
  int v;
  while (file >> v) {
    varValues.push_back(v > 0);
  }

  // Read formula file and check every clause
  ifstream cnfFile(cnf);
  string line;
  while (getline(cnfFile, line)) {
    const vector<string> tokens = splitString(line);

    if (tokens[0] == "c")
      continue;
    else if (tokens[0] == "p" && tokens[1] == "cnf") {
      totalVariables = stoi(tokens[2]);
      totalClauses = stoi(tokens[3]);

      if (totalVariables != varValues.size()) {
        cout << "Missing variables values: " << totalVariables << "/"
             << varValues.size() << endl;
        return false;
      }
    } else {
      bool isClauseSAT = false;
      for (string varStr : tokens) {
        if (varStr != "0") {
          int var = stoi(varStr);
          bool varValue = var > 0;
          int varId = abs(var);
          if (varValue == varValues[varId - 1]) isClauseSAT = true;
        }
      }

      if (isClauseSAT) totalSATClauses++;
      // else
      //   cout << line << endl;
    }
  }

  if (totalClauses == totalSATClauses) return true;
  cerr << "Missing " << totalClauses - totalSATClauses << " clauses from "
       << totalClauses << endl;
  return false;
}