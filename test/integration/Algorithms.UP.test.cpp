#include <catch2/catch.hpp>
#include <iostream>

// Project headders
#include <Algorithms.hpp>
#include <FactorGraph.hpp>
#include <Utils.hpp>

TEST_CASE("Algorithm - Unit Propagation (no unit clauses)", "[integration]") {
  std::ifstream file("./test/cnf/1.cnf");
  if (!file.is_open()) FAIL("ERROR: Can't open file ./test/cnf/1.cnf");
  sat::FactorGraph* graph = new sat::FactorGraph(file);
  file.close();

  sat::utils::RandomGen::setSeed(7357);

  bool result = sat::UnitPropagation(graph);

  CHECK(result);
};

TEST_CASE("Algorithm - Unit Propagation (1 propagation)", "[integration]") {
  std::ifstream file("./test/cnf/2.cnf");
  if (!file.is_open()) FAIL("ERROR: Can't open file ./test/cnf/2.cnf");
  sat::FactorGraph* graph = new sat::FactorGraph(file);
  file.close();

  sat::utils::RandomGen::setSeed(7357);

  bool result = sat::UnitPropagation(graph);

  CHECK(result);
};

TEST_CASE("Algorithm - Unit Propagation (2 propagation)", "[integration]") {
  std::ifstream file("./test/cnf/3.cnf");
  if (!file.is_open()) FAIL("ERROR: Can't open file ./test/cnf/3.cnf");
  sat::FactorGraph* graph = new sat::FactorGraph(file);
  file.close();

  sat::utils::RandomGen::setSeed(7357);

  bool result = sat::UnitPropagation(graph);

  CHECK(result);
};

TEST_CASE("Algorithm - Unit Propagation (2 unit clauses)", "[integration]") {
  std::ifstream file("./test/cnf/4.cnf");
  if (!file.is_open()) FAIL("ERROR: Can't open file ./test/cnf/4.cnf");
  sat::FactorGraph* graph = new sat::FactorGraph(file);
  file.close();

  sat::utils::RandomGen::setSeed(7357);

  bool result = sat::UnitPropagation(graph);

  CHECK(result);
};

TEST_CASE("Algorithm - Unit Propagation (contradiction 1)", "[integration]") {
  std::ifstream file("./test/cnf/2.cnf");
  if (!file.is_open()) FAIL("ERROR: Can't open file ./test/cnf/2.cnf");
  sat::FactorGraph* graph = new sat::FactorGraph(file);
  file.close();

  sat::utils::RandomGen::setSeed(7357);

  // Forcing variable contradiction
  graph->GetUnassignedVariables()[0]->AssignValue(false);

  bool result = sat::UnitPropagation(graph);

  REQUIRE_FALSE(result);
};

TEST_CASE("Algorithm - Unit Propagation (contradiction 2)", "[integration]") {
  std::ifstream file("./test/cnf/5.cnf");
  if (!file.is_open()) FAIL("ERROR: Can't open file ./test/cnf/5.cnf");
  sat::FactorGraph* graph = new sat::FactorGraph(file);
  file.close();

  sat::utils::RandomGen::setSeed(7357);

  bool result = sat::UnitPropagation(graph);

  REQUIRE_FALSE(result);
};