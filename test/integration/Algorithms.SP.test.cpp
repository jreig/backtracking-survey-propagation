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

  sat::utils::RandomGen::setSeed(7357);

  sat::SPResult result = sat::SurveyPropagation(graph);

  for (sat::Edge* edge : graph->GetEnabledEdges()) {
    CHECK(edge->survey < 0.0001);
  }

  REQUIRE(result.converged);
  REQUIRE(result.iterations == 3);
};