#include <catch2/catch.hpp>
#include <iostream>

// Project headders
#include <Algorithms.hpp>
#include <FactorGraph.hpp>
#include <Utils.hpp>

TEST_CASE("Algorithm - SID (walksat solution)", "[integration]") {
  std::ifstream file("./test/cnf/1.cnf");
  if (!file.is_open()) FAIL("ERROR: Can't open file ./test/cnf/1.cnf");
  sat::FactorGraph* graph = new sat::FactorGraph(file);
  file.close();

  sat::utils::RandomGen::setSeed(7357);

  sat::SIDResult result = sat::SID(graph, 0.0);

  REQUIRE(result.SAT);
  REQUIRE(result.totalSPIterations == 5);

  std::vector<sat::Variable*> variables = graph->GetAllVariables();
  CHECK_FALSE(variables[0]->value);
  CHECK(variables[1]->value);
  CHECK(variables[2]->value);
};
