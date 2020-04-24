// #pragma once

// #include <random>
// #include <string>

// // Project headers
// #include <FactorGraph.hpp>

// namespace sat {

// // Posible result states
// enum BSPResult { SAT, UNSAT, UNCONVERGED, CONVERGED, ERROR };

// //
// -----------------------------------------------------------------------------
// // SATSolver
// //
// // This class contains the Backtracking Survey Propagation algorithm (BSP)
// // and the Survey Inspired Decimation algorithm (SID), both based on SP.
// // Both algorithm take a DIMACS file containing a CNF as argument and check
// // if it is SAT or UNSAT.
// //
// -----------------------------------------------------------------------------
// class SATSolver {
//  public:
//   // Static constants for default initialization parameters
//   static const float DEFAULT_EPSILON;
//   static const int DEFAULT_MAX_ITERATIONS;
//   static const int RANDOM_SEED;

//  private:
//   std::mt19937 _randomGenerator;

//   // SP parameters
//   float _epsilon;
//   int _maxIterations;

//   // Factor graph where to save the CNF and be able to run the algorithms
//   FactorGraph* _graph;

//  public:
//   //
//   ---------------------------------------------------------------------------
//   // SATSolver constructor
//   //
//   // - epsilon: Error threshold for SP convergence checking.
//   // - maxIteration: Maximum amount of iterations to converge in SP.
//   // - seed: Seed for the random generator (for reproducibility).
//   //
//   ---------------------------------------------------------------------------
//   SATSolver(float epsilon, int maxIterations, int seed);
//   ~SATSolver();

//   //
//   ---------------------------------------------------------------------------
//   // BSP - Backtracking Survey Propagation Algorithm
//   //
//   // TODO
//   //
//   ---------------------------------------------------------------------------
//   BSPResult BSP(const std::string& path);

//   //
//   ---------------------------------------------------------------------------
//   // SID - Survey Inspired Decimaion Algorithm
//   //
//   // Params:
//   //  - path: path to the DIMACS file.
//   //
//   // Algorithm:
//   //  1 - Build a factor graph from a DIMACS file.
//   //  2 - Run SP. If does not converge, return UNCONVERGED.
//   //  3 - Found and assign a variable using decimation.
//   //  4 - Repeat 2-3 until all variables are assigned and return SAT.
//   //      If a contradiction is found, return UNSAT.
//   //
//   ---------------------------------------------------------------------------
//   BSPResult SID(const std::string& path);

//   //
//   ---------------------------------------------------------------------------
//   // SP - Survey Propagation Algorithm
//   //
//   // Param:
//   // - graph: FactorGraph encoding the CNF.
//   //
//   // Algorithm:
//   //  1 - Random initialization of survey values.
//   //  2 - Order randomly the set of edges.
//   //  3 - Update the survey value of each edge.
//   //  4 - Repeat 2-3 until all surveys converge and return CONVERGED.
//   //      If maximum iterations are reached, return UNCONVERGED.
//   //
//   ---------------------------------------------------------------------------
//   BSPResult SP(FactorGraph* graph);

//  private:
//   //
//   ---------------------------------------------------------------------------
//   // StoreFactorGraph
//   //
//   // Creates a new FactorGraph (deleting the prevoius stored one)
//   // from a DIMACS file containing a CNF and store it.
//   // Returns true if the graph is correctly created, false otherwise.
//   //
//   ---------------------------------------------------------------------------
//   bool StoreFactorGraph(const std::string& path);

//   //
//   ---------------------------------------------------------------------------
//   // InitSurveys
//   //
//   // Initialize the survey values of a FactorGraph with random numbers
//   // in the real interval [0,1].
//   //
//   ---------------------------------------------------------------------------
//   void InitSurveys(FactorGraph* graph);

//   //
//   ---------------------------------------------------------------------------
//   // ShuffleEdgesOrder
//   //
//   // Randomly order the edge vector of a FactorGraph.
//   //
//   ---------------------------------------------------------------------------
//   void ShuffleEdgesOrder(FactorGraph* graph);

//   //
//   ---------------------------------------------------------------------------
//   // UpdateSurvey
//   //
//   // Update survey value of an edge following equations 26 and 27.
//   //
//   ---------------------------------------------------------------------------
//   void UpdateSurvey(Edge* edge);

//   //
//   ---------------------------------------------------------------------------
//   // EvaluateVariable
//   //
//   // Evaluate a node of the factor graph with equations 28,29,30 and 31.
//   // Return the evaluation value v = (Wi+) - (Wi-) (WITH sign).
//   //
//   ---------------------------------------------------------------------------
//   float EvaluateVariable(Variable* variable);
// };

// }  // namespace sat