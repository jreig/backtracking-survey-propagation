#include <catch2/catch.hpp>
#include <iostream>

// Project headders
#include <Algorithms.hpp>
#include <FactorGraph.hpp>
#include <Utils.hpp>

TEST_CASE("Algorithm - Walksat (solution found)", "[integration]") {
  std::ifstream file("./test/cnf/6.cnf");
  if (!file.is_open()) FAIL("ERROR: Can't open file ./test/cnf/6.cnf");
  sat::FactorGraph* graph = new sat::FactorGraph(file);
  file.close();

  sat::utils::randomGenerator.seed(7357);

  bool result = sat::Walksat(graph, {100, 100, 0.5f});

  REQUIRE(result);
  for (sat::Variable* var : graph->GetAllVariables()) {
    CHECK(var->value);
  }
};

TEST_CASE("Algorithm - Walksat (solution NOT found)", "[integration]") {
  std::ifstream file("./test/cnf/7.cnf");
  if (!file.is_open()) FAIL("ERROR: Can't open file ./test/cnf/7.cnf");
  sat::FactorGraph* graph = new sat::FactorGraph(file);
  file.close();

  sat::utils::randomGenerator.seed(7357);

  bool result = sat::Walksat(graph, {100, 100, 0.5f});

  REQUIRE_FALSE(result);
};
