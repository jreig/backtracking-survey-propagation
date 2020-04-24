#include <catch2/catch.hpp>
#include <iostream>

// Project headders
#include <Algorithms.hpp>
#include <FactorGraph.hpp>
#include <Utils.hpp>

TEST_CASE("Algorithm - SID (solution found)", "[integration]") {
  std::ifstream file("./test/cnf/1.cnf");
  if (!file.is_open()) FAIL("ERROR: Can't open file ./test/cnf/1.cnf");
  sat::FactorGraph* graph = new sat::FactorGraph(file);
  file.close();

  sat::utils::randomGenerator.seed(7357);

  bool result = sat::SID(graph, 0.04f, {100, 0.001f}, {100, 100, 0.5f});

  REQUIRE(result);
  std::vector<sat::Variable*> variables = graph->GetAllVariables();
  CHECK_FALSE(variables[0]->value);
  CHECK_FALSE(variables[1]->value);
  CHECK_FALSE(variables[2]->value);
};
