#include <catch2/catch.hpp>
#include <iostream>

// Project headders
#include <Algorithms.hpp>
#include <FactorGraph.hpp>
#include <Utils.hpp>

TEST_CASE("Algorithm - Survey Propagation (converge)", "[integration]") {
  std::ifstream file("./test/cnf/1.cnf");
  if (!file.is_open()) FAIL("ERROR: Can't open file ./test/cnf/1.cnf");
  sat::FactorGraph* graph = new sat::FactorGraph(file);
  file.close();

  sat::utils::randomGenerator.seed(7357);

  bool result = sat::SurveyPropagation(graph, {100, 0.001f});

  for (sat::Edge* edge : graph->GetEnabledEdges()) {
    REQUIRE(edge->survey == 0.0f);
  }

  REQUIRE(result);
};

TEST_CASE("Algorithm - Survey Propagation (unconverge 1)", "[integration]") {
  std::ifstream file("./test/cnf/1.cnf");
  if (!file.is_open()) FAIL("ERROR: Can't open file ./test/cnf/1.cnf");
  sat::FactorGraph* graph = new sat::FactorGraph(file);
  file.close();

  sat::utils::randomGenerator.seed(7357);

  bool result = sat::SurveyPropagation(graph, {2, 0.001f});

  REQUIRE(result == false);
};

TEST_CASE("Algorithm - Survey Propagation (unconverge 2)", "[integration]") {
  std::ifstream file("./test/cnf/1.cnf");
  if (!file.is_open()) FAIL("ERROR: Can't open file ./test/cnf/1.cnf");
  sat::FactorGraph* graph = new sat::FactorGraph(file);
  file.close();

  sat::utils::randomGenerator.seed(7357);

  bool result = sat::SurveyPropagation(graph, {100, 0.0f});

  REQUIRE(result == false);
};