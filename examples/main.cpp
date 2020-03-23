#include <fstream>
#include <iostream>

#include <BacktrackingSP.hpp>
#include <FactorGraph.hpp>

using namespace std;

int main() {
  ifstream file("./examples/cnf-files/1.cnf");

  bsp::BacktrackingSP solver(file, 0.05f, 10);
  bsp::BSPResult result = solver.IsSAT();

  cout << "Result: " << result << endl;

  return 0;
}