#include <string.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Project includes
#include <Algorithms.hpp>
#include <Configuration.hpp>
#include <FactorGraph.hpp>
#include <Utils.hpp>

using namespace std;
using namespace sat;

// ---------------------------------------------------------------------------
// GetRandomCNFFiles
//
// Get the random CNF instances with the given parameters and return an array
// of the file paths.
// CNFs are stored in DIMACS files in the experiments/instances/ folder.
// ---------------------------------------------------------------------------
vector<string> GetRandomCNFFiles(int totalInstances, int N, float alpha,
                                 const string& generator) {
  vector<string> paths;

  for (int i = 1; i <= totalInstances; i++) {
    ostringstream ss;
    string dir = "experiments/instances/";
    ss << dir << generator << "_3SAT_" << N << "N_" << alpha << "R_" << i
       << ".cnf";
    string path = ss.str();
    paths.push_back(path);
    // break;
  }

  return paths;
}

// Entry point
int main(int argc, char* argv[]) {
  // ---------------------------------------------------------------------------
  // Parse arguments
  // ---------------------------------------------------------------------------
  if (argc != 5) {
    cout << "Invalid arguments. Usage: ./experiment N A [random|community] seed"
         << endl;
    cout << "If seed = 0, random seed is used" << endl;
    return -1;
  }

  float fractionParams[1] = {0.02f};
  int totalCnfInstances = CNF_INSTANCES;
  int totalVariables = atoi(argv[1]);
  float alpha = atof(argv[2]);
  string generator = "random";
  if (argc == 4) {
    if (strcmp(argv[3], "random") == 0 || strcmp(argv[3], "community") == 0) {
      generator = argv[3];
    } else {
      cout << "Invalid cnf generator. Use random or community";
      return -1;
    }
  }
  unsigned int seed = atoi(argv[4]);

  cout << "===========================================================" << endl;
  cout << "==                RUNNING BASE EXPERIMENT                ==" << endl;
  cout << "===========================================================" << endl;
  cout << endl;
  cout << "Experiment parameters:" << endl;
  cout << " - N (variables) = " << totalVariables << endl;
  cout << " - α (clauses/variables ratio) = " << alpha << endl;
  cout << " - 3-SAT CNF generator = " << generator << endl;
  cout << " - Seed = " << seed << endl;
  cout << endl;

  cout << "Setting up experiment environment..." << endl;

  if (seed != 0)
    utils::RandomGen::setSeed(seed);
  else
    cout << "Initial seed: " << utils::RandomGen::initialSeed << endl;
  // Get random CNF instances
  vector<string> paths =
      GetRandomCNFFiles(totalCnfInstances, totalVariables, alpha, generator);

  cout << "Done!" << endl;

  // ---------------------------------------------------------------------------
  // Run experiments
  // ---------------------------------------------------------------------------
  int experimentId = 1;

  for (float fraction : fractionParams) {
    cout << endl << endl;
    cout << "------------------------------" << endl;
    cout << "Experiment " << experimentId << ":" << endl;
    cout << " - N: " << totalVariables << endl;
    cout << " - α: " << alpha << endl;
    cout << " - f: " << fraction << endl;
    cout << "------------------------------" << endl;

    int totalConvergedInstances = 0;
    int totalSATInstances = 0;
    int totalSPSATIterations = 0;
    for (string path : paths) {
      ifstream file(path);
      if (!file.is_open()) {
        cerr << "ERROR: Can't open file " << path << endl;
        break;
      } else {
        cout << "Solving file " << path << endl;
      }

      FactorGraph* graph = new FactorGraph(file);

      SIDResult result = SID(graph, fraction);
      if (result.converged) totalConvergedInstances += 1;
      if (result.SAT) {
        totalSATInstances++;
        totalSPSATIterations += result.totalSPIterations;
        cout << "Solved: SAT" << endl;
      } else {
        cout << "Solved: UNSAT" << endl;
      }
      cout << "Elapsed time = "
           << chrono::duration_cast<chrono::seconds>(result.end - result.begin)
                  .count()
           << "s" << endl;
      cout << endl;

      delete graph;
    }

    // Results
    float satInstPercent = totalSATInstances * 100.0f / totalCnfInstances;
    cout << endl;
    cout << "Results:" << endl;
    cout << " Converged Instances: " << totalConvergedInstances << endl;
    cout << " SAT instances: ";
    cout << totalSATInstances << " (" << satInstPercent << "%)" << endl;
    cout << " Total SP it. in SAT instances: " << totalSPSATIterations << endl;

    // increase experiment id
    experimentId++;

    // If all instances solved, stop experiment, if not, continue with next f
    if (totalCnfInstances == totalSATInstances) break;
  }

  return 0;
}