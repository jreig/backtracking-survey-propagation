#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Validator {
 public:
  const vector<string> splitString(const string& s);
  bool validateResult(const string& cnf, const string& resultFile);
};