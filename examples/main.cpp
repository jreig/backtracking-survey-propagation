#include <iostream>

#include <BacktrackingSP.hpp>

int main() {
  bsp::BacktrackingSP solver;
  const bool result = solver.IsSAT(NULL /*Not needed in the example code*/);
  std::cout << "Result: " << result << std::endl;
  return 0;
}