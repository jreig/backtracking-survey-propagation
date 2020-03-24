#include <fstream>
#include <iostream>

#include <BacktrackingSP.hpp>
#include <FactorGraph.hpp>

using namespace std;

int main() {
  ifstream file("./examples/cnf-files/1.cnf");

  bsp::BacktrackingSP solver(0.001f, 10000);
  bsp::BSPResult result = solver.BSP(file);

  cout << "Result: " << result << endl;

  return 0;
}