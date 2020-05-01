#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Project includes
#include <Algorithms.hpp>
#include <FactorGraph.hpp>
#include <Utils.hpp>

using namespace std;

// ---------------------------------------------------------------------------
// GenerateRandomCNF
//
// Generate random CNF instances with the given parameters and return an array
// of the file paths.
// CNFs are stored in DIMACS files in the experiments/instances/ folder.
// ---------------------------------------------------------------------------
vector<string> GenerateRandomCNF(int totalInstances, int N, float alpha) {
  vector<string> paths;
  // cout << "Generating " << totalInstances << " random 3-SAT CNF
  // instances...";

  for (int i = 1; i <= totalInstances; i++) {
    ostringstream ss;
    string dir = "experiments/instances/";
    ss << dir << "random_3SAT_" << N << "N_" << alpha << "R_" << i << ".cnf";
    string path = ss.str();
    paths.push_back(path);
  }

  // cout << " - Done! (PREGENERATED)" << endl;

  return paths;
}

// Entry point
int main() {
  cout << "===========================================================" << endl;
  cout << "==                RUNNING BASE EXPERIMENT                ==" << endl;
  cout << "===========================================================" << endl;
  cout << endl;

  // ---------------------------------------------------------------------------
  // Experiment configuration
  //
  // TODO: Crete config file to avoid rebuild
  // ---------------------------------------------------------------------------
  // int totalVariablesParams[3] = {25000, 50000, 100000};
  // float alphaParams[4] = {4.21f, 4.22f, 4.23f, 4.24f};
  // float fractionParams[6] = {0.04f, 0.02f, 0.01f, 0.005f, 0.0025f, 0.00125f};

  // int totalCnfInstances = 50;

  // cout << "Experiment parameters:" << endl;
  // cout << " - N (variables) = 25000, 50000, 100000" << endl;
  // cout << " - α (clausules/variables ratio) = 4.21, 4.22, 4.23, 4.24" <<
  // endl; cout << " - f (assignment fraction) = 4%, 2%, 1%, .5%, .25%, .125%"
  // << endl; cout << endl;

  // FIXME! Hardcode first experiment
  int totalVariablesParams[1] = {25000};
  float alphaParams[1] = {4.21f};
  float fractionParams[1] = {0.04f};

  int totalCnfInstances = 50;

  cout << "Experiment parameters:" << endl;
  cout << " - N (variables) = 25000" << endl;
  cout << " - α (clausules/variables ratio) = 4.21" << endl;
  cout << " - f (assignment fraction) = 4%" << endl;
  cout << endl;

  // ---------------------------------------------------------------------------
  // Set up experimen
  // ---------------------------------------------------------------------------
  cout << "Setting up experiment environment...";

  sat::ParamsSP defaultParamsSP;
  sat::ParamsWalksat defaultParamsWalksat;
  sat::utils::randomGenerator.seed(1234);

  cout << " - Done!" << endl;

  // ---------------------------------------------------------------------------
  // Run experiments
  // ---------------------------------------------------------------------------
  int experimentId = 1;
  for (int totalVariables : totalVariablesParams) {
    for (float alpha : alphaParams) {
      // Get random CNF instances
      vector<string> paths =
          GenerateRandomCNF(totalCnfInstances, totalVariables, alpha);

      for (float fraction : fractionParams) {
        cout << endl << endl;
        cout << "------------------------------" << endl;
        cout << "Experiment " << experimentId << ":" << endl;
        cout << " - N: " << totalVariables << endl;
        cout << " - α: " << alpha << endl;
        cout << " - f: " << fraction << endl;
        cout << "------------------------------" << endl;

        int totalSATInstances = 0;
        sat::utils::totalSPIterations = 0;
        for (string path : paths) {
          std::ifstream file(path);
          if (!file.is_open()) {
            cerr << "ERROR: Can't open file " << path << endl;
            break;
          } else {
            cout << "Solving file " << path << endl;
          }

          sat::FactorGraph* graph = new sat::FactorGraph(file);

          bool result =
              sat::SID(graph, fraction, defaultParamsSP, defaultParamsWalksat);
          if (result) {
            totalSATInstances++;
            cout << "Solved: SAT" << endl;
          } else {
            cout << "Solved: - UNSAT" << endl;
          }

          delete graph;
        }

        // Results
        float satInstPercent = totalSATInstances * 100.0f / totalCnfInstances;
        cout << endl;
        cout << "Results:" << endl;
        cout << " SAT instances: ";
        cout << totalSATInstances << " (" << satInstPercent << "%)" << endl;
        cout << " Total SP it. in SAT instances: "
             << sat::utils::totalSPIterations << endl;

        // increase experiment id
        experimentId++;
      }
    }
  }

  return 0;
}