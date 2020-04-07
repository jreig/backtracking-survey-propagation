#include <BSPSolver.hpp>
#include <FactorGraph.hpp>
#include <fstream>
#include <iostream>

using namespace std;
using namespace bsp;

int main() {
  BSPSolver solver(BSPSolver::DEFAULT_EPSILON,
                   BSPSolver::DEFAULT_MAX_ITERATIONS, 1234);
  BSPResult result = solver.SID("./examples/cnf-files/1.cnf");

  cout << "Result: " << result << endl;

  return 0;
}