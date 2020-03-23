#include <BacktrackingSP.hpp>

using namespace std;

namespace bsp {

BacktrackingSP::BacktrackingSP(ifstream& file, float threshold,
                               int maxSeconds) {
  InitParams(file, threshold, maxSeconds);
};

BacktrackingSP::BacktrackingSP(string path, float threshold, int maxSeconds) {
  ifstream file(path);
  InitParams(file, threshold, maxSeconds);
};

BacktrackingSP::~BacktrackingSP() { delete graph; }

void BacktrackingSP::InitParams(ifstream& file, float threshold,
                                int maxSeconds) {
  threshold = threshold;
  maxTime = maxSeconds * CLOCKS_PER_SEC;

  if (!file.is_open()) {
    cerr << "ERROR: Can't open file" << endl;
    graph = nullptr;
  } else {
    graph = new FactorGraph(file);
    graph->Print();
  }
}

BSPResult BacktrackingSP::IsSAT() {
  // Check correct initialization
  if (graph == nullptr) {
    cerr << "ERROR: FactorGraph not initialized" << endl;
    return ERROR;
  }

  // Initialize surveys
  random_device rd;
  mt19937 gen(rd());
  uniform_real_distribution<> dis(0, 1);
  for (Edge* edge : graph->edges) {
    edge->survey = dis(gen);
  }

  // Loop until maxTime or convergence
  bool converged = false;
  clock_t t = clock();
  while (t < maxTime && !converged) {
    // Shuffle edges
    std::shuffle(graph->edges.begin(), graph->edges.end(), gen);

    bool allConverged = true;
    for (Edge* edge : graph->edges) {
      // Survey value in the previous iteration
      float lastSurvValue = edge->survey;

      // Update survey
      UpdateSurvey(edge);

      // Check if survey converged
      bool hasConverged = abs(edge->survey - lastSurvValue) < threshold;
      if (!hasConverged) allConverged = false;
    }

    converged = allConverged;
    t = clock();
  }

  // If algorithm didn't converged means we reach max time
  if (!converged) return UNCONVERGED;

  cout << "Survey propagation converged successfully in "
       << (float)(t / CLOCKS_PER_SEC) << " seconds" << endl;

  return SAT;
};

void BacktrackingSP::UpdateSurvey(Edge* edge) {
  // FIXME!
  return;
};

}  // namespace bsp