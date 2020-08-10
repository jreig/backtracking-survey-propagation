#include <string.h>

#include <chrono>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Project includes
#include <Configuration.hpp>
#include <FactorGraph.hpp>
#include <Solver.hpp>
#include <Validator.hpp>

using namespace sat;
using namespace std;

// -----------------------------------------------------------------------------
// Struct to save experiment parameters
// -----------------------------------------------------------------------------
struct ExperimentArgs {
  string g;
  int N;
  double a;
  unsigned int s;
  int m;
  string baseDir;
  int I = 50;
  double fractionParams[6] = {0.04, 0.02, 0.01, 0.005, 0.0025, 0.00125};
  int c = 100;
  double Q = -1;
};

// -----------------------------------------------------------------------------
// Build needed directories
// -----------------------------------------------------------------------------
void buildDirs(ExperimentArgs* args) {
  ostringstream ss;
  ss << "experiments/" << args->g << "/" << args->N << "/" << args->a;
  if (args->Q >= 0) ss << "/" << args->Q;
  string baseDir = ss.str();

  std::filesystem::create_directories(baseDir);
  std::filesystem::create_directory(baseDir + "/cnf");
  std::filesystem::create_directory(baseDir + "/cnf-solutions");

  args->baseDir = baseDir;
}

// -----------------------------------------------------------------------------
// Create cnf files
// -----------------------------------------------------------------------------
void createCNFFiles(ExperimentArgs* args) {
  for (int i = 1; i <= args->I; i++) {
    // Build file path
    ostringstream ss;
    ss << args->baseDir << "/cnf/" << i << ".cnf";
    string cnfFile = ss.str();
    unsigned int seed = i + args->m + args->s;

    // Build generator executable path with arguments and execute it
    ostringstream exe;
    if (args->g == "random") {
      exe << "./libs/cnf-generator/random -n " << args->N << " -m " << args->m
          << " -k 3 -s " << seed << " -o " << cnfFile;
    } else {
      exe << "./libs/cnf-generator/commAttach -n " << args->N << " -m "
          << args->m << " -k 3 -c " << args->c << " -Q " << args->Q << " -s "
          << seed << " -o " << cnfFile;
    }
    int retCode = system(exe.str().c_str());
    if (retCode) {
      cerr << "ERROR: cnf file creation failed" << endl;
      exit(-1);
    }
  }
}

// -----------------------------------------------------------------------------
// Parse command line arguments
// -----------------------------------------------------------------------------
ExperimentArgs* parseArgs(int argc, char* argv[]) {
  ExperimentArgs* args = new ExperimentArgs();

  // Check number of arguments
  if (argc != 5 && argc != 6) {
    cout << "Usage:" << endl;
    cout << "\t./experiment N a random seed" << endl;
    cout << "\t./experiment N a community seed Q" << endl;
    cout << "If seed = 0, random seed is used" << endl;
    exit(-1);
  }

  // Read arguments
  args->N = atoi(argv[1]);
  args->a = atof(argv[2]);
  if (strcmp(argv[3], "random") == 0 || strcmp(argv[3], "community") == 0) {
    args->g = argv[3];
    if (strcmp(argv[3], "community") == 0) {
      args->Q = atof(argv[4]);
      args->s = atoi(argv[5]);
    } else
      args->s = atoi(argv[4]);
  } else {
    cout << "Invalid cnf generator. Use random or community";
    exit(-1);
  }

  // Build derived args
  args->m = args->N * args->a;

  return args;
}

// Entry point
int main(int argc, char* argv[]) {
  // ---------------------------------------------------------------------------
  // Initialize environment
  // ---------------------------------------------------------------------------
  ExperimentArgs* args = parseArgs(argc, argv);

  cout << "===========================================================" << endl;
  cout << "==                  RUNNING  EXPERIMENT                  ==" << endl;
  cout << "===========================================================" << endl;
  cout << endl;
  cout << "Experiment parameters:" << endl;
  cout << " - N (variables) = " << args->N << endl;
  cout << " - α (clauses/variables ratio) = " << args->a << endl;
  cout << " - 3-SAT CNF generator = " << args->g << endl;
  cout << " - Seed = " << args->s << endl;
  if (args->Q >= 0) {
    cout << " - c (communities) = 100" << endl;
    cout << " - Q = " << args->Q << endl;
  }
  cout << endl;

  cout << "Setting up experiment environment..." << endl;

  buildDirs(args);
  ofstream resultFile;
  resultFile.open(args->baseDir + "/result.csv");
  if (args->Q < 0)
    resultFile << "N,a,f,sat,sp,unconv,contr,indet,totaltime\n";
  else
    resultFile << "N,a,Q,f,sat,sp,unconv,contr,indet,totaltime\n";
  resultFile.close();

  Validator validator;
  Solver solver(args->N, args->a, args->s);
  if (args->s == 0) cout << "Random seed: " << solver.initialSeed << endl;

  cout << "Generating CNF files..." << endl;
  createCNFFiles(args);

  cout << "Done!" << endl;

  // ---------------------------------------------------------------------------
  // Run experiments
  // ---------------------------------------------------------------------------
  int experimentId = 1;
  resultFile.open(args->baseDir + "/result.csv", ofstream::app);
  for (double fraction : args->fractionParams) {
    cout << endl << endl;
    cout << "------------------------------" << endl;
    cout << "Experiment " << experimentId << ":" << endl;
    cout << " - N: " << args->N << endl;
    cout << " - α: " << args->a << endl;
    if (args->Q >= 0) cout << " - Q: " << args->Q << endl;
    cout << " - f: " << fraction << endl;
    cout << "------------------------------" << endl;

    // Metrics
    int totalSATInstances = 0;
    int totalSPSATIterations = 0;
    int totalUnconvergedInstances = 0;
    int totalContradictionsInstances = 0;
    int totalIndeterminateInstances = 0;
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();

    for (int i = 1; i <= args->I; i++) {
      string path = args->baseDir + "/cnf/" + to_string(i) + ".cnf";
      ifstream file(path);
      if (!file.is_open()) {
        cerr << "ERROR: Can't open file " << path << endl;
        exit(-1);
      }
      cout << "Solving file " << path << endl;

      FactorGraph* graph = new FactorGraph(file);
      chrono::steady_clock::time_point beginSID = chrono::steady_clock::now();
      AlgorithmResult result = solver.SID(graph, fraction);
      chrono::steady_clock::time_point endSID = chrono::steady_clock::now();

      // Get result and update metrics
      if (result == SAT) {
        totalSATInstances++;
        totalSPSATIterations += solver.totalSPIterations;
        string solFile =
            args->baseDir + "/cnf-solutions/" + to_string(i) + ".cnf.sol";
        graph->storeVariableValues(solFile);
        bool valid = validator.validateResult(path, solFile);
        cout << "Solved: SAT" << endl;
        if (!valid) {
          cerr << "ERROR: Solution not valid!" << endl;
          exit(-1);
        }
      } else if (result == UNCONVERGE) {
        totalUnconvergedInstances++;
        cout << "Solved: UNCONVERGE" << endl;
      } else if (result == CONTRADICTION) {
        totalContradictionsInstances++;
        cout << "Solved: CONTRADICTION" << endl;
      } else if (result == INDETERMINATE) {
        totalIndeterminateInstances++;
        cout << "Solved: INDETERMINATE" << endl;
      }

      // Print elapsed time
      cout << "Elapsed time: "
           << chrono::duration_cast<chrono::seconds>(endSID - beginSID).count()
           << "s" << endl;
      cout << endl;

      delete graph;
    }
    chrono::steady_clock::time_point end = chrono::steady_clock::now();

    // Results
    double satInstPercent = totalSATInstances * 100.0 / args->I;
    cout << endl;
    if (args->Q < 0)
      cout << "Results [" << args->N << " - " << args->a << " - " << fraction
           << "]:" << endl;
    else
      cout << "Results [" << args->N << " - " << args->a << " - " << args->Q
           << " - " << fraction << "]:" << endl;
    cout << " SAT: ";
    cout << totalSATInstances << " (" << satInstPercent << "%)" << endl;
    cout << " SP it.: " << totalSPSATIterations << endl;
    cout << " UNCONVERGED: " << totalUnconvergedInstances << endl;
    cout << " CONTRADICTION: " << totalContradictionsInstances << endl;
    cout << " INDETERMINATE: " << totalIndeterminateInstances << endl;
    cout << " Total time: ";
    cout << chrono::duration_cast<chrono::seconds>(end - begin).count() << "s"
         << endl;
    cout << endl;

    // Store result
    if (args->Q < 0)
      resultFile << args->N << "," << args->a << "," << fraction << ",";
    else
      resultFile << args->N << "," << args->a << "," << args->Q << ","
                 << fraction << ",";
    resultFile << totalSATInstances << "," << totalSPSATIterations << ","
               << totalUnconvergedInstances << ","
               << totalContradictionsInstances << ","
               << totalIndeterminateInstances << ","
               << chrono::duration_cast<chrono::seconds>(end - begin).count()
               << "\n";

    // increase experiment id
    experimentId++;

    // If all instances solved, stop experiment, if not, continue with next f
    if (args->I == totalSATInstances) break;
  }

  resultFile.close();

  return 0;
}