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

  sat::utils::randomGenerator.seed(7357);

  sat::AssignmentStep* step = new sat::AssignmentStep();
  bool result = sat::UnitPropagation(graph, step);

  CHECK(result);
  CHECK(step->variables.size() == 0);
  CHECK(step->clauses.size() == 0);
  CHECK(step->edges.size() == 0);
};

TEST_CASE("Algorithm - Unit Propagation (1 propagation)", "[integration]") {
  std::ifstream file("./test/cnf/2.cnf");
  if (!file.is_open()) FAIL("ERROR: Can't open file ./test/cnf/2.cnf");
  sat::FactorGraph* graph = new sat::FactorGraph(file);
  file.close();

  sat::utils::randomGenerator.seed(7357);

  sat::AssignmentStep* step = new sat::AssignmentStep();
  bool result = sat::UnitPropagation(graph, step);

  CHECK(result);
  CHECK(step->variables.size() == 1);
  CHECK(step->clauses.size() == 2);
  CHECK(step->edges.size() == 5);
};

TEST_CASE("Algorithm - Unit Propagation (2 propagation)", "[integration]") {
  std::ifstream file("./test/cnf/3.cnf");
  if (!file.is_open()) FAIL("ERROR: Can't open file ./test/cnf/3.cnf");
  sat::FactorGraph* graph = new sat::FactorGraph(file);
  file.close();

  sat::utils::randomGenerator.seed(7357);

  sat::AssignmentStep* step = new sat::AssignmentStep();
  bool result = sat::UnitPropagation(graph, step);

  CHECK(result);
  CHECK(step->variables.size() == 2);
  CHECK(step->clauses.size() == 4);
  CHECK(step->edges.size() == 9);
};

TEST_CASE("Algorithm - Unit Propagation (2 unit clauses)", "[integration]") {
  std::ifstream file("./test/cnf/4.cnf");
  if (!file.is_open()) FAIL("ERROR: Can't open file ./test/cnf/4.cnf");
  sat::FactorGraph* graph = new sat::FactorGraph(file);
  file.close();

  sat::utils::randomGenerator.seed(7357);

  sat::AssignmentStep* step = new sat::AssignmentStep();
  bool result = sat::UnitPropagation(graph, step);

  CHECK(result);
  CHECK(step->variables.size() == 2);
  CHECK(step->clauses.size() == 5);
  CHECK(step->edges.size() == 10);
};

TEST_CASE("Algorithm - Unit Propagation (contradiction 1)", "[integration]") {
  std::ifstream file("./test/cnf/2.cnf");
  if (!file.is_open()) FAIL("ERROR: Can't open file ./test/cnf/2.cnf");
  sat::FactorGraph* graph = new sat::FactorGraph(file);
  file.close();

  sat::utils::randomGenerator.seed(7357);

  // Forcing variable contradiction
  graph->GetUnassignedVariables()[0]->AssignValue(false);

  sat::AssignmentStep* step = new sat::AssignmentStep();
  bool result = sat::UnitPropagation(graph, step);

  REQUIRE_FALSE(result);
};

TEST_CASE("Algorithm - Unit Propagation (contradiction 2)", "[integration]") {
  std::ifstream file("./test/cnf/5.cnf");
  if (!file.is_open()) FAIL("ERROR: Can't open file ./test/cnf/5.cnf");
  sat::FactorGraph* graph = new sat::FactorGraph(file);
  file.close();

  sat::utils::randomGenerator.seed(7357);

  sat::AssignmentStep* step = new sat::AssignmentStep();
  bool result = sat::UnitPropagation(graph, step);

  REQUIRE_FALSE(result);
};